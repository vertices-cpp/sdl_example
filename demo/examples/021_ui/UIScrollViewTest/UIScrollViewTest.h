

#ifndef __TestCpp__UIScrollViewTest__
#define __TestCpp__UIScrollViewTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"
USING_OG;

DEFINE_TEST_SUITE(UIScrollViewTests);

class UIScrollViewTest_Vertical : public MyLayer
{
public:
    CREATE_FUNC(UIScrollViewTest_Vertical);

    UIScrollViewTest_Vertical();
    ~UIScrollViewTest_Vertical();
    virtual bool init() override;
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIScrollViewTest_Horizontal : public MyLayer
{
public:
    CREATE_FUNC(UIScrollViewTest_Horizontal);

    UIScrollViewTest_Horizontal();
    ~UIScrollViewTest_Horizontal();
    virtual bool init() override;
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIScrollViewTest_Both : public MyLayer
{
public:
    CREATE_FUNC(UIScrollViewTest_Both);

    UIScrollViewTest_Both();
    ~UIScrollViewTest_Both();
    virtual bool init() override;
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIScrollViewTest_ScrollToPercentBothDirection : public MyLayer
{
public:
    CREATE_FUNC(UIScrollViewTest_ScrollToPercentBothDirection);

    UIScrollViewTest_ScrollToPercentBothDirection();
    ~UIScrollViewTest_ScrollToPercentBothDirection();
    virtual bool init() override;
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIScrollViewTest_ScrollToPercentBothDirection_Bounce : public MyLayer
{
public:
    CREATE_FUNC(UIScrollViewTest_ScrollToPercentBothDirection_Bounce);

    UIScrollViewTest_ScrollToPercentBothDirection_Bounce();
    ~UIScrollViewTest_ScrollToPercentBothDirection_Bounce();
    virtual bool init() override;
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIScrollViewNestTest : public MyLayer
{
public:
    CREATE_FUNC(UIScrollViewNestTest);

    UIScrollViewNestTest();
    ~UIScrollViewNestTest();
    virtual bool init() override;
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIScrollViewRotated : public MyLayer
{
public:
    CREATE_FUNC(UIScrollViewRotated);

    UIScrollViewRotated();
    ~UIScrollViewRotated();
    virtual bool init() override;
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UIScrollViewDisableTest : public MyLayer
{
public:
    CREATE_FUNC(UIScrollViewDisableTest);

    UIScrollViewDisableTest();
    ~UIScrollViewDisableTest();
    virtual bool init() override;

protected:

    orange::ui::Text* _displayValueLabel;
};

class UIScrollViewInnerSize : public MyLayer
{
public:
    CREATE_FUNC(UIScrollViewInnerSize);

    UIScrollViewInnerSize();
    ~UIScrollViewInnerSize();
    virtual bool init() override;

protected:

    orange::ui::Text* _displayValueLabel;
};

class UIScrollViewTestEvents : public MyLayer
{
public:
	CREATE_FUNC(UIScrollViewTestEvents);
	
	UIScrollViewTestEvents();
	virtual bool init() override;
	
protected:
	
	orange::ui::Text* _displayValueLabel;
};

class UIScrollViewStopScrollingTest : public MyLayer
{
public:
    CREATE_FUNC(UIScrollViewStopScrollingTest);

    UIScrollViewStopScrollingTest();
    virtual bool init() override;

    virtual void update(float dt) override;

protected:

    orange::ui::Text* _displayValueLabel;
    orange::ui::ScrollView* _scrollView;
    float _remainingTime;
};
#endif /* defined(__TestCpp__UIScrollViewTest__) */
