

#ifndef __TestCpp__UILayoutTest__
#define __TestCpp__UILayoutTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"
USING_OG;

DEFINE_TEST_SUITE(UILayoutTests);

class UILayoutTest : public MyLayer
{
public:
    UILayoutTest();
    ~UILayoutTest();
    virtual bool init() override;
    
    CREATE_FUNC(UILayoutTest);
};

class UILayoutTest_Color : public MyLayer
{
public: 
    virtual bool init() override;
    
    CREATE_FUNC(UILayoutTest_Color);
};

class UILayoutTest_Gradient : public MyLayer
{
public: 
    virtual bool init() override;
    
    CREATE_FUNC(UILayoutTest_Gradient);
};

class UILayoutTest_BackGroundImage : public MyLayer
{
public: 
    virtual bool init() override;
    
    CREATE_FUNC(UILayoutTest_BackGroundImage);

    void printWidgetResources(orange::Ref* sender);
protected:
    orange::ui::Layout* _layout;
};

class UILayoutTest_BackGroundImage_Scale9 : public MyLayer
{
public: 
    virtual bool init() override;
    
    CREATE_FUNC(UILayoutTest_BackGroundImage_Scale9);
};

class UILayoutTest_Layout_Linear_Vertical : public MyLayer
{
public: 
    virtual bool init() override;
    
    CREATE_FUNC(UILayoutTest_Layout_Linear_Vertical);
};

class UILayoutTest_Layout_Linear_Horizontal : public MyLayer
{
public: 
    virtual bool init() override;
    
    CREATE_FUNC(UILayoutTest_Layout_Linear_Horizontal);
};

class UILayoutTest_Layout_Relative_Align_Parent : public MyLayer
{
public: 
    virtual bool init() override;
    
    CREATE_FUNC(UILayoutTest_Layout_Relative_Align_Parent);
};

class UILayoutTest_Layout_Relative_Location : public MyLayer
{
public: 
    virtual bool init() override;
    
    CREATE_FUNC(UILayoutTest_Layout_Relative_Location);
};

class UILayoutTest_Layout_Scaled_Widget : public MyLayer
{
public: 
    virtual bool init() override;
    
    CREATE_FUNC(UILayoutTest_Layout_Scaled_Widget);
};

class UILayoutComponentTest : public MyLayer
{
public:
    CREATE_FUNC(UILayoutComponentTest);

    virtual bool init() override;
    void touchEvent(orange::Ref* sender, orange::ui::Widget::TouchEventType type);
protected:
    orange::LayerColor* _baseLayer;
    
};

class UILayoutComponent_Berth_Test : public UILayoutComponentTest
{
public:
    virtual bool init() override;

    CREATE_FUNC(UILayoutComponent_Berth_Test);
};

class UILayoutComponent_Berth_Stretch_Test : public UILayoutComponentTest
{
public:
    virtual bool init() override;

    CREATE_FUNC(UILayoutComponent_Berth_Stretch_Test);
};

#endif /* defined(__TestCpp__UILayoutTest__) */
