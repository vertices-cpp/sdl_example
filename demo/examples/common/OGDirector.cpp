#include "OGDirector.h"
#include "OGTexture2D.h" 
#include "SDLView.h"
#include "OGEventDispatcher.h"
#include "OGFileUtils.h" 
#include "OGData.h"
#include "OGCamera.h"

#include "OGActionManager.h"


OG_BEGIN



// ==================== TextureCache ====================
 
    // 命中缓存直接返回，否则 loadFrom 加载
    Texture2D* TextureCache::addImage(const std::string& filepath)
    {
        // 1. 先查缓存
        auto it = _textures.find(filepath);
        if (it != _textures.end())
            return it->second;

        // 2. 解析真实路径
        std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filepath);
        if (fullPath.empty())
        {
            printf("[TextureCache] file not found: %s\n", filepath.c_str());
            return nullptr;
        }

        // 3. 创建并加载
        Texture2D* tex = new (std::nothrow) Texture2D();
        if (!tex) return nullptr;

        tex->loadFrom(fullPath.c_str());

        if (!tex->getTexture())
        {
            printf("[TextureCache] load failed: %s\n", fullPath.c_str());
            delete tex;
            return nullptr;
        }

        tex->retain();                  // 由 cache 持有
        _textures[filepath] = tex;
        return tex;
    }

    // 从内存加载（zip / 打包资源用）
    Texture2D* TextureCache::addImageFromMemory(const std::string& key,
                                  unsigned char* data, int len)
    {
        auto it = _textures.find(key);
        if (it != _textures.end())
            return it->second;

        Texture2D* tex = new (std::nothrow) Texture2D();
        if (!tex) return nullptr;

        if (!tex->loadMemData(data, len))
        {
            delete tex;
            return nullptr;
        }

        tex->retain();
        _textures[key] = tex;
        return tex;
    }

    // 主动移除
    void TextureCache::removeTexture(const std::string& key)
    {
        auto it = _textures.find(key);
        if (it == _textures.end()) return;
        it->second->release();
        _textures.erase(it);
    }

    void TextureCache::removeAllTextures()
    {
        for (auto& kv : _textures)
            kv.second->release();
        _textures.clear();
    }

    void TextureCache::removeUnusedTextures()
    {
        for (auto it = _textures.begin(); it != _textures.end(); )
        {
            if (it->second->getReferenceCount() == 1)
            {
                it->second->release();
                it = _textures.erase(it);
            }
            else ++it;
        }
    }
	 
	bool TextureCache::reloadTexture(const std::string& fileName)
	{
		std::string fullpath = FileUtils::getInstance()->fullPathForFilename(fileName);
		if (fullpath.empty()) return false;

		auto it = _textures.find(fullpath);
		if (it != _textures.end())
		{
			// ★ 直接重载这张纹理
			it->second->loadFrom(fullpath.c_str());
			return it->second->getTexture() != nullptr;
		}

		// 不在缓存里 → 新加
		return addImage(fullpath) != nullptr;
	}


	Data FileCache::addFile(const std::string &file)
	{
		Data data;
		std::string fullpath = FileUtils::getInstance()->fullPathForFilename(file);
		if (fullpath.size() == 0)
		{
			return data;
		}
		data = FileUtils::getInstance()->getDataFromFile(fullpath);
	 
		return data;
	}
// ==================== Director ====================

	SDLView *Director::_sdlView = nullptr;
	EventDispatcher *Director::_eventDispatcher = nullptr;
	Scheduler *Director::_scheduler = nullptr;
	Camera * Director::_camera = nullptr;
	ActionManager *Director::_actionManager = nullptr;

	Director::Director(){
		OG_SAFE_RELEASE(_eventDispatcher);
		OG_SAFE_RELEASE(_scheduler);
		OG_SAFE_RELEASE(_camera);
		OG_SAFE_RELEASE(_actionManager); 
	}
	void Director::init()
	{
		//在init单独操作中初始化,防止互相构造产生的死锁
		_sdlView = SDLView::getInstance();
		_sdlView->init();
		
		 
		_eventDispatcher =new EventDispatcher;
		_eventDispatcher->setEnabled(true);

		_scheduler = new Scheduler;

		_actionManager = new ActionManager;
		_scheduler->scheduleUpdate(_actionManager, Scheduler::PRIORITY_SYSTEM, false);
		_camera = Camera::getInstance();

	}

	void Director::setScene(Node * scene)
	{
		_nextScene = scene;
	}
	Camera * Director::getCamera()
	{
		return _camera;
	}
	void Director::MainLoop()
	{ 
		
		auto renderer = Renderer::getInstance();
		renderer->init(_sdlView->getRender());

		while (!_sdlView->event())
		{
			Camera::_visitingCamera = _camera;
			_scheduler->update(0.2f);
			if (_nextScene != nullptr)
			{
				_curScene = _nextScene; 
				_nextScene = nullptr;
			}

			_sdlView->cls();
			_curScene->visit(renderer, _camera->getViewMatrix(), _camera->isViewProjectionUpdated());
			renderer->render();
			_sdlView->refresh();

		}
	}

OG_END

