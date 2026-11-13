 
#include "HelloWorld.h" 
 
#include "orange.h"
   
#include <functional>
#include <string>
#include <vector>

#include <windows.h>

USING_OG;
 
Scene * HelloWorld::createScene() {
	//SetConsoleOutputCP(936);
	return HelloWorld::create();
}
bool HelloWorld::init()
{
	_defaultCamera->setZoom(0.5f);   // 缩小
	_defaultCamera->setViewportSize(Size(500, 300));
	_defaultCamera->setViewportPos(Vec2(100, 100));
	_defaultCamera->setCenter(Vec2(500, 300));   // ← 世界原点 → 视口左上角

	auto splistener = EventListenerTouchAllAtOnce::create();
	splistener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event  *event)
	{
		auto touch = touches[0];

		auto diff = touch->getDelta();
		diff.x = (float)diff.x, diff.y = (float)diff.y;
		auto node = event->getCurrentTarget();// getChildByTag(1);
		auto currentPos = node->getCenter();
		node->setCenter(currentPos + diff);

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(splistener, this->_defaultCamera);

	auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseScroll = [this](EventMouse* event) {
		//获取滚轮滚动值（ > 0 向上滚，放大）

		auto scene = (Scene*)event->getCurrentTarget();

		float scroll = event->getScrollY();
		float scaleFactor = 0.1f;
		float newScale = scene->_defaultCamera->getZoom() + (scroll > 0 ? scaleFactor : -scaleFactor);

		//限制缩放范围（例如 0.5 ~2.0）
		newScale = std::max(0.5f, std::min(newScale, 2.0f));

		//可选：以鼠标位置为中心缩放（需要坐标转换）
		//	如果不做坐标转换，地图会以锚点（默认中心）缩放
		scene->_defaultCamera->setZoom(newScale);


	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

	uiTest();

	return true;
}

// ============================================================
// draw（保留原相机框绘制）
// ============================================================
void HelloWorld::draw(Renderer* renderer, const Mat3& transform, uint32_t flags)
{
	auto camera = Camera::getVisitingCamera();
	if (!camera) return;

	Rect screenRect = camera->getScreenRect();

	SDL_Renderer* sdlRen = SDLView::getInstance()->getRender();
	if (!sdlRen) return;

	SDL_Rect r1 = {
		(int)screenRect.origin.x,
		(int)screenRect.origin.y,
		(int)screenRect.size.width,
		(int)screenRect.size.height
	};
	SDL_Rect r2 = {
		(int)screenRect.origin.x - camera->getMarginX(),
		(int)screenRect.origin.y - camera->getMarginY(),
		(int)screenRect.size.width + camera->getMarginX() * 2,
		(int)screenRect.size.height + camera->getMarginY() * 2
	};
	SDL_SetRenderDrawColor(sdlRen, 255, 0, 0, 255);
	SDL_RenderDrawRect(sdlRen, &r1);
	SDL_RenderDrawRect(sdlRen, &r2);
}