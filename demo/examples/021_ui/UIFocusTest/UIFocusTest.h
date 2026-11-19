

//
//  UIFocusTest.h
//  orange_tests
//
//  Created by guanghui on 5/4/14.
//
//

#ifndef __orange_tests__UIFocusTest__
#define __orange_tests__UIFocusTest__


#include "MyLayer.h"
#include "ui/UIWidget.h"
#include "2d/OGMenu.h"
#include "TestNavigation.h"

USING_OG;
using namespace orange::ui;



DEFINE_TEST_SUITE(UIFocusTests);

class UIFocusTestBase : public MyLayer
{
public:
    UIFocusTestBase();
    virtual ~UIFocusTestBase();

    virtual bool init() override;

    virtual void onLeftKeyPressed();
    virtual void onRightKeyPressed();
    virtual void onUpKeyPressed();
    virtual void onDownKeyPressed();
    virtual void onFocusChanged(orange::ui::Widget* widgetLostFocus, orange::ui::Widget* widgetGetFocus);

    void onImageViewClicked(orange::Ref* ref, orange::ui::Widget::TouchEventType touchType);

protected:
    orange::Menu* _dpadMenu;
    orange::ui::Widget* _firstFocusedWidget;
    orange::EventListenerFocus* _eventListener;
    orange::ui::Button* _toggleButton;
};

class UIFocusTestHorizontal : public UIFocusTestBase
{
public:
    CREATE_FUNC(UIFocusTestHorizontal); 
    virtual bool init() override;

    void toggleFocusLoop(orange::Ref*,orange::ui::Widget::TouchEventType);

protected:
    orange::ui::Layout* _horizontalLayout;
    orange::ui::Text* _loopText;
};

class UIFocusTestVertical : public UIFocusTestBase
{
public:
    CREATE_FUNC(UIFocusTestVertical); 

    virtual bool init() override;

    void toggleFocusLoop(orange::Ref*,orange::ui::Widget::TouchEventType);
protected:
    orange::ui::Layout* _verticalLayout;
    orange::ui::Text* _loopText;
};

class UIFocusTestNestedLayout1 : public UIFocusTestBase
{
public:
    CREATE_FUNC(UIFocusTestNestedLayout1); 
    virtual bool init() override;

    void toggleFocusLoop(orange::Ref*,orange::ui::Widget::TouchEventType);
protected:
    orange::ui::Layout* _verticalLayout;
    orange::ui::Text* _loopText;
};

class UIFocusTestNestedLayout2 : public UIFocusTestBase
{
public:
    CREATE_FUNC(UIFocusTestNestedLayout2); 

    virtual bool init() override;

    void toggleFocusLoop(orange::Ref*,orange::ui::Widget::TouchEventType);
protected:
    orange::ui::Layout* _horizontalLayout;
    orange::ui::Text* _loopText;
};

class UIFocusTestNestedLayout3 : public UIFocusTestBase
{
public:
    CREATE_FUNC(UIFocusTestNestedLayout3); 
    virtual bool init() override;

    void toggleFocusLoop(orange::Ref*,orange::ui::Widget::TouchEventType);
protected:
    orange::ui::Layout* _verticalLayout;
    orange::ui::Text* _loopText;
};



#endif /* defined(__orange_tests__UIFocusTest__) */
