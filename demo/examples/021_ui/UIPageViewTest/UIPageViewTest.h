

#ifndef __TestCpp__UIPageViewTest__
#define __TestCpp__UIPageViewTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"
USING_OG;

DEFINE_TEST_SUITE(UIPageViewTests);

class UIPageViewTest : public MyLayer
{
public:
    CREATE_FUNC(UIPageViewTest);

    UIPageViewTest();
    ~UIPageViewTest();
    virtual bool init() override;

    void pageViewEvent(orange::Ref* sender, orange::ui::PageView::EventType type);

protected:

    orange::ui::Text* _displayValueLabel;
};

class UIPageViewButtonTest : public MyLayer
{
public:
    CREATE_FUNC(UIPageViewButtonTest);

    UIPageViewButtonTest();
    ~UIPageViewButtonTest();
    virtual bool init() override;
    
    void pageViewEvent(orange::Ref* sender, orange::ui::PageView::EventType type);
    void onButtonClicked(orange::Ref* sender, orange::ui::Widget::TouchEventType type);
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIPageViewTouchPropagationTest : public MyLayer
{
public:
    CREATE_FUNC(UIPageViewTouchPropagationTest);

    UIPageViewTouchPropagationTest();
    ~UIPageViewTouchPropagationTest();
    virtual bool init() override;
    
    void pageViewEvent(orange::Ref* sender, orange::ui::PageView::EventType type);
    void onButtonClicked(orange::Ref* sender, orange::ui::Widget::TouchEventType type);

protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIPageViewDynamicAddAndRemoveTest : public MyLayer
{
public:
    CREATE_FUNC(UIPageViewDynamicAddAndRemoveTest);

    UIPageViewDynamicAddAndRemoveTest();
    ~UIPageViewDynamicAddAndRemoveTest();
    virtual bool init() override;
    
    void pageViewEvent(orange::Ref* sender, orange::ui::PageView::EventType type);
    void onButtonClicked(orange::Ref* sender, orange::ui::Widget::TouchEventType type);
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIPageViewJumpToPageTest : public MyLayer
{
public:
    CREATE_FUNC(UIPageViewJumpToPageTest);

    UIPageViewJumpToPageTest();
    ~UIPageViewJumpToPageTest();
    virtual bool init() override;

protected:

    orange::ui::Text* _displayValueLabel;
};

class UIPageViewVerticalTest : public MyLayer
{
public:
    CREATE_FUNC(UIPageViewVerticalTest);
    
    UIPageViewVerticalTest();
    ~UIPageViewVerticalTest();
    virtual bool init() override;
    
    void pageViewEvent(orange::Ref* sender, orange::ui::PageView::EventType type);
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIPageViewDisableTouchTest : public MyLayer
{
public:
    CREATE_FUNC(UIPageViewDisableTouchTest);
    
    UIPageViewDisableTouchTest();
    ~UIPageViewDisableTouchTest();
    virtual bool init() override;
        
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIPageViewChildSizeTest : public MyLayer
{
public:
    CREATE_FUNC(UIPageViewChildSizeTest);

    UIPageViewChildSizeTest();
    ~UIPageViewChildSizeTest();
    virtual bool init() override;

    void pageViewEvent(orange::Ref* sender, orange::ui::PageView::EventType type);

protected:

    orange::ui::Text* _displayValueLabel;
};

class UIPageViewIndicatorTest : public MyLayer
{
public:
    CREATE_FUNC(UIPageViewIndicatorTest);
    
    UIPageViewIndicatorTest();
    ~UIPageViewIndicatorTest();
    virtual bool init() override;
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

#endif /* defined(__TestCpp__UIPageViewTest__) */
