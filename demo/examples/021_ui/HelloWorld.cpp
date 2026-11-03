 
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