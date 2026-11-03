

#ifndef __TestCpp__UILoadingBarTest__
#define __TestCpp__UILoadingBarTest__
#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"

USING_OG;

DEFINE_TEST_SUITE(UILoadingBarTests);

class UILoadingBarTest_Left : public MyLayer
{
public:
    CREATE_FUNC(UILoadingBarTest_Left);

    UILoadingBarTest_Left();
    ~UILoadingBarTest_Left();
    virtual bool init() override;
    void update(float delta)override;
    void printWidgetResources(orange::Ref* sender);
    
protected:
    int _count;
    orange::ui::LoadingBar* _loadingBar;
};

class UILoadingBarTest_Right : public MyLayer
{
public:
    CREATE_FUNC(UILoadingBarTest_Right);

    UILoadingBarTest_Right();
    ~UILoadingBarTest_Right();
    virtual bool init() override;
    void update(float delta)override;
    
protected:
    int _count;
};

class UILoadingBarTest_Left_Scale9 : public MyLayer
{
public:
    CREATE_FUNC(UILoadingBarTest_Left_Scale9);

    UILoadingBarTest_Left_Scale9();
    ~UILoadingBarTest_Left_Scale9();
    virtual bool init() override;
    void update(float delta)override;
    
protected:
    int _count;
};

class UILoadingBarTest_Right_Scale9 : public MyLayer
{
public:
    CREATE_FUNC(UILoadingBarTest_Right_Scale9);

    UILoadingBarTest_Right_Scale9();
    ~UILoadingBarTest_Right_Scale9();
    virtual bool init() override;
    void update(float delta)override;
    
protected:
    int _count;
};

class UILoadingBarTest_Scale9_State_Change : public MyLayer
{
public:
    CREATE_FUNC(UILoadingBarTest_Scale9_State_Change);

    UILoadingBarTest_Scale9_State_Change();
    ~UILoadingBarTest_Scale9_State_Change();
    virtual bool init() override;

protected:
    int _count;
};

class UILoadingBarReloadTexture : public MyLayer
{
public:
    CREATE_FUNC(UILoadingBarReloadTexture);

    UILoadingBarReloadTexture();
    ~UILoadingBarReloadTexture();
    void update(float dt) override;
    virtual bool init() override;
    
protected:
    int _count;
    
};

class UILoadingBarIssue12249 : public MyLayer
{
public:
    CREATE_FUNC(UILoadingBarIssue12249);
    
    UILoadingBarIssue12249();
    ~UILoadingBarIssue12249();
    virtual bool init() override;
    void update(float delta)override;
    
protected:
    int _count;
};

class UILoadingBarTest_Direction : public MyLayer
{
public:
    CREATE_FUNC(UILoadingBarTest_Direction);

    UILoadingBarTest_Direction();
    ~UILoadingBarTest_Direction();
    virtual bool init() override;
protected:
    int _count;
    orange::ui::LoadingBar* _loadingBar;
};

#endif /* defined(__TestCpp__UILoadingBarTest__) */
