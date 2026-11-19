#ifndef _TEST_H_
#define _TEST_H_

#include "2d/OGNode.h" 
#include "2d/OGScene.h"
#include "2d/OGDrawNode.h"
USING_OG;

class Test :public Scene{
public:
#if OG_USE_PHYSICS
	CREATE_PHYSICS_FUNC(Test);
#else
	CREATE_FUNC(Test);
#endif
	 static orange::Scene* createScene();      // ← 返回 Scene
	 void control_init();
	 void test1();
#if OG_USE_PHYSICS
	 cpBody*   _ballBody = nullptr;
#endif
	 float   _ballRadius = 20;
	 DrawNode* _debugDraw = nullptr;
	 void test2();
 
	 void testHttp();

	 void testNetwork();

	 bool init();
	void menuCloseCallback(Ref * pSender);
	Test(){}
	void update(float dt);
	void draw(Renderer * renderer, const Mat3 & transform, uint32_t flags);
	 
};

#endif