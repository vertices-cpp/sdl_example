

#ifndef __TestCpp__UIRadioButtonTest__
#define __TestCpp__UIRadioButtonTest__

#include "orange.h"
#include "ui/UIWidget.h"
#include "MyLayer.h"
#include "TestNavigation.h"

USING_OG;

DEFINE_TEST_SUITE(UIRadioButtonTests);

class UIRadioButtonTest : public MyLayer
{
public:
    CREATE_FUNC(UIRadioButtonTest);
    
    UIRadioButtonTest();
    ~UIRadioButtonTest();
    
    virtual bool init() override;
    
    void addRadioButton(Ref* sender);
    void deleteRadioButton(Ref* sender);

private:
    orange::ui::RadioButtonGroup* _radioButtonGroup;
    orange::ui::Text* _allowNoSelectionText;
    
};


class UIRadioButtonTwoGroupsTest : public MyLayer
{
public:
    CREATE_FUNC(UIRadioButtonTwoGroupsTest);

    UIRadioButtonTwoGroupsTest();
    ~UIRadioButtonTwoGroupsTest();

    virtual bool init() override;
    void onChangedRadioButtonGroup1(orange::ui::RadioButton* radioButton, int index, orange::ui::RadioButtonGroup::EventType type);
    void onChangedRadioButtonGroup2(orange::ui::RadioButton* radioButton, int index, orange::ui::RadioButtonGroup::EventType type);
    void onChangedRadioButtonSelect(orange::ui::RadioButton* radioButton,orange::ui::RadioButton::EventType type);
    void clearRadioButtonGroup(Ref* sender);

    
protected:
    void addLog(const std::string& log);
    
    orange::ui::RadioButtonGroup* _radioButtonGroups[2];
    orange::ui::Text* _groupEventLabel;
    orange::ui::Text* _buttonEventLabel;
    orange::ui::Text* _logConsole;
    int _numberOfLogLines;
};

class UIRadioButtonTabTest : public MyLayer
{
public:
    CREATE_FUNC(UIRadioButtonTabTest);
    virtual bool init() override;
};

#endif /* defined(__TestCpp__UIRadioButtonTest__) */
