

#ifndef __TestCpp__UITextBMFontTest__
#define __TestCpp__UITextBMFontTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"

USING_OG;
DEFINE_TEST_SUITE(UITextBMFontTests);


class UITextBMFontTest : public MyLayer
{
public:
    CREATE_FUNC(UITextBMFontTest)

    virtual bool init() override;
    void printWidgetResources(orange::Ref* sender);
protected:
    orange::ui::TextBMFont* _textBMFont;
};

#endif /* defined(__TestCpp__UITextBMFontTest__) */
