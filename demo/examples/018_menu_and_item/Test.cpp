// Test.cpp
#include "Test.h"
#include "OGMenu.h"
#include "OGMenuItem.h"
#include "OGLabel.h"
#include "OGSprite.h"
#include "OGDirector.h"
//#include "OGScene.h"
#include "OGLayer.h"
#include "OGCamera.h"
#include "OGEventDispatcher.h"
#include "OGEventListenerTouch.h"
#include "OGFileUtils.h"

#define PATH_RES
#include "path_head.h"

USING_OG;

bool Test::init()
{
	if (!Node::init()) return false;

	// ===== 1. 相机 =====
	auto camera = Camera::getInstance();
	camera->setZoom(0.5f);
	camera->setViewportSize(Size(200, 200));
	camera->setViewportPos(Vec2(130, 130));
	camera->setCenter(Vec2(200, 200));

 	auto l = Label::createWithTTF("OFF", "fonts/arial.ttf", 32);
 	addChild(l);
 	l->setPosition(Vec2::ZERO);
// 
 	// 拖拽相机
 	auto listener = EventListenerTouchAllAtOnce::create();
 	listener->onTouchesMoved = [](const std::vector<Touch*>& touches, Event* event) {
 		auto touch = touches[0];
 		auto camera = Camera::getInstance();
 		Vec2 diff = touch->getDelta();
 		camera->setCenter(camera->getCenter() + diff);
 	};
 	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
 
 
 	// ===== 2. MenuItemFont（点击回调） =====
 	MenuItemFont::setFontName("fonts/arial.ttf");
  	MenuItemFont::setFontSize(32);
  
  	auto itemFont1 = MenuItemFont::create("Font1", [](Ref* sender) {
  		OGLOG("clicked: Font1");
  	});
  
  	auto itemFont2 = MenuItemFont::create("Font2", [](Ref* sender) {
  		OGLOG("clicked: Font2");
  	});
  
  	// ===== 3. MenuItemImage（normal / selected / disabled） =====
  	auto itemImage = MenuItemImage::create(
  		og::checkPath("018_menu_and_item/CloseNormal.png"),
  		og::checkPath("018_menu_and_item/CloseSelected.png"),
  		[](Ref* sender) {
  		OGLOG("clicked: Image");
  	});
 
 	if (itemImage)
 	{
 		OGLOG("itemImage normal size=(%f,%f)",
 			itemImage->getNormalImage()->getContentSize().width,
 			itemImage->getNormalImage()->getContentSize().height);
 
 		if (itemImage->getSelectedImage())
 			OGLOG("itemImage selected ok");
 		else
 			OGLOGERROR("itemImage selected = nullptr! CloseSelected.png load failed");
 	}
 	else
 	{
 		OGLOGERROR("itemImage create failed");
 	}
 
 	// ===== 4. MenuItemSprite（用 Sprite 包图） =====
 	auto normalSpr = Sprite::create("018_menu_and_item/CloseNormal.png");
 	auto selectedSpr = Sprite::create("018_menu_and_item/CloseSelected.png");
 
 	auto itemSprite = MenuItemSprite::create(
 		normalSpr, selectedSpr,
 		[](Ref* sender) {
 		OGLOG("clicked: Sprite");
 	});

	// ===== 5. MenuItemToggle（OFF / ON） =====
	auto toggleOff = MenuItemFont::create("OFF");
	auto toggleOn = MenuItemFont::create("ON");

	auto itemToggle = MenuItemToggle::createWithCallback(
		[](Ref* sender) {
		auto* toggle = dynamic_cast<MenuItemToggle*>(sender);
		if (toggle)
			OGLOG("toggle -> index=%u", toggle->getSelectedIndex());
	},
		toggleOff, toggleOn, nullptr);
	//itemToggle->setPosition(Vec2::ZERO);
	// ===== 6. 组装 Menu，垂直排列 =====
	auto menu = Menu::create(itemFont1, itemFont2, itemImage, itemSprite, itemToggle, nullptr);
	if (!menu)
	{
		OGLOGERROR("Menu::create failed");
		return false;
	}

	menu->setPosition(Vec2(220, 220));   // 窗口中心
 	menu->alignItemsVerticallyWithPadding(20);
	addChild(menu);

	// ===== 7. 1 秒后禁用 itemFont2 =====
// 	scheduleOnce([itemFont2](float) {
// 		if (itemFont2)
// 		{
// 			itemFont2->setEnabled(false);
// 			OGLOG("itemFont2 disabled");
// 		}
// 	}, 1.0f, "disable_item2");
// 
// 	// ===== 8. 2 秒后重新启用 =====
// 	scheduleOnce([itemFont2](float) {
// 		if (itemFont2)
// 		{
// 			itemFont2->setEnabled(true);
// 			OGLOG("itemFont2 enabled");
// 		}
// 	}, 2.0f, "enable_item2");
// 
// 	// ===== 9. 3 秒后切换 toggle =====
// 	scheduleOnce([itemToggle](float) {
// 		if (itemToggle)
// 		{
// 			itemToggle->setSelectedIndex(1);
// 			OGLOG("toggle -> ON");
// 		}
// 	}, 3.0f, "toggle_on");
// 
// 	// ===== 10. 4 秒后把相机往右上拖，看选中图位置 =====
// 	scheduleOnce([](float) {
// 		Camera::getInstance()->setCenter(Vec2(420, 340));
// 		OGLOG("camera moved");
// 	}, 4.0f, "move_camera");

	return true;
}
 void Test::menuCloseCallback(Ref* pSender)
 { 
	 //Director::getInstance()->end(); 
	 cout << "111" << endl;

 }
void Test::update(float dt)
{
// 	label->enableOutline(r[i], 2);
// 	i++;
// 	if (i>3)
// 	{
// 		i = 0;
// 	}
}

void Test::draw(Renderer* renderer, const Mat3& transform, uint32_t flags)
{
	Node::draw(renderer, transform, flags);

	auto camera = Camera::getVisitingCamera();
	if (!camera) return;

	// 屏幕矩形直接画（因为 draw 的 transform 是"局部 -> 屏幕"，Screen 坐标就是屏幕）
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