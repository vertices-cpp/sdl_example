

#ifndef __TestCpp__UITextTest__
#define __TestCpp__UITextTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"

USING_OG;

DEFINE_TEST_SUITE(UITextTests);

class UITextTest : public MyLayer
{
public:
    CREATE_FUNC(UITextTest);

    virtual bool init() override;
};

class UITextTest_LineWrap : public MyLayer
{
public:
    CREATE_FUNC(UITextTest_LineWrap);

    virtual bool init() override;
};


class UILabelTest_Effect : public MyLayer
{
public:
    CREATE_FUNC(UILabelTest_Effect);

    virtual bool init() override;
};
 

class UITextTest_TTF : public MyLayer
{
public:
    CREATE_FUNC(UITextTest_TTF);

    virtual bool init() override;
};

class UITextTest_IgnoreContentSize : public MyLayer
{
public:
    CREATE_FUNC(UITextTest_IgnoreContentSize);

    virtual bool init() override;
};

class UITextTest_Clone : public MyLayer
{
public:
    CREATE_FUNC(UITextTest_Clone);

    virtual bool init() override;
};

class Issue16073Test : public MyLayer
{
public:
    CREATE_FUNC(Issue16073Test);

    virtual bool init() override;
};

#endif /* defined(__TestCpp__UITextTest__) */
