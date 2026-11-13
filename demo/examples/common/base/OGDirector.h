#ifndef _OG_DIRECTOR_H_
#define _OG_DIRECTOR_H_
 

#include <string>
#include <unordered_map>
#include "platform/SDLView.h"
#include "platform/OGPlatformMacros.h"
#include "2d/OGScene.h"
#include "base/OGScheduler.h"
#include "2d/OGCamera.h"
#include "renderer/OGTextureCache.h"

OG_BEGIN

class FileUtils; 
class EventDispatcher;
class Texture2D;
 
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
	Scene *_runningScene, *_nextScene;
public:

	/** Director will trigger an event before set next scene. */
	static const char* EVENT_BEFORE_SET_NEXT_SCENE;
	/** Director will trigger an event after set next scene. */
	static const char* EVENT_AFTER_SET_NEXT_SCENE;

	/** Director will trigger an event when projection type is changed. */
	static const char* EVENT_PROJECTION_CHANGED;
	/** Director will trigger an event before Schedule::update() is invoked. */
	static const char* EVENT_BEFORE_UPDATE;
	/** Director will trigger an event after Schedule::update() is invoked. */
	static const char* EVENT_AFTER_UPDATE;
	/** Director will trigger an event while resetting Director */
	static const char* EVENT_RESET;
	/** Director will trigger an event after Scene::render() is invoked. */
	static const char* EVENT_AFTER_VISIT;
	/** Director will trigger an event after a scene is drawn, the data is sent to GPU. */
	static const char* EVENT_AFTER_DRAW;
	/** Director will trigger an event before a scene is drawn, right after clear. */
	static const char* EVENT_BEFORE_DRAW;

	static SDLView *_sdlView;
	static   EventDispatcher   *_eventDispatcher;
	static Scheduler *_scheduler;
	static Renderer *_renderer;
	//static Camera * _camera;
	static ActionManager *_actionManager;
	static Director *getInstance() { 
			static Director instance_;  
			return &instance_;  
	}
	void init();
	Size getWinSize() {
		return _sdlView->getWindowSize();
	}
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
	ActionManager *  getActionManager() const { return _actionManager; }
    TextureCache* getTextureCache() { return &_textureCache; }
	//FileCache* getFileCache() { return &_fileCache; }
	EventDispatcher* getEventDispatcher() const { return _eventDispatcher; }
	Scheduler* getScheduler() { return _scheduler; }
    // 数据目录（项目里可能已有，这里只是占位）
    void setResourceRoot(const std::string& path) { _resourceRoot = path; }
    const std::string& getResourceRoot() const { return _resourceRoot; }
	SDLView *getSDLView() {
		return _sdlView;
	}
	void runWithScene(Scene *scene);
//	Camera * getCamera();
	Scene *getRunningScene() { return _runningScene; }
	Vec2 convertToUI(const Vec2 & worldPoint);
	bool EventHandle();
	void mainLoop();
	Renderer * getRenderer() const;
private: 
	Director();

    ~Director() = default;
    Director(const Director&) = delete;
    Director& operator=(const Director&) = delete;
	
    TextureCache _textureCache;
	//FileCache _fileCache;
    std::string  _resourceRoot;
};
 

OG_END

#endif