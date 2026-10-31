

#include "UIButtonTests.h"
#include "TestNavigation.h"

USING_OG;
using namespace ui;

static int offsetX = 300, offsetY = 100;
  
//----------------UIButtonTests-------------------

UIButtonTests::UIButtonTests()
{ 
}
void UIButtonTests::Tests()
{
	Size widgetSize = Director::getInstance()->getWinSize();
	_uiLayer = Layer::create();
	_uiLayer->setContentSize(widgetSize);
	addChild(_uiLayer);
}

bool UIButtonTests::init()
{
	if (Layer::init()) // ★ 既然不用场景，这里必须是 Layer::init()
	{
		Tests();NAV_BOTH(UIButtonScale9ChangeSpriteFrame, UIButtonTest_Scale9);

		Size widgetSize = Director::getInstance()->getWinSize();
		this->setContentSize(widgetSize);
		 
		 

		// ★ 2. 添加状态文字
		_displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f - 120, widgetSize.height / 2.0f + 60));
		//	OGLOG("_displayValueLabel %f,%f\n", _displayValueLabel->getPosition().x, _displayValueLabel->getPosition().y);
		_uiLayer->addChild(_displayValueLabel);
		// ★ 3. 把 _uiLayer 尺寸也撑满
		_uiLayer->setContentSize(widgetSize);

		// ★ 3. 创建测试按钮（修正了图片路径）
		// 因为 searchPaths 设为了 021_ui，直接用文件名即可
		Button* button = Button::create("animationbuttonnormal.png",
			"animationbuttonpressed.png");

		if (button) {
			//	OGLOG("Button size: %f, %f", button->getContentSize().width, button->getContentSize().height);
			button->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
			//	OGLOG("button %f,%f\n", button->getPosition().x, button->getPosition().y);
			button->addTouchEventListener(OG_CALLBACK_2(UIButtonTests::touchEvent, this));
			button->setZoomScale(0.4f);
			button->setPressedActionEnabled(true);
			_uiLayer->addChild(button);
			button->setOpacity(100); // 测试透明度
		}
		else {
			OGLOG("Button Create Failed! Path error.");
		}

		// ★ 4. 创建 ImageView（用于点击后的动画测试）
		ImageView* imageView = ImageView::create();
		if (imageView) {
			imageView->setPosition(Vec2(widgetSize.width / 2.0f + 220, widgetSize.height / 2.0f));
			imageView->setTag(12);
			_uiLayer->addChild(imageView);
		}

		return true;
	}
	return false;
}



void UIButtonTests::touchEvent(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case Widget::TouchEventType::BEGAN:
		_displayValueLabel->setString(StringUtils::format("Touch Down"));
		break;

	case Widget::TouchEventType::MOVED:
		_displayValueLabel->setString(StringUtils::format("Touch Move"));
		break;

	case Widget::TouchEventType::ENDED:
	{
		OGLOG("START-------------------\n");
		_displayValueLabel->setString(StringUtils::format("Touch Up"));
		ImageView* imageView = (ImageView*)_uiLayer->getChildByTag(12);
		imageView->setVisible(false);
		imageView->loadTexture("ccicon.png");
		imageView->setOpacity(0);
		imageView->setVisible(true);
		imageView->runAction(Sequence::create(FadeIn::create(0.5), DelayTime::create(1.0), FadeOut::create(0.5), nullptr));
		Button *btn = (Button*)pSender;
		btn->loadTextureNormal("animationbuttonnormal.png");
	}
	break;

	case Widget::TouchEventType::CANCELED:
		_displayValueLabel->setString(StringUtils::format("Touch Cancelled"));
		break;

	default:
		break;
	}
}
//----------------UIButtonTest_Scale9-------------------
bool UIButtonTest_Scale9::init()
{
	if (Layer::init())
	{
		Size widgetSize = Director::getInstance()->getWinSize();
 

		// ★ Scale9 的前一个是 UIButtonTests，后一个是 UIButtonTextOnly
	 
		Tests();NAV_BOTH(UIButtonTests, UIButtonTest_Scale9_State_Change);

		// ... 下面写 Scale9 测试逻辑 ...

        // Add a label in which the button events will be displayed
        _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
        _uiLayer->addChild(_displayValueLabel);

        // Add the alert
        Text* alert = Text::create("Button scale9 render", "fonts/Marker Felt.ttf",30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                 widgetSize.height / 2.0f - alert->getContentSize().height * 1.75f));
        _uiLayer->addChild(alert);

        // Create the button
        Button* button = Button::create("button.png", "buttonHighlighted.png");
        // open scale9 render
        button->setScale9Enabled(true);
        button->setContentSize(Size(150.0f, 70.0f));
        button->setPosition(Vec2(-button->getContentSize().width - 10, widgetSize.height / 2.0f));
        auto moveBy = MoveBy::create(1.0f, Vec2(widgetSize.width/2, 0.0f));
        auto moveByReverse = moveBy->reverse()->clone();
        button->runAction(RepeatForever::create(
                                Sequence::create(moveBy,moveByReverse, NULL)));
        button->setPressedActionEnabled(true);
        button->addTouchEventListener(OG_CALLBACK_2(UIButtonTest_Scale9::touchEvent, this));
        _uiLayer->addChild(button);

        // Create the imageview
        Button* button2 = Button::create();
        button2->setPosition(Vec2(widgetSize.width / 2.0f + button->getContentSize().width + 20, widgetSize.height / 2.0f));
        button2->setName("normal");
        _uiLayer->addChild(button2);

        Sprite *sprite = Sprite::create("animationbuttonnormal.png");
        button2->addChild(sprite);

        return true;
    }
    return false;
}

void UIButtonTest_Scale9::touchEvent(Ref *pSender, Widget::TouchEventType type)
{
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            _displayValueLabel->setString(StringUtils::format("Touch Down"));
            break;

        case Widget::TouchEventType::MOVED:
            _displayValueLabel->setString(StringUtils::format("Touch Move"));
            break;

        case Widget::TouchEventType::ENDED:
        {
            _displayValueLabel->setString(StringUtils::format("Touch Up"));
            Button *btn = (Button*)_uiLayer->getChildByName("normal");
            btn->loadTextureNormal("animationbuttonnormal.png");
            btn->loadTexturePressed("animationbuttonpressed.png");
            btn->runAction(Sequence::create(FadeIn::create(0.5),DelayTime::create(1.0),FadeOut::create(0.5), nullptr));
        }
            break;

        case Widget::TouchEventType::CANCELED:
            _displayValueLabel->setString(StringUtils::format("Touch Cancelled"));
            break;

        default:
            break;
    }
} 
//----------------UIButtonTest_Scale9_State_Change-------------------
 bool UIButtonTest_Scale9_State_Change::init()
 {
     if (Layer::init())
     {
		 Size widgetSize = Director::getInstance()->getWinSize();

		 Tests();NAV_BOTH(UIButtonTest_Scale9, UIButtonTest_PressedAction);
 
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
         _uiLayer->addChild(_displayValueLabel);
 
         // Add the alert
         Text* alert = Text::create("Button scale9 render", "fonts/Marker Felt.ttf", 30);
         alert->setColor(Color3B(159, 168, 176));
         alert->setPosition(Vec2(widgetSize.width / 2.0f,
             widgetSize.height / 2.0f - alert->getContentSize().height * 1.75f));
         _uiLayer->addChild(alert);
 
         // Create the button
         Button* button = Button::create("button.png"/*, "buttonHighlighted.png"*/);
         // open scale9 render
         button->ignoreContentAdaptWithSize(false);
         button->setScale9Enabled(true);
         button->setPosition(Vec2(widgetSize.width / 2.0f - 200, widgetSize.height / 2.0f));
         button->setContentSize(Size(180.0f, 60.0f));
         button->setTitleText("Hello Scale9++");
		 //button->setTitleFontSize(22);
         button->setPressedActionEnabled(false);
         button->addTouchEventListener(OG_CALLBACK_2(UIButtonTest_Scale9_State_Change::touchEvent, this));
         _uiLayer->addChild(button);
 
         Button* button2 = Button::create("button.png", "buttonHighlighted.png");
         // open scale9 render
         button2->ignoreContentAdaptWithSize(false);
         button2->setScale9Enabled(true);
         button2->setTitleText("Hello scale9__");
		 //button2->setTitleFontSize(22);
		 auto title = button2->getTitleRenderer();
		 if (title) {
			 
			 title->setLocalZOrder(100); // 强行把文字提到最顶层
			 OGLOG("Title Text: %s, Pos: (%f, %f), Size: (%f, %f)",
				 title->getString().c_str(),
				 title->getPosition().x, title->getPosition().y,
				 title->getContentSize().width, title->getContentSize().height);
		 }
         button2->setPosition(Vec2(widgetSize.width / 2.0f + 200, widgetSize.height / 2.0f));
         button2->setContentSize(Size(180.0f, 60.0f));
         button2->setPressedActionEnabled(true);
         button2->addTouchEventListener(OG_CALLBACK_2(UIButtonTest_Scale9_State_Change::touchEvent, this));
         _uiLayer->addChild(button2);
         return true;
     }
     return false;
 }
 
 void UIButtonTest_Scale9_State_Change::touchEvent(Ref *pSender, Widget::TouchEventType type)
 {
     switch (type)
     {
     case Widget::TouchEventType::BEGAN:
         _displayValueLabel->setString(StringUtils::format("Touch Down"));
         break;
 
     case Widget::TouchEventType::MOVED:
         _displayValueLabel->setString(StringUtils::format("Touch Move"));
         break;
 
     case Widget::TouchEventType::ENDED:
     {
         _displayValueLabel->setString(StringUtils::format("Touch Up"));
         Button *btn = (Button*)pSender;
         if (btn->isScale9Enabled())
         {
             btn->setScale9Enabled(false);
         }
         else
             btn->setScale9Enabled(true);
     }
         break;
 
     case Widget::TouchEventType::CANCELED:
         _displayValueLabel->setString(StringUtils::format("Touch Cancelled"));
         break;
 
     default:
         break;
     }
 }
 //----------------UIButtonTest_PressedAction-------------------
 
 bool UIButtonTest_PressedAction::init()
 {
     if (Layer::init())
     {
		 Tests();NAV_BOTH(UIButtonTest_Scale9_State_Change, UIButtonTest_Title);

         Size widgetSize = Director::getInstance()->getWinSize();
  
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
         _uiLayer->addChild(_displayValueLabel);
 
         // Add the alert
         Text* alert = Text::create("Button Pressed Action", "fonts/Marker Felt.ttf", 30);
         alert->setColor(Color3B(159, 168, 176));
 
         alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                  widgetSize.height / 2.0f - alert->getContentSize().height * 1.75f));
 
         _uiLayer->addChild(alert);
 
         // Create the button
         Button* button = Button::create("animationbuttonnormal.png", "animationbuttonpressed.png");
         button->setPressedActionEnabled(true);
         button->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
         button->setColor(Color3B::GREEN);
         button->setOpacity(30);
         button->addTouchEventListener(OG_CALLBACK_2(UIButtonTest_PressedAction::touchEvent, this));
         button->setName("button");
         _uiLayer->addChild(button);
 
         Button* button2 = Button::create("animationbuttonnormal.png", "animationbuttonpressed.png");
         button2->setPosition(button->getPosition() + Vec2(100.0f,0.0f));
         button2->setName("button2");
         _uiLayer->addChild(button2);
 
         return true;
     }
     return false;
 }
 
 void UIButtonTest_PressedAction::touchEvent(Ref *pSender, Widget::TouchEventType type)
 {
     switch (type)
     {
         case Widget::TouchEventType::BEGAN:
             _displayValueLabel->setString(StringUtils::format("Touch Down"));
             break;
 
         case Widget::TouchEventType::MOVED:
             _displayValueLabel->setString(StringUtils::format("Touch Move"));
             break;
 
         case Widget::TouchEventType::ENDED:
         {
             _displayValueLabel->setString(StringUtils::format("Touch Up"));
             Button* btn = (Button*)_uiLayer->getChildByName("button");
             btn->loadTextureNormal("animationbuttonnormal.png");
 
             Button* btn2 = (Button*)_uiLayer->getChildByName("button2");
             //btn2->setAnchorPoint(Vec2(0,0.5));
 
			 Vec2 btnPos = btn->getPosition();
			 btn2->setPosition(Vec2(btnPos.x + btn2->getContentSize().width, btnPos.y ));
         }
             break;
 
         case Widget::TouchEventType::CANCELED:
             _displayValueLabel->setString(StringUtils::format("Touch Cancelled"));
             break;
 
         default:
             break;
     }
 }
 
 //----------------UIButtonTest_Title-------------------
 bool UIButtonTest_Title::init()
 {
	 if (Layer::init())
	 {
		 Tests();NAV_BOTH(UIButtonTest_PressedAction, UIButtonTestRemoveSelf);

		 Size widgetSize = Director::getInstance()->getWinSize();
	 

		 // Add a label in which the text button events will be displayed
		 _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
		 _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
		 _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
		 _uiLayer->addChild(_displayValueLabel);

		 // Add the alert
		 Text* alert = Text::create("Button with title, title should be flipped!", "fonts/Marker Felt.ttf", 30);
		 alert->setColor(Color3B(159, 168, 176));
		 alert->setPosition(Vec2(widgetSize.width / 2.0f,
			 widgetSize.height / 2.0f - alert->getContentSize().height * 1.75f));

		 _uiLayer->addChild(alert);

		 // Create the button with title
		 Button* button = Button::create("backtotoppressed.png", "backtotopnormal.png");
		 button->setTitleText("Title Button!");
		 button->setPosition(Vec2(widgetSize.width / 2.0f - 30, widgetSize.height / 2.0f + 30));
		 button->setTitleColor(Color3B::YELLOW);
		 OGASSERT(button->getTitleColor() == Color3B::YELLOW, "Button setTitleColor & getTitleColor not match!");
		 button->addTouchEventListener(OG_CALLBACK_2(UIButtonTest_Title::touchEvent, this));
		 _uiLayer->addChild(button);
		 button->setFlippedX(true);
		 auto label = button->getTitleRenderer();
		 label->setScale(4.0);
		 button->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(1.0f, 1.2f),
			 ScaleTo::create(1.0f, 1.0f), nullptr)));


		 TextBMFont *text = TextBMFont::create("BMFont", "bitmapFontTest2.fnt");
		 text->setPosition(button->getPosition() + Vec2(button->getContentSize().width  - offsetX, 30.0f));
		 text->setColor(Color3B::YELLOW);
		 text->setOpacity(50);
		 text->setName("text");

		 auto button2 = Button::create("backtotopnormal.png", "backtotoppressed.png");
		 button2->setTitleFontName("fonts/helvetica-32.fnt");
		 button2->setTitleText("BMFont");
		 //the change of font size will take no effect.
		 button2->setTitleFontSize(30);
		 button2->setPosition(Vec2(button->getPosition() + Vec2(-offsetX, -80.0f)));
		 this->addChild(button2);

		 _uiLayer->addChild(text);

		 return true;
	 }
	 return false;
 }
 
 
 void UIButtonTest_Title::touchEvent(Ref *pSender, Widget::TouchEventType type)
 {
     switch (type)
     {
         case Widget::TouchEventType::BEGAN:
             _displayValueLabel->setString(StringUtils::format("Touch Down"));
             break;
 
         case Widget::TouchEventType::MOVED:
             _displayValueLabel->setString(StringUtils::format("Touch Move"));
             break;
 
         case Widget::TouchEventType::ENDED:
         {
             _displayValueLabel->setString(StringUtils::format("Touch Up"));
             TextBMFont *text = (TextBMFont*)_uiLayer->getChildByName("text");
             text->setFntFile("bitmapFontTest2.fnt");
             if (text->getString() == "BMFont") {
                 text->setString("Hello");
             }
             else{
                 text->setString("BMFont");
             }
         }
             break;
 
         case Widget::TouchEventType::CANCELED:
             _displayValueLabel->setString(StringUtils::format("Touch Cancelled"));
             break;
 
         default:
             break;
     }
 }
 //----------------UIButtonTestRemoveSelf-------------------
 bool UIButtonTestRemoveSelf::init()
 {
     if (Layer::init())
     {
		 Tests();NAV_BOTH(UIButtonTest_Title, UIButtonTestSwitchScale9);

         Size widgetSize = Director::getInstance()->getWinSize();
	 
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
         _uiLayer->addChild(_displayValueLabel);
 
         // Add the alert
         Text* alert = Text::create("Remove Self in the Button's Callback shouldn't cause crash!","fonts/Marker Felt.ttf",10);
         alert->setColor(Color3B(159, 168, 176));
 
         alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                 widgetSize.height / 2.0f - alert->getContentSize().height * 2.75f));
 
         _uiLayer->addChild(alert);
 
         Layout *layout = Layout::create();
         layout->setContentSize(widgetSize * 0.6f);
         layout->setBackGroundColor(Color3B::GREEN);
         layout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
         layout->setBackGroundColorOpacity(100);
         //layout->setPosition(Size(widgetSize.width/2, widgetSize.height/2));
		 layout->setPosition((widgetSize - layout->getContentSize()) / 2);
         layout->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
         layout->setTag(12);
         _uiLayer->addChild(layout);
 
         // Create the button
         Button* button = Button::create("animationbuttonnormal.png",
                                         "animationbuttonpressed.png");
         button->setPosition((widgetSize - layout->getContentSize()) / 2);
         //        button->addTouchEventListener(this, toucheventselector(UIButtonTest::touchEvent));
         button->addTouchEventListener(OG_CALLBACK_2(UIButtonTestRemoveSelf::touchEvent, this));
         layout->addChild(button);
 
 
 
         return true;
     }
     return false;
 }
 
 void UIButtonTestRemoveSelf::touchEvent(Ref *pSender, Widget::TouchEventType type)
 {
     switch (type)
     {
         case Widget::TouchEventType::BEGAN:
             _displayValueLabel->setString(StringUtils::format("Touch Down"));
             break;
 
         case Widget::TouchEventType::MOVED:
             _displayValueLabel->setString(StringUtils::format("Touch Move"));
             break;
 
         case Widget::TouchEventType::ENDED:
         {
             _displayValueLabel->setString(StringUtils::format("Touch Up"));
             auto layout = _uiLayer->getChildByTag(12);
             layout->removeFromParentAndCleanup(true);
         }
             break;
 
         case Widget::TouchEventType::CANCELED:
             _displayValueLabel->setString(StringUtils::format("Touch Cancelled"));
             break;
 
         default:
             break;
     }
 } 
 //----------------UIButtonTestSwitchScale9-------------------
 bool UIButtonTestSwitchScale9::init()
 {
     if (Layer::init())
     {
		 Tests();NAV_BOTH(UIButtonTestRemoveSelf, UIButtonTestZoomScale);


         Size widgetSize = Director::getInstance()->getWinSize();
	 
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f - offsetX, widgetSize.height / 2.0f));
         _uiLayer->addChild(_displayValueLabel);
 
 
         // Create the button
         Button* button = Button::create("animationbuttonnormal.png",
                                         "animationbuttonpressed.png");
         button->setPosition((widgetSize - button->getContentSize()) / 2);
         button->addTouchEventListener(OG_CALLBACK_2(UIButtonTestSwitchScale9::touchEvent, this));
         button->setTitleText("Button Title");
         button->ignoreContentAdaptWithSize(false);
 
         _uiLayer->addChild(button);
 
 
 
         return true;
     }
     return false;
 }
 
 void UIButtonTestSwitchScale9::touchEvent(Ref *pSender, Widget::TouchEventType type)
 {
     switch (type)
     {
         case Widget::TouchEventType::BEGAN:
             _displayValueLabel->setString(StringUtils::format("Touch Down"));
             break;
 
         case Widget::TouchEventType::MOVED:
             _displayValueLabel->setString(StringUtils::format("Touch Move"));
             break;
 
         case Widget::TouchEventType::ENDED:
         {
             _displayValueLabel->setString(StringUtils::format("Touch Up"));
             auto btn = ((Button*)pSender);
             btn->setScale9Enabled(!btn->isScale9Enabled());
             btn->setContentSize(Size(200.0f,100.0f));
         }
             break;
 
         case Widget::TouchEventType::CANCELED:
             _displayValueLabel->setString(StringUtils::format("Touch Cancelled"));
             break;
 
         default:
             break;
     }
 }
 //----------------UIButtonTestZoomScale-------------------
 bool UIButtonTestZoomScale::init()
 {
     if (Layer::init())
     {

		 Tests();NAV_BOTH(UIButtonTestSwitchScale9, UIButtonTextOnly);

         Size widgetSize = Director::getInstance()->getWinSize();
	 
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("Zoom Scale: -0.5", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f + 20));
         _uiLayer->addChild(_displayValueLabel);
 
 
         // Create the button
         Button* button = Button::create("animationbuttonnormal.png",
                                         "animationbuttonpressed.png");
         button->setPosition(Vec2(widgetSize.width / 2.0f- offsetX, widgetSize.height / 2.0f + 20));
         button->setPressedActionEnabled(true);
         button->addClickEventListener([=](Ref* sender){
             OGLOG("Button clicked, position = (%f, %f)", button->getPosition().x,
                   button->getPosition().y);
 
         });
         button->setName("button");
         _uiLayer->addChild(button);
         button->setZoomScale(-0.5);
 
         Slider* slider = Slider::create();
         slider->loadBarTexture("sliderTrack.png");
         slider->loadSlidBallTextures("sliderThumb.png","" /*"slider_bar_active_9patch2.png", "sliderballnormal.png"*/);
		 // slider->setEnabled(false);
         slider->loadProgressBarTexture("sliderProgress.png");
         slider->setPosition(Vec2(widgetSize.width / 2.0f - offsetX , widgetSize.height / 2.0f + offsetY));
         slider->addEventListener(OG_CALLBACK_2(UIButtonTestZoomScale::sliderEvent, this));
         slider->setPercent(button->getZoomScale()*100);
         _uiLayer->addChild(slider);
         return true;
     }
     return false;
 }
 
 void UIButtonTestZoomScale::sliderEvent(Ref *pSender, Slider::EventType type)
 {
     if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
     {
         Slider* slider = dynamic_cast<Slider*>(pSender);
         int percent = slider->getPercent();
         Button* btn = (Button*)_uiLayer->getChildByName("button");
         float zoomScale = percent * 0.01;
         btn->setZoomScale(zoomScale);
         _displayValueLabel->setString(StringUtils::format("Zoom Scale: %f", zoomScale));
     }
 } 
 //----------------UIButtonTextOnly-------------------
 bool UIButtonTextOnly::init()
 {
     if (Layer::init())
     {
		 Tests();NAV_BOTH(UIButtonTestZoomScale, UIButtonIgnoreContentSizeTest);

         Size widgetSize = Director::getInstance()->getWinSize();
	 

         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("Text Only Button", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f + 20));
         _uiLayer->addChild(_displayValueLabel);
 
 
         // Create the button
         auto button = Button::create();
         button->setPositionNormalized(Vec2(0.5f, 0.5f));
 
         button->setTitleText("PLAY GAME");
         OGLOG("content size should be greater than 0:  width = %f, height = %f", button->getContentSize().width,
               button->getContentSize().height);
         button->setZoomScale(0.3f);
         button->setPressedActionEnabled(true);
         button->addClickEventListener([](Ref* sender) {
             OGLOG("clicked!");
         });
         _uiLayer->addChild(button);
 
         return true;
     }
     return false;
 }
 //----------------UIButtonIgnoreContentSizeTest-------------------
 bool UIButtonIgnoreContentSizeTest::init()
 {
     if (Layer::init())
     {
		 Tests();NAV_BOTH(UIButtonTextOnly, UIButtonTitleEffectTest);

         Size widgetSize = Director::getInstance()->getWinSize();
		 
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("Button IgnoreContent Size Test", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f + 20));
         _uiLayer->addChild(_displayValueLabel);
 
 
         // Create the button
         auto button = Button::create("animationbuttonnormal.png",
                                      "animationbuttonpressed.png");
         button->ignoreContentAdaptWithSize(false);
         button->setContentSize(Size(200.0f,100.0f));
         button->setPositionNormalized(Vec2(0.3f, 0.5f));
         button->setTitleText("PLAY GAME");
         button->setZoomScale(0.3f);
         button->setPressedActionEnabled(true);
         button->addClickEventListener([=](Ref* sender) {
             OGLOG("clicked!");
             button->setScale(1.2f);
         });
         _uiLayer->addChild(button);
 
         // Create the button
         auto button2 = Button::create("animationbuttonnormal.png",
                                      "animationbuttonpressed.png");
         button2->ignoreContentAdaptWithSize(false);
         button2->setContentSize(Size(200.0f,100.0f));
         button2->setPositionNormalized(Vec2(0.8f, 0.5f));
         button2->setTitleText("PLAY GAME");
         button2->setZoomScale(0.3f);
         button2->setPressedActionEnabled(true);
         button2->addClickEventListener([=](Ref* sender) {
             button2->runAction(ScaleTo::create(1.0f, 1.2f));
             OGLOG("clicked!");
         });
         _uiLayer->addChild(button2);
 
         return true;
     }
     return false;
 } 
 //----------------UIButtonTitleEffectTest-------------------
 bool UIButtonTitleEffectTest::init()
 {
     if (Layer::init())
     {

		 Tests();NAV_BOTH(UIButtonIgnoreContentSizeTest, UIButtonFlipTest);

         Size widgetSize = Director::getInstance()->getWinSize();
		 
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("Button Title Effect", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f + 20));
         _uiLayer->addChild(_displayValueLabel);
 
 
         // Create the button
         auto button = Button::create("animationbuttonnormal.png",
                                      "animationbuttonpressed.png");
         button->setPositionNormalized(Vec2(0.3f, 0.5f));
         button->setTitleText("PLAY GAME");
         button->setTitleFontName("fonts/Marker Felt.ttf");
         button->setZoomScale(0.3f);
         button->setScale(2.0f);
         button->setPressedActionEnabled(true);
         Label *title = button->getTitleRenderer();
         button->setTitleColor(Color3B::RED);
         title->enableShadow(Color4B::BLACK,Vec2(2,-2));
 
 
         _uiLayer->addChild(button);
 
         // Create the button
         auto button2 = Button::create("animationbuttonnormal.png",
                                       "animationbuttonpressed.png");
         button2->setPositionNormalized(Vec2(0.8f, 0.5f));
         button2->setTitleText("PLAY GAME");
         auto title2 = button2->getTitleRenderer();
         title2->enableOutline(Color4B::GREEN, 3);
         _uiLayer->addChild(button2);
 
         return true;
     }
     return false;
 }
 
 //----------------UIButtonFlipTest-------------------
 bool UIButtonFlipTest::init()
 {
     if (Layer::init())
     {
		 Tests();NAV_BOTH(UIButtonTitleEffectTest, UIButtonNormalDefaultTest);


         Size widgetSize = Director::getInstance()->getWinSize();
 
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("Button X Flipped", "fonts/Marker Felt.ttf",20);
         _displayValueLabel->setPositionNormalized(Vec2(0.3f, 0.7f));
         _uiLayer->addChild(_displayValueLabel);
 
 
         // Create the button
         auto button = Button::create("animationbuttonnormal.png",
                                      "animationbuttonpressed.png");
         button->setPositionNormalized(Vec2(0.3f, 0.5f));
         button->setTitleText("PLAY GAME");
         button->setTitleFontName("fonts/Marker Felt.ttf");
         button->setZoomScale(0.3f);
         button->setScale(2.0f);
         button->setFlippedX(true);
         button->setPressedActionEnabled(true);
 
 
         _uiLayer->addChild(button);
 
         // Create the button
         auto button2 = Button::create("animationbuttonnormal.png",
                                       "animationbuttonpressed.png");
         button2->setPositionNormalized(Vec2(0.8f, 0.5f));
         button2->setTitleText("PLAY GAME");
         button2->setFlippedY(true);
         _uiLayer->addChild(button2);
 
         auto titleLabel = Text::create("Button Y flipped", "Arial", 20);
         titleLabel->setPositionNormalized(Vec2(0.8f, 0.7f));
         this->addChild(titleLabel);
 
         return true;
     }
     return false;
 } 
 //----------------UIButtonNormalDefaultTest-------------------
 bool UIButtonNormalDefaultTest::init()
 {
     if (Layer::init())
     {
		 Tests();NAV_BOTH(UIButtonFlipTest, UIButtonDisableDefaultTest);

         Size widgetSize = Director::getInstance()->getWinSize();
		 
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
         _uiLayer->addChild(_displayValueLabel);
 
         // Add the alert
         Text* alert = Text::create("Button should scale when clicked","fonts/Marker Felt.ttf",20);
         alert->setColor(Color3B(159, 168, 176));
 
         alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                 widgetSize.height / 2.0f - alert->getContentSize().height * 1.75f));
 
         _uiLayer->addChild(alert);
 
         // Create the button
         Button* button = Button::create("animationbuttonnormal.png");
         button->setPosition(Vec2(widgetSize.width / 2.0f - 80, widgetSize.height / 2.0f + 40));
         button->setZoomScale(0.4f);
         button->setPressedActionEnabled(true);
         _uiLayer->addChild(button);
 
         // Create the button
         Button* buttonScale9 = Button::create("button.png");
         // open scale9 render
         buttonScale9->setScale9Enabled(true);
         buttonScale9->setPosition(Vec2(widgetSize.width / 2.0f + 50, widgetSize.height / 2.0f + 40));
         buttonScale9->setContentSize(Size(150.0f, 70.0f));
         buttonScale9->setPressedActionEnabled(true);
         _uiLayer->addChild(buttonScale9);
 
 
 
 
         return true;
     }
     return false;
 }
  
 //----------------UIButtonDisableDefaultTest-------------------
 bool UIButtonDisableDefaultTest::init()
 {
     if (Layer::init())
     {

		 Tests();NAV_BOTH(UIButtonNormalDefaultTest, UIButtonCloneTest);


         Size widgetSize = Director::getInstance()->getWinSize();
	 

         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
         _uiLayer->addChild(_displayValueLabel);
 
         // Add the alert
         Text* alert = Text::create("Left button will turn normal when clicked","fonts/Marker Felt.ttf",20);
         alert->setColor(Color3B(159, 168, 176));
 
         alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                 widgetSize.height / 2.0f - alert->getContentSize().height * 1.75f));
 
         _uiLayer->addChild(alert);
         
		 auto sp = Sprite::create("animationbuttonnormal.png");
		 this->addChild(sp);
		 sp->setColor(Color3B(128,128,128));

         // Create the button
         Button* button = Button::create("animationbuttonnormal.png");
         button->setPosition(Vec2(widgetSize.width / 2.0f - 80, widgetSize.height / 2.0f + 40));
         button->setZoomScale(0.4f);
         button->setPressedActionEnabled(true);
         button->setBright(false);
         button->setScale9Enabled(true);
         button->setCapInsets(Rect(3,3,5,5));
         button->addClickEventListener([=](Ref*){
             button->setBright(true);
         });
         _uiLayer->addChild(button);
 
         // Create the button
         Button* buttonScale9 = Button::create("button.png");
         // open scale9 render
         buttonScale9->setScale9Enabled(true);
         buttonScale9->setPosition(Vec2(widgetSize.width / 2.0f + 50, widgetSize.height / 2.0f + 40));
         buttonScale9->setContentSize(Size(150.0f, 70.0f));
         buttonScale9->setPressedActionEnabled(true);
         buttonScale9->setEnabled(false);
         buttonScale9->setBright(false);
         _uiLayer->addChild(buttonScale9);
 
 
 
 
         return true;
     }
     return false;
 }
  
 //----------------UIButtonCloneTest-------------------
 bool UIButtonCloneTest::init()
 {
     if (Layer::init())
     {
		 Tests();NAV_BOTH(UIButtonDisableDefaultTest, Issue12249);

         Size widgetSize = Director::getInstance()->getWinSize();
	 
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f,
                                              widgetSize.height / 2.0f));
         _uiLayer->addChild(_displayValueLabel);
 
         // Add the alert
         Text* alert = Text::create("This test case shouldn't trigger the Assertion!",
                                    "fonts/Marker Felt.ttf",20);
         alert->setColor(Color3B(159, 168, 176));
 
         alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                 widgetSize.height / 2.0f
                                 - alert->getContentSize().height * 1.75f));
 
         _uiLayer->addChild(alert);
 
         // Create the button
         Button* button = Button::create("animationbuttonnormal.png");
         button->setPosition(Vec2(widgetSize.width / 2.0f - 80,
                                  widgetSize.height / 2.0f + 40));
         _uiLayer->addChild(button);
 
 
         OGASSERT(button->getTitleRenderer() == nullptr,
                  "Button title render must be nullptr ");
 
         auto buttonCopy = (Button*)button->clone();
         buttonCopy->setPosition(Vec2(widgetSize.width / 2.0f + 80,
                                      widgetSize.height / 2.0f + 40));
         this->addChild(buttonCopy);
         
         
         
         auto buttonScale9Copy = (Button*)button->clone();
         buttonScale9Copy->setPosition(button->getPosition() + Vec2(0.0f, +60.0f));
         buttonScale9Copy->setScale9Enabled(true);
         buttonScale9Copy->setContentSize(button->getContentSize() * 1.5f);
         this->addChild(buttonScale9Copy);
         
         
         auto buttonScale9Copy2 = (Button*)buttonScale9Copy->clone();
         buttonScale9Copy2->setPosition(buttonCopy->getPosition() + Vec2(0.0f, +60.0f));
         buttonScale9Copy2->setScale9Enabled(true);
         buttonScale9Copy2->setContentSize(buttonCopy->getContentSize() * 1.5f);
         this->addChild(buttonScale9Copy2);
         
         
         OGASSERT(button->getTitleRenderer() == nullptr,
                  "Original Button title render must be nullptr ");
 
         OGASSERT(buttonCopy->getTitleRenderer() == nullptr,
                  "Copied Button title render must be nullptr ");
 
         return true;
     }
     return false;
 }
  
 //----------------Issue12249-------------------
 bool Issue12249::init()
 {
     if (Layer::init())
     {
		 Tests();NAV_BOTH(UIButtonCloneTest, Issue17116);


         Size widgetSize = Director::getInstance()->getWinSize();
		 
         // Add a label in which the button events will be displayed
         _displayValueLabel = Text::create("Test Issue 12249", "fonts/Marker Felt.ttf",32);
         _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
         _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f,
                                              widgetSize.height / 2.0f));
         _uiLayer->addChild(_displayValueLabel);
         
         // Add the alert
         Text* alert = Text::create("This test case two buttons should seem to be same!",
                                    "fonts/Marker Felt.ttf",20);
         alert->setColor(Color3B(159, 168, 176));
         
         alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                 widgetSize.height / 2.0f
                                 - alert->getContentSize().height * 2.0f));
         
         _uiLayer->addChild(alert);
         
         // Create the button 1
         Button* button = nullptr, *button2 = nullptr;
         float btnWidth = 100;
         button = Button::create("button.png", "buttonHighlighted.png");
         button->setScale9Enabled(true);
         button->setContentSize(Size(btnWidth, button->getContentSize().height));
         button->setTitleText("Scale9 Button 1");
         button->setPosition(Vec2(widgetSize.width / 2.0f - btnWidth, widgetSize.height / 2.0f + 30));
         _uiLayer->addChild(button);
 
         // create button 2, load texture after button creation
         button2 = Button::create();
         button2->setScale9Enabled(true);
         button2->loadTextures("button.png", "buttonHighlighted.png", "");
         button2->setContentSize(Size(btnWidth, button2->getContentSize().height));
         button2->setTitleText("Scale9 Button 2");
         button2->setPosition(Vec2(widgetSize.width / 2.0f + btnWidth, widgetSize.height / 2.0f + 30));
         _uiLayer->addChild(button2);
         
         return true;
     }
     return false;
 } 
 //----------------Issue17116-------------------
 bool Issue17116::init()
 {
     if (Layer::init())
     {
		 Tests();NAV_BOTH(Issue12249, UIButtonWithPolygonInfo);

		 Size visibleSize = Director::getInstance()->getWinSize();
		 
         SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Images/issue_17116.plist");
         auto button = ui::Button::create();
         button->loadTextureNormal("buttons/play-big", ui::Widget::TextureResType::PLIST);
         button->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
         button->setOpacity(100);
         addChild(button);
         return true;
     }
     return false;
 }
 //----------------UIButtonWithPolygonInfo-------------------
 bool UIButtonWithPolygonInfo::init()
 {
	 if (Layer::init())
	 {
		 Tests();NAV_BOTH(Issue17116, UIButtonScale9ChangeSpriteFrame);

		 Size visibleSize = Director::getInstance()->getWinSize();
		 

		 SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Images/poly_test_textures.plist");
		 auto button = ui::Button::create();
		 button->loadTextureNormal("poly_test/wheel_disc_back.png", ui::Widget::TextureResType::PLIST);
		 button->setPosition(Vec2(visibleSize.width / 2 - 100, visibleSize.height / 2));
		 button->setScale(0.5);
		 button->setScale9Enabled(true);
		 addChild(button);

		 auto buttonCopy = button->clone();
		 button->setPosition(Vec2(visibleSize.width / 2 + 100, visibleSize.height / 2));
		 addChild(buttonCopy);

		 return true;
	 }
	 return false;
 }
 //----------------UIButtonScale9ChangeSpriteFrame-------------------

 bool UIButtonScale9ChangeSpriteFrame::init()
 {
	 if (Layer::init())
	 {
		 Tests();NAV_BOTH(UIButtonWithPolygonInfo, UIButtonTests);

		 ImageView* sprite = ImageView::create("Images/blocks.png");
		 sprite->setScale9Enabled(true);
		 sprite->setCapInsets(Rect(32, 32, 32, 32));
		 sprite->setContentSize(Size(96 * 1.5, 96));
		 sprite->setNormalizedPosition(Vec2(0.2f, 0.5f));
		 this->addChild(sprite);

		 auto button = Button::create("button.png", "buttonHighlighted.png");
		 button->setScale9Enabled(true);
		 button->setContentSize(Size(100.0f, 50.0f));
		 button->setNormalizedPosition(Vec2(0.6f, 0.5f));
		 button->setTitleText("Press me");
		 addChild(button);

		 button->addClickEventListener([sprite](Ref* button) {
			 sprite->loadTexture("Images/blocks9.png");
		 });


		 return true;
	 }
	 return false;
 }

