

#ifndef __TestCpp__UITextFieldTest__
#define __TestCpp__UITextFieldTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"
USING_OG;

DEFINE_TEST_SUITE(UITextFieldTests);

class UITextFieldTest : public MyLayer
{
public:
    CREATE_FUNC(UITextFieldTest);

    UITextFieldTest();
    ~UITextFieldTest();
    virtual bool init() override;
    void textFieldEvent(orange::Ref* sender, orange::ui::TextField::EventType type);
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UITextFieldTest_MaxLength : public MyLayer
{
public:
    CREATE_FUNC(UITextFieldTest_MaxLength);

    UITextFieldTest_MaxLength();
    ~UITextFieldTest_MaxLength();
    virtual bool init() override;
    void textFieldEvent(orange::Ref* sender, orange::ui::TextField::EventType type);
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UITextFieldTest_Password : public MyLayer
{
public:
    CREATE_FUNC(UITextFieldTest_Password);

    UITextFieldTest_Password();
    ~UITextFieldTest_Password();
    virtual bool init() override;
    void textFieldEvent(orange::Ref* sender, orange::ui::TextField::EventType type);
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UITextFieldTest_LineWrap : public MyLayer
{
public:
    CREATE_FUNC(UITextFieldTest_LineWrap);

    UITextFieldTest_LineWrap();
    ~UITextFieldTest_LineWrap();
    virtual bool init() override;
    void textFieldEvent(orange::Ref* sender, orange::ui::TextField::EventType type);
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UITextFieldTest_TrueTypeFont : public MyLayer
{
public:
    CREATE_FUNC(UITextFieldTest_TrueTypeFont);

    UITextFieldTest_TrueTypeFont();
    ~UITextFieldTest_TrueTypeFont();
    virtual bool init() override;
    void textFieldEvent(orange::Ref* sender, orange::ui::TextField::EventType type);
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UITextFieldTest_BMFont : public MyLayer
{
public:
    CREATE_FUNC(UITextFieldTest_BMFont);
    
    UITextFieldTest_BMFont();
    ~UITextFieldTest_BMFont();
    virtual bool init() override;
    void textFieldEvent(orange::Ref* sender, orange::ui::TextField::EventType type);
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};

class UITextFieldTest_PlaceHolderColor : public MyLayer
{
public:
    CREATE_FUNC(UITextFieldTest_PlaceHolderColor);

    UITextFieldTest_PlaceHolderColor();
    ~UITextFieldTest_PlaceHolderColor();
    virtual bool init() override;
    void textFieldEvent(orange::Ref* sender, orange::ui::TextField::EventType type);
    
protected:
    
    orange::ui::Text* _displayValueLabel;
};
#endif /* defined(__TestCpp__UITextFieldTest__) */
