 

#ifndef __TestCpp__UICheckBoxTest__
#define __TestCpp__UICheckBoxTest__

#include "orange.h"
#include "ui/UIWidget.h"

USING_OG;

class UICheckBoxTest : public Layer
{
	
 
public:
    CREATE_FUNC(UICheckBoxTest);

    UICheckBoxTest();
	void Tests();
    ~UICheckBoxTest();

    virtual bool init() override;
    void selectedEvent(Ref* sender,ui::CheckBox::EventType type);
    
    void printWidgetResources(Ref* sender);
protected:
    ui::Text* _displayValueLabel;
    ui::CheckBox* _checkBox;

	orange::Layer* _uiLayer;
};

class UICheckBoxDefaultBehaviorTest : public UICheckBoxTest
{
public:
    CREATE_FUNC(UICheckBoxDefaultBehaviorTest); 
    virtual bool init() override;
    
protected: 
};

class UICheckBoxCopyTest : public UICheckBoxTest
{
public:
    CREATE_FUNC(UICheckBoxCopyTest);  
    virtual bool init() override;

protected: 
};
#endif /* defined(__TestCpp__UICheckBoxTest__) */
