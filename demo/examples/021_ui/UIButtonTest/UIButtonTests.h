 

#ifndef __TestCpp__UIButtonTest__
#define __TestCpp__UIButtonTest__

#include "orange.h"
#include "ui/UIWidget.h"

USING_OG;

class UIButtonTests : public Layer {  // ¡ï ¸Ä³É Layer
public: 
	UIButtonTests();

	void Tests();

	virtual void touchEvent(Ref* pSender, ui::Widget::TouchEventType type);
	virtual bool init();

	orange::Layer* _uiLayer;
	orange::ui::Text* _displayValueLabel;
	orange::ui::Button* _button;
};

class UIButtonTest_Scale9 : public UIButtonTests
{
public:
    CREATE_FUNC(UIButtonTest_Scale9); 

    virtual bool init() override;
    void touchEvent(Ref* sender, ui::Widget::TouchEventType type);

protected:
    ui::Text* _displayValueLabel;
};
// 
class UIButtonTest_Scale9_State_Change : public UIButtonTests
{
public:
	CREATE_FUNC(UIButtonTest_Scale9_State_Change); 

	virtual bool init() override;
	void touchEvent(Ref* sender, ui::Widget::TouchEventType type);

protected:
	ui::Text* _displayValueLabel;
};

class UIButtonTest_PressedAction : public UIButtonTests
{
public:
	CREATE_FUNC(UIButtonTest_PressedAction); 
	virtual bool init() override;
	void touchEvent(Ref* sender, ui::Widget::TouchEventType type); 
};
 
 class UIButtonTest_Title : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonTest_Title); 
     virtual bool init() override;
     void touchEvent(Ref* sender, ui::Widget::TouchEventType type); 
 };
 
 class UIButtonTestRemoveSelf : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonTestRemoveSelf); 
     virtual bool init() override;
     void touchEvent(Ref* sender, ui::Widget::TouchEventType type);
 
 protected: 
 };
 
 class UIButtonTestSwitchScale9 : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonTestSwitchScale9); 
     virtual bool init() override;
     void touchEvent(Ref* sender, ui::Widget::TouchEventType type);
 
 protected: 
 };
 
 class UIButtonTestZoomScale : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonTestZoomScale); 
     virtual bool init() override;
     void sliderEvent(Ref* sender, ui::Slider::EventType type);
 
 protected: 
 };
 
 class UIButtonTextOnly : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonTextOnly); 
     virtual bool init() override;
 
 protected: 
 };
 
 class UIButtonIgnoreContentSizeTest : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonIgnoreContentSizeTest); 
     virtual bool init() override;
 
 protected: 
 };
 
 class UIButtonTitleEffectTest : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonTitleEffectTest); 
     virtual bool init() override;
 
 protected: 
 };
 
 class UIButtonFlipTest : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonFlipTest); 
     virtual bool init() override;
 
 protected:
     ui::Text* _displayValueLabel;
 };
 
 class UIButtonNormalDefaultTest : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonNormalDefaultTest); 
     virtual bool init() override;
 
 protected:
     ui::Text* _displayValueLabel;
 };
 
 class UIButtonDisableDefaultTest : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonDisableDefaultTest); 
     virtual bool init() override;
 
 protected:
     ui::Text* _displayValueLabel;
 };
 
 class UIButtonCloneTest : public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonCloneTest); 
 
     virtual bool init() override;
 
 protected:
     ui::Text* _displayValueLabel;
 };
 
 class Issue12249 : public UIButtonTests
 {
 public:
     CREATE_FUNC(Issue12249);
     
 
     
     virtual bool init() override;
     
 protected:
     ui::Text* _displayValueLabel;
 };
 
 class Issue17116: public UIButtonTests
 {
 public:
     CREATE_FUNC(Issue17116);
 
 
 
     virtual bool init() override;
 };
 
 class UIButtonWithPolygonInfo: public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonWithPolygonInfo);
     
 
     
     virtual bool init() override;
 };
 
 class UIButtonScale9ChangeSpriteFrame: public UIButtonTests
 {
 public:
     CREATE_FUNC(UIButtonScale9ChangeSpriteFrame);
      
     
     virtual bool init() override;
 };

#endif /* defined(__TestCpp__UIButtonTest__) */
