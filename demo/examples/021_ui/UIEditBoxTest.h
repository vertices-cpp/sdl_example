 

#ifndef __orange_tests__UIEditBoxTest__
#define __orange_tests__UIEditBoxTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"

DEFINE_TEST_SUITE(UIEditBoxTests);

class UIEditBoxTest : public MyLayer, public orange::ui::EditBoxDelegate
{
public:
    CREATE_FUNC(UIEditBoxTest);

    virtual bool init() override;
    
    virtual void editBoxEditingDidBegin(orange::ui::EditBox* editBox)override;
    virtual void editBoxEditingDidEndWithAction(orange::ui::EditBox* editBox, orange::ui::EditBoxDelegate::EditBoxEndAction action)override;
    virtual void editBoxTextChanged(orange::ui::EditBox* editBox, const std::string& text)override;
    virtual void editBoxReturn(orange::ui::EditBox* editBox)override;

    
protected:
    orange::Label* _TTFShowEditReturn;
    orange::ui::EditBox* _editName;
    orange::ui::EditBox* _editPassword;
    orange::ui::EditBox* _editEmail;
    orange::Node* _editEmailParent;
};

class UIEditBoxTestToggleVisibility : public MyLayer, public orange::ui::EditBoxDelegate
{
public:
    CREATE_FUNC(UIEditBoxTestToggleVisibility);
    
    virtual bool init() override;
    
    virtual void editBoxEditingDidBegin(orange::ui::EditBox* editBox)override;
    virtual void editBoxTextChanged(orange::ui::EditBox* editBox, const std::string& text)override;
    virtual void editBoxReturn(orange::ui::EditBox* editBox)override;
    
    
protected:
    orange::Label* _TTFShowEditReturn;
    orange::ui::EditBox* _editName;
    orange::ui::EditBox* _editPassword;
    orange::ui::EditBox* _editEmail;
    orange::Node* _editEmailParent;
};

class UIEditBoxTestTextHorizontalAlignment : public MyLayer
{
public:
    CREATE_FUNC(UIEditBoxTestTextHorizontalAlignment);
    
    virtual bool init() override;
};

class UIEditBoxTestPressedAndDisabled : public MyLayer
{
public:
    CREATE_FUNC(UIEditBoxTestPressedAndDisabled);
    virtual bool init() override;
};

#endif /* defined(__orange_tests__UIEditBoxTest__) */
