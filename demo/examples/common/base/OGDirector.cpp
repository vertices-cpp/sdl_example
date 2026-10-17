#include "base/OGDirector.h"
#include "renderer/OGTexture2D.h" 
#include "platform/SDLView.h"
#include "base/OGEventDispatcher.h"
#include "platform/OGFileUtils.h" 
#include "base/OGData.h"
#include "2d/OGCamera.h"

#include "2d/OGActionManager.h"
#include "renderer/OGRenderer.h"

#include <chrono>

OG_BEGIN
 
// ==================== Director ====================

	const char *Director::EVENT_BEFORE_SET_NEXT_SCENE = "director_before_set_next_scene";
	const char *Director::EVENT_AFTER_SET_NEXT_SCENE = "director_after_set_next_scene";
	const char *Director::EVENT_PROJECTION_CHANGED = "director_projection_changed";
	const char *Director::EVENT_AFTER_DRAW = "director_after_draw";
	const char *Director::EVENT_AFTER_VISIT = "director_after_visit";
	const char *Director::EVENT_BEFORE_UPDATE = "director_before_update";
	const char *Director::EVENT_AFTER_UPDATE = "director_after_update";
	const char *Director::EVENT_RESET = "director_reset";
	const char *Director::EVENT_BEFORE_DRAW = "director_before_draw";


	SDLView *Director::_sdlView = nullptr;
	EventDispatcher *Director::_eventDispatcher = nullptr;
	Scheduler *Director::_scheduler = nullptr;
 
	ActionManager *Director::_actionManager = nullptr;
	Renderer *Director::_renderer = nullptr;

	Director::Director(){
		OG_SAFE_RELEASE(_eventDispatcher);
		OG_SAFE_RELEASE(_scheduler);
		//OG_SAFE_RELEASE(_camera);
		OG_SAFE_RELEASE(_actionManager); 
		delete _renderer;
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
		_renderer = Renderer::getInstance();
		//_camera = Camera::getInstance();

	}

	void Director::setScene(Scene * scene)
	{
		_nextScene = scene;
	}
// 	Camera * Director::getCamera()
// 	{
// 		return _camera;
// 	}
	bool Director::EventHandle() {

		return true;

	}
	void Director::mainLoop()
	{ 

		
		auto renderer = Renderer::getInstance();
		renderer->init(_sdlView->getRender());

		std::chrono::steady_clock::time_point _lastUpdate;

		while (!_sdlView->event())
		{
			// 算 deltaTime
			auto now = std::chrono::steady_clock::now();
			auto _deltaTime = std::chrono::duration<float>(now - _lastUpdate).count();
			_lastUpdate = now;

			// 或者
			_deltaTime = 1.0f / 60.0f;   // 固定步长

		 
			_scheduler->update(_deltaTime);
			if (_nextScene != nullptr)
			{
				_curScene = _nextScene; 
				_curScene->onEnter();
				_nextScene = nullptr;

			}
#if (OG_USE_PHYSICS )
			_curScene->stepPhysicsAndNavigation(_deltaTime);
#endif
			_sdlView->cls();
			_curScene->render(renderer);
			renderer->render();
			_sdlView->refresh();

		}
	}
	Renderer * Director::getRenderer() const { return _renderer; }
OG_END

