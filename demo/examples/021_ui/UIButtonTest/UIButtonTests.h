 

#ifndef __TestCpp__UIButtonTest__
#define __TestCpp__UIButtonTest__

#include "MyLayer.h"
#include "TestNavigation.h"
 
DEFINE_TEST_SUITE(UIButtonTests);

class UIButtonTest : public MyLayer {  // ★ 改成 MyLayer
public: 
	UIButtonTest(); 
	virtual void touchEvent(Ref* pSender, ui::Widget::TouchEventType type);
	virtual bool init(); 
};

class UIButtonTest_Scale9 : public  MyLayer
{
public:
    CREATE_FUNC(UIButtonTest_Scale9); 

    virtual bool init() override;
    void touchEvent(Ref* sender, ui::Widget::TouchEventType type);

protected: 
};
// 
class UIButtonTest_Scale9_State_Change : public  MyLayer
{
public:
	CREATE_FUNC(UIButtonTest_Scale9_State_Change); 

	virtual bool init() override;
	void touchEvent(Ref* sender, ui::Widget::TouchEventType type);

protected:
	
};

class UIButtonTest_PressedAction : public  MyLayer
{
public:
	CREATE_FUNC(UIButtonTest_PressedAction); 
	virtual bool init() override;
	void touchEvent(Ref* sender, ui::Widget::TouchEventType type); 
};
 
 class UIButtonTest_Title : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonTest_Title); 
     virtual bool init() override;
     void touchEvent(Ref* sender, ui::Widget::TouchEventType type); 
 };
 
 class UIButtonTestRemoveSelf : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonTestRemoveSelf); 
     virtual bool init() override;
     void touchEvent(Ref* sender, ui::Widget::TouchEventType type);
 
 protected: 
 };
 
 class UIButtonTestSwitchScale9 : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonTestSwitchScale9); 
     virtual bool init() override;
     void touchEvent(Ref* sender, ui::Widget::TouchEventType type);
 
 protected: 
 };
 
 class UIButtonTestZoomScale : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonTestZoomScale); 
     virtual bool init() override;
     void sliderEvent(Ref* sender, ui::Slider::EventType type);
 
 protected: 
 };
 
 class UIButtonTextOnly : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonTextOnly); 
     virtual bool init() override;
 
 protected: 
 };
 
 class UIButtonIgnoreContentSizeTest : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonIgnoreContentSizeTest); 
     virtual bool init() override;
 
 protected: 
 };
 
 class UIButtonTitleEffectTest : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonTitleEffectTest); 
     virtual bool init() override;
 
 protected: 
 };
 
 class UIButtonFlipTest : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonFlipTest); 
     virtual bool init() override;
 
 protected:
     
 };
 
 class UIButtonNormalDefaultTest : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonNormalDefaultTest); 
     virtual bool init() override;
 
 protected:
     
 };
 
 class UIButtonDisableDefaultTest : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonDisableDefaultTest); 
     virtual bool init() override;
 
 protected:
     
 };
 
 class UIButtonCloneTest : public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonCloneTest); 
 
     virtual bool init() override;
 
 protected:
     
 };
 
 class Issue12249 : public  MyLayer
 {
 public:
     CREATE_FUNC(Issue12249);
     
 
     
     virtual bool init() override;
     
 protected:
     
 };
 
 class Issue17116: public  MyLayer
 {
 public:
     CREATE_FUNC(Issue17116);
 
 
 
     virtual bool init() override;
 };
 
 class UIButtonWithPolygonInfo: public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonWithPolygonInfo);
     
 
     
     virtual bool init() override;
 };
 
 class UIButtonScale9ChangeSpriteFrame: public  MyLayer
 {
 public:
     CREATE_FUNC(UIButtonScale9ChangeSpriteFrame);
      
     
     virtual bool init() override;
 };

#endif /* defined(__TestCpp__UIButtonTest__) */
