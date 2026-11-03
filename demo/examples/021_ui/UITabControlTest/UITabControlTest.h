

#ifndef __TestCpp__UITabControlTest__
#define __TestCpp__UITabControlTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"
USING_OG;

DEFINE_TEST_SUITE(UITabControlTests);

class UITabControlTest : public MyLayer
{
public:
    CREATE_FUNC(UITabControlTest);


    UITabControlTest();
    ~UITabControlTest();

    bool init() override;

};


#endif //__TestCpp__UITabControlTest__