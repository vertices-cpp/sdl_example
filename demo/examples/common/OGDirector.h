#ifndef _OG_DIRECTOR_H_
#define _OG_DIRECTOR_H_
 
#include "OGFileUtils.h" 
#include <string>
#include <unordered_map>

OG_BEGIN



// ==================== TextureCache ====================
class Texture2D;
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

// ==================== Director ====================

class SDLView;
class Renderer;

class Node;
class Scheduler;
class ActionManager;
class EventDispatcher;
class PoolManager;

class Director
{
protected:
	
public:
	static SDLView *_sdlView;
	static   EventDispatcher   *_eventDispatcher;
    static Director* getInstance()
    {
        static Director inst;
        return &inst;
    }

    TextureCache* getTextureCache() { return &_textureCache; }
	EventDispatcher* getEventDispatcher() const { return _eventDispatcher; }
    // 数据目录（你项目里可能已有，这里只是占位）
    void setResourceRoot(const std::string& path) { _resourceRoot = path; }
    const std::string& getResourceRoot() const { return _resourceRoot; }
	SDLView *getOpenSDLView() {
		return _sdlView;
	}
private:
	Director();
    ~Director() = default;
    Director(const Director&) = delete;
    Director& operator=(const Director&) = delete;
	
    TextureCache _textureCache;
    std::string  _resourceRoot;
};
 

OG_END

#endif