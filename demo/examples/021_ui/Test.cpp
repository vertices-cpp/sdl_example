/** Use physics integration API. */
#ifndef OG_USE_PHYSICS
#define OG_USE_PHYSICS 1
#endif
#include "network/HttpClient.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"

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
#include "ui/UILoadingBar.h"

#include <vector>
// #define PATH_RES
// #include "path_head.h"

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
#include "network/HttpClient.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"

USING_OG;

void Test::testHttp()
{
	auto request = new network::HttpRequest();
	request->setUrl("http://httpbin.org/get");
	request->setRequestType(network::HttpRequest::Type::GET);
	request->setResponseCallback([](network::HttpClient* client, network::HttpResponse* response)
	{
		if (!response->isSucceed())
		{
			OGLOG("HTTP 失败: %s", response->getErrorBuffer());
			return;
		}

		std::vector<char>* buffer = response->getResponseData();
		std::string data(buffer->begin(), buffer->end());
		OGLOG("HTTP 响应: %s", data.c_str());
	});

	network::HttpClient::getInstance()->send(request);
	request->release();   // 你之前报错 release，现在 Ref 修好了应该有
}

void Test::testNetwork()
{
	auto http = network::HttpClient::getInstance();

	// ============================================================
	// 1. GET 请求
	// ============================================================
	{
		auto req = new network::HttpRequest();
		req->setUrl("http://httpbin.org/get");
		req->setRequestType(network::HttpRequest::Type::GET);
		req->setResponseCallback([](network::HttpClient* c, network::HttpResponse* r)
		{
			if (!r->isSucceed())
			{
				OGLOG("[1] GET 失败: %s", r->getErrorBuffer());
				return;
			}
			auto* buf = r->getResponseData();
			std::string data(buf->begin(), buf->end());
			OGLOG("[1] GET 成功, 长度=%zu", data.size());
		});
		http->send(req);
		req->release();
	}

	// ============================================================
	// 2. GET 带参数
	// ============================================================
	{
		auto req = new network::HttpRequest();
		req->setUrl("http://httpbin.org/get?name=hello&age=20");
		req->setRequestType(network::HttpRequest::Type::GET);
		req->setResponseCallback([](network::HttpClient* c, network::HttpResponse* r)
		{
			if (!r->isSucceed()) { OGLOG("[2] 失败: %s", r->getErrorBuffer()); return; }
			auto* buf = r->getResponseData();
			std::string data(buf->begin(), buf->end());
			OGLOG("[2] 带参数 GET 成功, 响应=%s", data.c_str());
		});
		http->send(req);
		req->release();
	}

	// ============================================================
	// 3. POST 表单
	// ============================================================
	{
		auto req = new network::HttpRequest();
		req->setUrl("http://httpbin.org/post");
		req->setRequestType(network::HttpRequest::Type::POST);

		const char* body = "name=hello&age=20";
		req->setRequestData(body, strlen(body));

		req->setResponseCallback([](network::HttpClient* c, network::HttpResponse* r)
		{
			if (!r->isSucceed()) { OGLOG("[3] 失败: %s", r->getErrorBuffer()); return; }
			auto* buf = r->getResponseData();
			std::string data(buf->begin(), buf->end());
			OGLOG("[3] POST 表单成功, 响应=%s", data.c_str());
		});
		http->send(req);
		req->release();
	}

	// ============================================================
	// 4. POST JSON
	// ============================================================
	{
		auto req = new network::HttpRequest();
		req->setUrl("http://httpbin.org/post");
		req->setRequestType(network::HttpRequest::Type::POST);

		const char* json = R"({"name":"hello","age":20})";
		req->setRequestData(json, strlen(json));

		std::vector<std::string> headers;
		headers.push_back("Content-Type: application/json");
		req->setHeaders(headers);

		req->setResponseCallback([](network::HttpClient* c, network::HttpResponse* r)
		{
			if (!r->isSucceed()) { OGLOG("[4] 失败: %s", r->getErrorBuffer()); return; }
			auto* buf = r->getResponseData();
			std::string data(buf->begin(), buf->end());
			OGLOG("[4] POST JSON 成功, 响应=%s", data.c_str());
		});
		http->send(req);
		req->release();
	}

	// ============================================================
	// 5. 自定义 Header
	// ============================================================
	{
		auto req = new network::HttpRequest();
		req->setUrl("http://httpbin.org/headers");
		req->setRequestType(network::HttpRequest::Type::GET);

		std::vector<std::string> headers;
		headers.push_back("X-Custom-Header: 12345");
		headers.push_back("User-Agent: OrangeTest/1.0");
		req->setHeaders(headers);

		req->setResponseCallback([](network::HttpClient* c, network::HttpResponse* r)
		{
			if (!r->isSucceed()) { OGLOG("[5] 失败: %s", r->getErrorBuffer()); return; }
			auto* buf = r->getResponseData();
			std::string data(buf->begin(), buf->end());
			OGLOG("[5] 自定义 Header 成功, 响应=%s", data.c_str());
		});
		http->send(req);
		req->release();
	}

	// ============================================================
	// 6. 404 错误
	// ============================================================
	{
		auto req = new network::HttpRequest();
		req->setUrl("http://httpbin.org/status/404");
		req->setRequestType(network::HttpRequest::Type::GET);
		req->setResponseCallback([](network::HttpClient* c, network::HttpResponse* r)
		{
			if (!r->isSucceed())
			{
				OGLOG("[6] 404 错误: 成功捕获, error=%s", r->getErrorBuffer());
				return;
			}
			OGLOG("[6] 404 竟然成功了？状态码=%d", r->getResponseCode());
		});
		http->send(req);
		req->release();
	}

	// ============================================================
	// 7. 超时
	// ============================================================
	{
		// ★ 打印改之前
		OGLOG("[7] timeout before: read=%d, connect=%d",
			network::HttpClient::getInstance()->getTimeoutForRead(),
			network::HttpClient::getInstance()->getTimeoutForConnect());

		network::HttpClient::getInstance()->setTimeoutForConnect(3);
		network::HttpClient::getInstance()->setTimeoutForRead(3);

		// ★ 打印改之后
		OGLOG("[7] timeout after: read=%d, connect=%d",
			network::HttpClient::getInstance()->getTimeoutForRead(),
			network::HttpClient::getInstance()->getTimeoutForConnect());

		auto req = new network::HttpRequest();
		req->setUrl("http://httpbin.org/delay/10");
		req->setRequestType(network::HttpRequest::Type::GET);

		req->setResponseCallback([](network::HttpClient* c, network::HttpResponse* r)
		{
			// 回调里改回默认
			network::HttpClient::getInstance()->setTimeoutForConnect(30);
			network::HttpClient::getInstance()->setTimeoutForRead(60);

			if (!r->isSucceed())
			{
				OGLOG("[7] 超时成功捕获: %s", r->getErrorBuffer());
				return;
			}
			OGLOG("[7] 没超时");
		});
		http->send(req);
		req->release();
	}

	// ============================================================
	// 8. 并发 10 个请求
	// ============================================================
	{
		for (int i = 0; i < 10; ++i)
		{
			auto req = new network::HttpRequest();
			std::string url = "http://httpbin.org/get?id=" + std::to_string(i);
			req->setUrl(url);
			req->setRequestType(network::HttpRequest::Type::GET);
			req->setResponseCallback([i](network::HttpClient* c, network::HttpResponse* r)
			{
				if (!r->isSucceed()) { OGLOG("[8-%d] 失败", i); return; }
				OGLOG("[8-%d] 并发成功", i);
			});
			http->send(req);
			req->release();
		}
	}

	// ============================================================
	// 9. 重定向
	// ============================================================
	{
		auto req = new network::HttpRequest();
		req->setUrl("http://httpbin.org/redirect/3");   // 重定向 3 次
		req->setRequestType(network::HttpRequest::Type::GET);
		req->setResponseCallback([](network::HttpClient* c, network::HttpResponse* r)
		{
			if (!r->isSucceed()) { OGLOG("[9] 失败: %s", r->getErrorBuffer()); return; }
			OGLOG("[9] 重定向成功");
		});
		http->send(req);
		req->release();
	}

	// ============================================================
	// 10. HTTPS（如果支持）
	// ============================================================
	{
		auto req = new network::HttpRequest();
		req->setUrl("https://httpbin.org/get");
		req->setRequestType(network::HttpRequest::Type::GET);
		req->setResponseCallback([](network::HttpClient* c, network::HttpResponse* r)
		{
			if (!r->isSucceed())
			{
				OGLOG("[10] HTTPS 失败（可能没链 SSL）: %s", r->getErrorBuffer());
				return;
			}
			OGLOG("[10] HTTPS 成功");
		});
		http->send(req);
		req->release();
	}
}
bool Test::init()
{ 

	auto visibleSize = Director::getInstance()->getWinSize();

// 	control_init();
// 	test2();

	testNetwork();

	// 背景图（加载栏底图）
	auto bg = Sprite::create("loadingbar.png");
	bg->setPosition(Vec2(480, 320));
	this->addChild(bg);
	bg->setGlobalZOrder(1);
	bg->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
	Director::getInstance()->getSDLView()->setRenderBlended();
	// 加载栏
	auto bar = ui::LoadingBar::create("fill.png");
	if (bar)
	{
		bar->setPosition(Vec2(480, 320));
		bar->setDirection(ui::LoadingBar::Direction::LEFT);
		bar->setPercent(0);
		this->addChild(bar);
		
		// 每秒涨 10%
		bar->schedule([bar](float dt) {

			float p = bar->getPercent() + 5.f;

			OGLOG("tick %f", dt);
			if (p > 100.f) p = 100.f;
			bar->setPercent(p);
			OGLOG("percent = %f", bar->getPercent());   // ← 打出来看
		}, 1.0f, "loading");
	}


 	 
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