#ifndef _OG_DIRECTOR_H_
#define _OG_DIRECTOR_H_
 

#include <string>
#include <unordered_map>
#include "OGPlatformMacros.h"
#include "OGNode.h"
#include "OGScheduler.h"
#include "OGCamera.h"

OG_BEGIN

class FileUtils; 
class EventDispatcher;
class Texture2D;

// ==================== TextureCache ====================
 
class TextureCache
{
public:
    TextureCache() = default;
    ~TextureCache() { removeAllTextures(); }

    // 命中缓存直接返回，否则 loadFrom 加载
	Texture2D* addImage(const std::string& filepath);

    // 从内存加载（zip / 打包资源用）
	Texture2D* addImageFromMemory(const std::string& key,
		unsigned char* data, int len);

    // 主动移除
	void removeTexture(const std::string& key);
	void removeAllTextures();

	void removeUnusedTextures();
	bool reloadTexture(const std::string & fileName);
private:
    std::unordered_map<std::string, Texture2D*> _textures;
	
};

class Data;
class FileCache {
public:
	Data addFile(const std::string &file);
};

// ==================== Director ====================

class SDLView;
class Renderer;

class Node;
class Scheduler;
class ActionManager;
class EventDispatcher;
class PoolManager;

class Director:public Ref
{
protected:
	Node *_curScene, *_nextScene;
public:
	static SDLView *_sdlView;
	static   EventDispatcher   *_eventDispatcher;
	static Scheduler *_scheduler;
	Instance(Director);

	Vec2  convertToWorld(const Vec2& uiPoint)
	{ 
		Camera* currentCamera = Camera::getVisitingCamera();

		if (!currentCamera)
		{
			return uiPoint;
		}
		Vec2 worldPoint = uiPoint;
		auto m = currentCamera->getViewMatrix();
		if (fabsf(m.a * m.d - m.b * m.c) > 1e-4f)
		{
			Mat3 transform = m.getInversed();
			transform.out(&worldPoint);
		} 
		return worldPoint;
	}

    TextureCache* getTextureCache() { return &_textureCache; }
	FileCache* getFileCache() { return &_fileCache; }
	EventDispatcher* getEventDispatcher() const { return _eventDispatcher; }
	Scheduler* getScheduler() { return _scheduler; }
    // 数据目录（你项目里可能已有，这里只是占位）
    void setResourceRoot(const std::string& path) { _resourceRoot = path; }
    const std::string& getResourceRoot() const { return _resourceRoot; }
	SDLView *getSDLView() {
		return _sdlView;
	}
	void setScene(Node *scene);
	void MainLoop();
private:
	bool  init() { return true; }
	Director();
    ~Director() = default;
    Director(const Director&) = delete;
    Director& operator=(const Director&) = delete;
	
    TextureCache _textureCache;
	FileCache _fileCache;
    std::string  _resourceRoot;
};
 

OG_END

#endif