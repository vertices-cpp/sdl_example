#ifndef __TEST_NAVIGATION_H__
#define __TEST_NAVIGATION_H__

#include "orange.h"
#include "ui/UIWidget.h" 

USING_OG;
using namespace ui;

// 定义工厂类型
typedef std::function<Layer*()> LayerFactory;

// ==========================================================
// 1. 统一的返回逻辑（回主菜单）
// ==========================================================
static void bind_back(Layer* _this) {
	auto parent = _this->getParent(); // testContainer
	if (parent) {
		auto helloWorld = parent->getParent(); // HelloWorld
		if (helloWorld) {
			auto sv = (ui::ScrollView*)helloWorld->getChildByName("MyMenuList");
			if (sv) {
				sv->setVisible(true);
				sv->setTouchEnabled(true);
				auto inner = sv->getInnerContainer();
				if (inner) {
					for (auto child : inner->getChildren()) {
						auto menu = dynamic_cast<Menu*>(child);
						if (menu) {
							menu->setEnabled(true);
							menu->setVisible(true);
						}
					}
				}
			}
		}
	}
	_this->retain();
	_this->removeFromParent();
	_this->release();
}

// ==========================================================
// 2. 内部页面切换逻辑（Prev / Next）
// ==========================================================
static void switchTo(Layer* _this, LayerFactory targetFactory) {
	if (!targetFactory) return;
	auto parent = _this->getParent();
	if (!parent) return;

	// ★ 先 retain 保护自己
	_this->retain();
	_this->removeFromParent();

	auto newLayer = targetFactory();
	if (newLayer) {
		parent->addChild(newLayer);
		newLayer->release();
	}

	// ★ 最后释放
	_this->release();
}

// ==========================================================
// 3. 统一注入导航栏（外挂到当前 Layer）
// ==========================================================
static void setupNavigation(Layer* _this, LayerFactory prevFactory, LayerFactory nextFactory) {
	Size widgetSize = Director::getInstance()->getWinSize();

	// 1. 返回按钮
	auto backLabel = Label::createWithTTF("<- Back", "fonts/Marker Felt.ttf", 24);
	auto backItem = MenuItemLabel::create(backLabel, std::bind(bind_back, _this));
	backItem->setPosition(Vec2(widgetSize.width - 100, 40));
	auto backMenu = Menu::create(backItem, nullptr);
	backMenu->setPosition(Vec2::ZERO);
	_this->addChild(backMenu, 100);

	// 2. 上一个 (Prev)
	if (prevFactory) {
		auto prevLabel = Label::createWithTTF("< Prev", "fonts/Marker Felt.ttf", 24);
		auto prevItem = MenuItemLabel::create(prevLabel, [=](Ref*) {
			switchTo(_this, prevFactory);
		});
		OGLOG("Texture Count: %zu", Director::getInstance()->getTextureCache()->getTexturesCache().size());
		auto& frames = SpriteFrameCache::getInstance()->debugGetFrames();
		OGLOG("=== SpriteFrame size: %zu ===", frames.size());
		for (const auto& pair : frames) {
			OGLOG("Frame key: %s", pair.first.c_str());
		}

		prevItem->setPosition(Vec2(widgetSize.width - 300, 40));
		auto prevMenu = Menu::create(prevItem, nullptr);
		prevMenu->setPosition(Vec2::ZERO);
		_this->addChild(prevMenu, 100);
	}

	// 3. 下一个 (Next)
	if (nextFactory) {
		auto nextLabel = Label::createWithTTF("Next >", "fonts/Marker Felt.ttf", 24);
		auto nextItem = MenuItemLabel::create(nextLabel, [=](Ref*) {
			switchTo(_this, nextFactory);
		});
		nextItem->setPosition(Vec2(widgetSize.width - 200, 40));
		auto nextMenu = Menu::create(nextItem, nullptr);
		nextMenu->setPosition(Vec2::ZERO);
		_this->addChild(nextMenu, 100);
	}
}

// 前置的工厂宏（之前已经有了）
#define TEST_LAYER(__TYPE__) \
    []() -> Layer* { \
        auto node = new (std::nothrow) __TYPE__(); \
        if (node && node->init()) return static_cast<Layer*>(node); \
        delete node; \
        return nullptr; \
    }

// ★ 1. 只有 Next，没有 Prev (比如第一个测试用例)
#define NAV_NEXT_ONLY(NEXT_TYPE) \
    setupNavigation(this, nullptr, TEST_LAYER(NEXT_TYPE))

// ★ 2. 只有 Prev，没有 Next (比如最后一个测试用例)
#define NAV_PREV_ONLY(PREV_TYPE) \
    setupNavigation(this, TEST_LAYER(PREV_TYPE), nullptr)

// ★ 3. 既有 Prev 又有 Next (中间的测试用例)
#define NAV_BOTH(PREV_TYPE, NEXT_TYPE) \
    setupNavigation(this, TEST_LAYER(PREV_TYPE), TEST_LAYER(NEXT_TYPE))

#define ADD_CLASS(_CLASS_NAME_)\
{#_CLASS_NAME_, []() {\
auto node = new (std::nothrow) _CLASS_NAME_;\
if (node && node->init())\
return (Layer*)node;\
return (Layer*)nullptr;\
}\
}

#endif // __TEST_NAVIGATION_H__