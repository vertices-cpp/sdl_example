

#ifndef __TestCpp__UIImageViewTest__
#define __TestCpp__UIImageViewTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"

USING_OG;

DEFINE_TEST_SUITE(UIImageViewTests);

class UIImageViewTest : public MyLayer
{
public:
    CREATE_FUNC(UIImageViewTest);

    virtual bool init() override;

    void printWidgetResources(orange::Ref* sender);
protected:
    orange::ui::ImageView* _image;
};

class UIImageViewTest_Scale9 : public MyLayer
{
public:
    CREATE_FUNC(UIImageViewTest_Scale9);

    virtual bool init() override;
};

class UIImageViewTest_Scale9_State_Change : public MyLayer
{
public:
    CREATE_FUNC(UIImageViewTest_Scale9_State_Change);

    virtual bool init() override;
};

class UIImageViewTest_ContentSize : public MyLayer
{
public:
    CREATE_FUNC(UIImageViewTest_ContentSize);

    virtual bool init() override;
};

class UIImageViewFlipTest : public MyLayer
{
public:
    CREATE_FUNC(UIImageViewFlipTest);

    virtual bool init() override;
};

class UIImageViewIssue12249Test : public MyLayer
{
public:
    CREATE_FUNC(UIImageViewIssue12249Test);
    
    virtual bool init() override;
};

#endif /* defined(__TestCpp__UIImageViewTest__) */
