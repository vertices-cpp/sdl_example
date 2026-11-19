 

#ifndef __TestCpp__UICheckBoxTest__
#define __TestCpp__UICheckBoxTest__

#include "MyLayer.h"
#include "TestNavigation.h"

DEFINE_TEST_SUITE(UICheckBoxTests);

class UICheckBoxTest : public  MyLayer
{
	
 
public:
    CREATE_FUNC(UICheckBoxTest);

    UICheckBoxTest();
 
    ~UICheckBoxTest();

    virtual bool init() override;
    void selectedEvent(Ref* sender,ui::CheckBox::EventType type);
    
    void printWidgetResources(Ref* sender);
protected: 
    ui::CheckBox* _checkBox; 
};

class UICheckBoxDefaultBehaviorTest : public MyLayer
{
public:
    CREATE_FUNC(UICheckBoxDefaultBehaviorTest); 
    virtual bool init() override;
    
protected: 
};

class UICheckBoxCopyTest : public MyLayer
{
public:
    CREATE_FUNC(UICheckBoxCopyTest);  
    virtual bool init() override;

protected: 
};
#endif /* defined(__TestCpp__UICheckBoxTest__) */
