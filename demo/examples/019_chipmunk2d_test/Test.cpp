/** Use physics integration API. */
#ifndef OG_USE_PHYSICS
#define OG_USE_PHYSICS 1
#endif

#include "Test.h" 
#include "2d/OGSprite.h"
#include "base/OGDirector.h"
//#include "OGScene.h" 
#include "2d/OGCamera.h"
#include "base/OGEventDispatcher.h"
#include "base/OGEventListenerTouch.h"
#include "platform/OGFileUtils.h"
#include "physics/OGPhysicsWorld.h"
#include "2d/OGDrawNode.h"
#include "base/OGTouch.h"
#include "chipmunk/chipmunk.h"

#define PATH_RES
#include "path_head.h"

USING_OG;

Scene* Test::createScene()
{
	auto scene = Test::createWithPhysics();   // ← 直接创建
	scene->getPhysicsWorld()->setGravity(Vec2(0, 300));
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
 
	return scene;
}
void Test::control_init()
{
	auto visibleSize = Director::getInstance()->getWinSize();
	// ===== 1. 相机 =====
// 	auto camera = _defaultCamera;
// 	_defaultCamera->setZoom(0.5f);   // 缩小一半
// 	_defaultCamera->setViewportSize(Size(500, 400));
// 	_defaultCamera->setViewportPos(Vec2(300, 200));
// 	_defaultCamera->setCenter(Vec2(500, 400));   // ← 世界原点 → 视口左上角

	auto splistener = EventListenerTouchAllAtOnce::create();
	splistener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event  *event)
	{
		auto touch = touches[0];

		auto diff = touch->getDelta();
		diff.x = (float)diff.x, diff.y = (float)diff.y;
		auto node = (Camera*)event->getCurrentTarget();// getChildByTag(1);
		auto currentPos = node->getCenter();
		node->setCenter(currentPos + diff);

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(splistener, this->_defaultCamera);

}
void Test::test1() {
	auto visibleSize = Director::getInstance()->getWinSize();

	auto draw = DrawNode::create();
	addChild(draw);
	{
		Vec2 tri[3] = {
			Vec2(100, 100),
			Vec2(200, 100),
			Vec2(150, 200)
		};

		draw->drawPolygon(
			tri, 3,
			Color4F::RED,          // 填充
			2.0f,                  // 边框宽
			Color4F::WHITE);       // 边框色
	}
	//  
	//  	// ===== 2. 画矩形 =====
	//  	{
	//  		Vec2 rect[4] = {
	//  			Vec2(10, 10),
	//  			Vec2(110, 10),
	//  			Vec2(10, 110),
	//  			Vec2(110, 110)
	//  		};
	//  
	//  		draw->drawPolygon(
	//  			rect, 4,
	//  			Color4F(0, 1, 0, 0.5f),   // 半透明绿
	//  			2.0f,
	//  			Color4F::WHITE);
	//  	}

		// ===== 3. 画圆（Chipmunk 调试那种）=====
	{
		//  		const int SEG = 32;
		//  		Vec2 circle[SEG];
		//  		Vec2 center(600, 300);
		//  		float radius = 80;
		//  
		//  		for (int i = 0; i < SEG; ++i)
		//  		{
		//  			float a = (float)i / SEG * 6.2831853f;
		//  			circle[i] = Vec2(center.x + cosf(a) * radius,
		//  				center.y + sinf(a) * radius);
		//  		}
		//  
		//  		draw->drawPolygon(
		//  			circle, SEG,
		//  			Color4F(0, 0.5f, 1.0f, 0.3f),  // 淡蓝填充
		//  			2.0f,
		//  			Color4F::BLUE);                 // 蓝边
	}

	// 地面
	{
		float groundY = visibleSize.height - 300;   // 窗口底部再往下 100
		auto edge = PhysicsBody::createEdgeSegment(
			Vec2(0, groundY), Vec2(visibleSize.width, groundY));
		auto node = Node::create();
		node->setPhysicsBody(edge);
		addChild(node);
	}

	// 方块
	{
		auto body = PhysicsBody::createBox(Size(40, 40));
		body->setDynamic(true);

		auto sprite = Sprite::create("girl.png");
		sprite->setPosition(0, 0);   // 从窗口底部开始
		sprite->setPhysicsBody(body);
		addChild(sprite);

		body->setVelocity(Vec2(100, 98));
	}

	// 圆
	{
		auto body = PhysicsBody::createCircle(20);
		body->setDynamic(true);

		auto sprite = Sprite::create("girl.png");
		sprite->setPosition(visibleSize.width / 2 + 80, 0);
		sprite->setPhysicsBody(body);
		addChild(sprite);
	}
}

 
void Test::test2()
{
	auto visibleSize = Director::getInstance()->getWinSize();
	auto world = this->getPhysicsWorld();      // Scene 的物理世界
	if (!world) return;
	float W = visibleSize.width;
	float H = visibleSize.height;

	// ============================================================
   // 1. 斜坡：(0, H*0.3) → (W*0.7, H*0.6)
   // ============================================================
	{
		auto edge = PhysicsBody::createEdgeSegment(
			Vec2(0, H * 0.5f),
			Vec2(W * 0.7f, H * 0.3f),
			PhysicsMaterial(0.1f, 0.0f, 1.0f));
		auto node = Node::create();
		node->setPhysicsBody(edge);
		addChild(node);
	}

	// ============================================================
	// 2. 底部平台：(W*0.7, H*0.6) → (W, H*0.6)
	// ============================================================
	{
		auto edge = PhysicsBody::createEdgeSegment(
			Vec2(W * 0.7f, H * 0.6f),
			Vec2(W, H * 0.6f),
			PhysicsMaterial(0.1f, 0.0f, 1.0f));
		auto node = Node::create();
		node->setPhysicsBody(edge);
		addChild(node);
	}

	// ============================================================
	// 3. 圆球
	// ============================================================
	{
		auto body = PhysicsBody::createCircle(
			_ballRadius,
			PhysicsMaterial(1.0f, 0.3f, 1.0f));
		body->setDynamic(true);
		body->setGravityEnable(true);
		body->setRotationEnable(true);

		auto sprite = Sprite::create("girl.png");
		sprite->setTextureRect(Rect(0, 0, _ballRadius * 2, _ballRadius * 2));
		sprite->setColor(Color3B::RED);
		sprite->setPosition(Vec2(W * 0.15f, H * 0.1f));
		sprite->setPhysicsBody(body);
		addChild(sprite);
	}

	// ============================================================
	// 4. DrawNode：画斜坡和平台
	// ============================================================
	_debugDraw = DrawNode::create();
	addChild(_debugDraw);
	_debugDraw->drawSegment(
		Vec2(0, H * 0.3f),
		Vec2(W * 0.7f, H * 0.6f),
		4.0f, Color4F::WHITE);
	_debugDraw->drawSegment(
		Vec2(W * 0.7f, H * 0.6f),
		Vec2(W, H * 0.6f),
		4.0f, Color4F::WHITE);
}

bool Test::init()
{ 

	auto visibleSize = Director::getInstance()->getWinSize();

	control_init();
	test2();


 	

 	 
	return true;
} 

void Test::update(float dt)
{
	if (!_debugDraw || !_ballBody) return;

	// 清掉上一帧的球（只清球，保留斜坡）
	_debugDraw->clear();

	// 重画斜坡
	_debugDraw->drawSegment(Vec2(0, 300), Vec2(1200, 600), 4.0f, Color4F::WHITE);
	_debugDraw->drawSegment(Vec2(1200, 600), Vec2(2000, 600), 4.0f, Color4F::WHITE);

	// 画球（位置从物理 body 拿）
	cpVect p = cpBodyGetPosition(_ballBody);
	_debugDraw->drawFillCircle(Vec2(p.x, p.y), _ballRadius, 24, Color4F::RED);
	_debugDraw->drawCircle(Vec2(p.x, p.y), _ballRadius, 2.0f, Color4F::WHITE);
}

void Test::draw(Renderer* renderer, const Mat3& transform, uint32_t flags)
{ 

	auto camera = Camera::getVisitingCamera();
	if (!camera) return;

	//// 屏幕矩形直接画（因为 draw 的 transform 是"局部 -> 屏幕"，Screen 坐标就是屏幕）
	Rect screenRect = camera->getScreenRect();

	SDL_Renderer* sdlRen = SDLView::getInstance()->getRender();
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