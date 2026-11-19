

#ifndef __TestCpp__UINodeContainerTest__
#define __TestCpp__UINodeContainerTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"
USING_OG;

DEFINE_TEST_SUITE(UIWidgetAddNodeTests);

class UIWidgetAddNodeTest : public MyLayer
{
public:
    CREATE_FUNC(UIWidgetAddNodeTest);

    UIWidgetAddNodeTest();
    ~UIWidgetAddNodeTest();

    virtual bool init() override;
};

#endif /* defined(__TestCpp__UINodeContainerTest__) */
