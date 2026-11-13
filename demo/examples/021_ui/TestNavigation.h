#ifndef __TEST_NAVIGATION_H__
#define __TEST_NAVIGATION_H__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "UITestMenu.h"
#include <vector>
#include <functional>

USING_OG;
using namespace ui;

#define ADD_TEST(__className__) \
    { #__className__, []() -> Layer* { \
        return __className__::create(); \
    }}

// ★ 魔法宏：把类名变成字符串，并自动生成工厂函数
#define ADD_TEST_CASE(__className__) \
    addTestCase( #__className__, []() -> Layer* { \
        auto node = new (std::nothrow) __className__(); \
        if (node && node->init()) { \
            node->autorelease(); \
            return (Layer*)node; \
        } \
        delete node; \
        return nullptr; \
    })

// ★ 继承套件的宏（如果你需要动态创建子菜单，可以用这个）
#define DEFINE_TEST_SUITE(__className__) \
class __className__ : public UITestMenu \
{ \
public: \
    CREATE_FUNC(__className__); \
    __className__(); \
    virtual bool init() override { \
        if (UITestMenu::init()) { \
            this->startFirstTest(); \
            return true; \
        } \
        return false; \
    } \
}

#endif