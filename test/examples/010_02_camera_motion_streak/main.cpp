
#if defined(_WIN32)||defined(_WIN64)
#include <SDL.h>  
#else
#include <SDL2/SDL.h> 
#endif
#include <cmath>
#include "path_head.h"

#include "SDLView.h" 
#include "MotionStreak.h"
using namespace orange;
/* 
   #include <chrono> #include <thread> #include<functional> #include<sstream> */

using namespace std;

int main(int, char **)
{
	SDLView *e = SDLView::getInstance();
	e->init(0, 0, 1000, 1000);

	auto cam = Camera::create();
	Camera::_visitingCamera  = cam;
	cam->setShowDebug(true);

	cam->setZoom(0.5f);   // 缩小一半
	cam->setViewportSize(Size(500, 400));
	cam->setViewportPos(Vec2(100, 100));
	cam->setCenter(Vec2(500, 400));   // ← 世界原点 → 视口左上角
 
	auto  motionSk = MotionStreak::create(
		5.4f, 5.0f, 32.0f,
		{ 0, 128, 255, 255 }, og::checkPath("010_01_motion_streak/circle_texture.png")
	);

	motionSk->addPoint(cam->getScreenRect().size +Size(100,100));

	while (!e->event())
	{
		e->cls();

		Vec2 screenPos = e->GetMousePos();
		Rect vp = cam->getScreenRect();

		// ★ 一句话：内部完成变化检测 + 控制 s_verbose
		MotionStreak::tickMouseState(screenPos, vp);

		Mat3 clipTrans = cam->getViewMatrix().getInversed();
		Vec2 pos = screenPos;
		clipTrans.out(&pos);

		motionSk->addPoint(pos);
		motionSk->update(0.2f);
		motionSk->draw(e->getRender(), cam->getViewMatrix(), 0);

		cam->updateDebugDraw();
		e->refresh();
	}
	return 0;
}
