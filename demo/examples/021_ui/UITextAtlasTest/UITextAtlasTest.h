



#ifndef __TestCpp__UITextAtlasTest__
#define __TestCpp__UITextAtlasTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"

USING_OG;
DEFINE_TEST_SUITE(UITextAtlasTests);

class UITextAtlasTest : public MyLayer
{
public:
    CREATE_FUNC(UITextAtlasTest);

    virtual bool init() override;
    void printWidgetResources(orange::Ref* sender);
protected:
    orange::ui::TextAtlas* _textAtlas;
};

class UITextAtlasETC1ShadowTest : public MyLayer
{
public:
    CREATE_FUNC(UITextAtlasETC1ShadowTest);
    
    virtual bool init() override;
    void printWidgetResources(orange::Ref* sender);
protected:
    orange::ui::TextAtlas* _textAtlas;
};

#endif /* defined(__TestCpp__UITextAtlasTest__) */
