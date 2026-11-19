

#ifndef __TestCpp__UISliderTest__
#define __TestCpp__UISliderTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"
USING_OG;

DEFINE_TEST_SUITE(UISliderTests);

class UISliderTest : public MyLayer
{
public:
    CREATE_FUNC(UISliderTest); 
    virtual bool init() override;
    void sliderEvent(orange::Ref* sender, orange::ui::Slider::EventType type);
    void printWidgetResources(orange::Ref* sender);
    
protected:
    orange::ui::TextBMFont* _displayValueLabel;
    orange::ui::Slider* _slider;
};

class UISliderTest_Scale9 : public MyLayer
{
public:
    CREATE_FUNC(UISliderTest_Scale9); 
    virtual bool init() override;
    void sliderEvent(orange::Ref* sender, orange::ui::Slider::EventType type);
    
protected:
    orange::ui::Text* _displayValueLabel;
};

class UISliderTest_Scale9_State_Change : public MyLayer
{
public:
    CREATE_FUNC(UISliderTest_Scale9_State_Change); 
    virtual bool init() override;
    void sliderEvent(orange::Ref* sender, orange::ui::Slider::EventType type);

protected:
    orange::ui::Text* _displayValueLabel;
};


class UISliderNormalDefaultTest : public MyLayer
{
public:
    CREATE_FUNC(UISliderNormalDefaultTest); 
    virtual bool init() override;
    
protected:
    orange::ui::Text* _displayValueLabel;
};

class UISliderDisabledDefaultTest : public MyLayer
{
public:
    CREATE_FUNC(UISliderDisabledDefaultTest); 
    virtual bool init() override;
    
protected:
    orange::ui::Text* _displayValueLabel;
};

class UISliderNewEventCallbackTest : public MyLayer
{
public:
    CREATE_FUNC(UISliderNewEventCallbackTest); 
    virtual bool init() override;

protected:
    orange::ui::Text* _displayValueLabel;
};

class UISliderIssue12249Test : public MyLayer
{
public:
    CREATE_FUNC(UISliderIssue12249Test); 
    virtual bool init() override;
    void sliderEvent(orange::Ref* sender, orange::ui::Slider::EventType type);
    
protected:
    orange::ui::TextBMFont* _displayValueLabel;
};
#endif /* defined(__TestCpp__UISliderTest__) */
