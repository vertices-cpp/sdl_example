#include "UISliderTest.h" 

using namespace orange::ui;


UISliderTests::UISliderTests()
{
	ADD_TEST_CASE(UISliderTest);
	ADD_TEST_CASE(UISliderTest_Scale9);
	ADD_TEST_CASE(UISliderTest_Scale9_State_Change);
	ADD_TEST_CASE(UISliderNormalDefaultTest);
	ADD_TEST_CASE(UISliderDisabledDefaultTest);
	ADD_TEST_CASE(UISliderNewEventCallbackTest);
	ADD_TEST_CASE(UISliderIssue12249Test);
}

// UISliderTest
bool UISliderTest::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UISliderIssue12249Test, UISliderTest_Scale9);

		Size widgetSize = Director::getInstance()->getWinSize();

		// 无锚点居中：x减去一半宽度，y放在中心偏上
		_displayValueLabel = TextBMFont::create("Move the slider thumb", "ccb/markerfelt24shadow.fnt");
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f - _displayValueLabel->getContentSize().width / 2.0f, widgetSize.height / 2.0f - 80.0f));
		_uiLayer->addChild(_displayValueLabel);

		// alert 放在中心偏下
		Text* alert = Text::create("Slider", "fonts/Marker Felt.ttf", 30);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2(widgetSize.width / 2.0f - alert->getContentSize().width / 2.0f, widgetSize.height / 2.0f + 40.0f));
		_uiLayer->addChild(alert);

		// Create the slider
		Slider* slider = Slider::create();
		slider->loadBarTexture("sliderTrack.png");
		slider->loadSlidBallTextures("sliderThumb.png", "sliderThumb.png", "");
		slider->loadProgressBarTexture("sliderProgress.png");
		slider->setMaxPercent(10000);
		slider->setPosition(Vec2(widgetSize.width / 2.0f - slider->getContentSize().width / 2.0f, widgetSize.height / 2.0f));
		slider->addEventListener(OG_CALLBACK_2(UISliderTest::sliderEvent, this));
		_uiLayer->addChild(slider);

		_slider = slider;

		TTFConfig ttfConfig("fonts/arial.ttf", 15);
		auto label1 = Label::createWithTTF(ttfConfig, "Print Resources");
		auto item1 = MenuItemLabel::create(label1, OG_CALLBACK_1(UISliderTest::printWidgetResources, this));
		// 无锚点：放到屏幕左下角
		item1->setPosition(Vec2(60, widgetSize.height - 60.0f));
		auto pMenu1 = Menu::create(item1, nullptr);
		pMenu1->setPosition(Vec2(0.0f, 0.0f));
		this->addChild(pMenu1, 10);

		return true;
	}
	return false;
}

void UISliderTest::sliderEvent(Ref *pSender, Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(pSender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		_displayValueLabel->setString(StringUtils::format("Percent %f", 10000.0 * percent / maxPercent));
	}
}
void UISliderTest::printWidgetResources(orange::Ref* /*sender*/)
{
	orange::ResourceData textureFile = _slider->getBackFile();
	OGLOG("textureFile  Name : %s, Type: %d", textureFile.file.c_str(), textureFile.type);
	orange::ResourceData progressBarTextureFile = _slider->getProgressBarFile();
	OGLOG("progressBarTextureFile  Name : %s, Type: %d", progressBarTextureFile.file.c_str(), progressBarTextureFile.type);
	orange::ResourceData slidBallNormalTextureFile = _slider->getBallNormalFile();
	OGLOG("slidBallNormalTextureFile  Name : %s, Type: %d", slidBallNormalTextureFile.file.c_str(), slidBallNormalTextureFile.type);
	orange::ResourceData slidBallPressedTextureFile = _slider->getBallPressedFile();
	OGLOG("slidBallPressedTextureFile  Name : %s, Type: %d", slidBallPressedTextureFile.file.c_str(), slidBallPressedTextureFile.type);
	orange::ResourceData slidBallDisabledTextureFile = _slider->getBallDisabledFile();
	OGLOG("slidBallDisabledTextureFile  Name : %s, Type: %d", slidBallDisabledTextureFile.file.c_str(), slidBallDisabledTextureFile.type);
}

// UISliderTest_Scale9
bool UISliderTest_Scale9::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UISliderTest, UISliderTest_Scale9_State_Change);

		Size widgetSize = Director::getInstance()->getWinSize();

		_displayValueLabel = Text::create("Move the slider thumb", "fonts/Marker Felt.ttf", 32);
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f - _displayValueLabel->getContentSize().width / 2.0f, widgetSize.height / 2.0f - 80.0f));
		_uiLayer->addChild(_displayValueLabel);

		Text *alert = Text::create("Slider scale9 render", "fonts/Marker Felt.ttf", 30);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2(widgetSize.width / 2.0f - alert->getContentSize().width / 2.0f, widgetSize.height / 2.0f + 40.0f));
		_uiLayer->addChild(alert);

		Slider* slider = Slider::create();
		slider->loadBarTexture("sliderTrack2.png");
		slider->loadSlidBallTextures("sliderThumb.png", "sliderThumb.png", "");
		slider->loadProgressBarTexture("slider_bar_active_9patch.png");
		slider->setScale9Enabled(true);
		slider->setCapInsets(Rect(0.0f, 0.0f, 0.0f, 0.0f));
		slider->setContentSize(Size(250.0f, 19.0f));
		// 宽度250，减去一半
		slider->setPosition(Vec2(widgetSize.width / 2.0f - 125.0f, widgetSize.height / 2.0f));
		slider->addEventListener(OG_CALLBACK_2(UISliderTest_Scale9::sliderEvent, this));
		_uiLayer->addChild(slider);

		return true;
	}
	return false;
}

void UISliderTest_Scale9::sliderEvent(Ref *pSender, Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(pSender);
		int percent = slider->getPercent();
		_displayValueLabel->setString(StringUtils::format("Percent %d", percent));
	}
}

// UISliderTest_Scale9_State_Change
bool UISliderTest_Scale9_State_Change::init()
{
	if (MyLayer::init())
	{
		//9宫未实现
		//NAV_BOTH(UISliderTest_Scale9, UISliderNormalDefaultTest);

		Size widgetSize = Director::getInstance()->getWinSize();

		_displayValueLabel = Text::create("Click the slider thumb", "fonts/Marker Felt.ttf", 32);
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f - _displayValueLabel->getContentSize().width / 2.0f, widgetSize.height / 2.0f - 80.0f));
		_uiLayer->addChild(_displayValueLabel);

		Text *alert = Text::create("Slider scale9 render", "fonts/Marker Felt.ttf", 30);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2(widgetSize.width / 2.0f - alert->getContentSize().width / 2.0f, widgetSize.height / 2.0f + 40.0f));
		_uiLayer->addChild(alert);

		Slider* slider = Slider::create();
		slider->loadBarTexture("sliderballnormal.png");
		slider->loadSlidBallTextures("sliderThumb.png", "sliderThumb.png", "");
		slider->loadProgressBarTexture("slider_bar_active_9patch.png");
		slider->ignoreContentAdaptWithSize(false);
		slider->setScale9Enabled(true);
		slider->setCapInsets(Rect(0.0f, 0.0f, 0.0f, 0.0f));
		slider->setContentSize(Size(200.0f, 60.0f));
		slider->setPosition(Vec2(widgetSize.width / 2.0f - 100.0f, widgetSize.height / 2.0f));
		slider->addTouchEventListener([=](Ref* /*sender*/, Widget::TouchEventType type) {
			if (type == Widget::TouchEventType::ENDED) {
				if (slider->isScale9Enabled())
				{
					slider->setScale9Enabled(false);
				}
				else
					slider->setScale9Enabled(true);
			}
		});
		_uiLayer->addChild(slider);

		return true;
	}
	return false;
}

void UISliderTest_Scale9_State_Change::sliderEvent(Ref *pSender, Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(pSender);
		int percent = slider->getPercent();
		_displayValueLabel->setString(StringUtils::format("Percent %d", percent));
	}
}

// UISliderNormalDefaultTest
bool UISliderNormalDefaultTest::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UISliderTest_Scale9_State_Change, UISliderDisabledDefaultTest);

		Size widgetSize = Director::getInstance()->getWinSize();

		_displayValueLabel = Text::create("", "Arial", 32);
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f - _displayValueLabel->getContentSize().width / 2.0f, widgetSize.height / 2.0f - 100.0f));
		_uiLayer->addChild(_displayValueLabel);

		Text* alert = Text::create("when pressed, the slider ball should scale", "fonts/Marker Felt.ttf", 20);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2(widgetSize.width / 2.0f - alert->getContentSize().width / 2.0f, widgetSize.height / 2.0f + 100.0f));
		_uiLayer->addChild(alert);

		Slider* slider = Slider::create();
		slider->loadBarTexture("sliderTrack.png");
		slider->loadSlidBallTextures("sliderThumb.png");
		slider->setPosition(Vec2(widgetSize.width / 2.0f - slider->getContentSize().width / 2.0f, widgetSize.height / 2.0f - 40.0f));
		_uiLayer->addChild(slider);

		Slider* sliderScale9 = (Slider*)slider->clone();
		sliderScale9->setScale9Enabled(true);
		sliderScale9->setCapInsets(Rect(0.0f, 0.0f, 0.0f, 0.0f));
		sliderScale9->setZoomScale(1.0);
		sliderScale9->setContentSize(Size(250.0f, 19.0f));
		sliderScale9->setPosition(Vec2(widgetSize.width / 2.0f - 125.0f, widgetSize.height / 2.0f + 40.0f));
		_uiLayer->addChild(sliderScale9);

		return true;
	}
	return false;
}

// UISliderDisabledDefaultTest
bool UISliderDisabledDefaultTest::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UISliderNormalDefaultTest, UISliderNewEventCallbackTest);

		Size widgetSize = Director::getInstance()->getWinSize();

		_displayValueLabel = Text::create("", "Arial", 32);
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f - _displayValueLabel->getContentSize().width / 2.0f, widgetSize.height / 2.0f - 100.0f));
		_uiLayer->addChild(_displayValueLabel);

		Text* alert = Text::create("slider ball should be gray.", "fonts/Marker Felt.ttf", 20);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2(widgetSize.width / 2.0f - alert->getContentSize().width / 2.0f, widgetSize.height / 2.0f + 100.0f));
		_uiLayer->addChild(alert);

		Slider* slider = Slider::create();
		slider->loadBarTexture("slidbar.png");
		slider->loadSlidBallTextureNormal("sliderballnormal.png");
		slider->setEnabled(false);
		slider->setBright(false);
		slider->setPosition(Vec2(widgetSize.width / 2.0f - slider->getContentSize().width / 2.0f, widgetSize.height / 2.0f - 40.0f));
		_uiLayer->addChild(slider);

		Slider* sliderScale9 = (Slider*)slider->clone();
		sliderScale9->setScale9Enabled(true);
		sliderScale9->setEnabled(false);
		sliderScale9->setBright(false);
		sliderScale9->setCapInsets(Rect(0.0f, 0.0f, 0.0f, 0.0f));
		sliderScale9->setContentSize(Size(250.0f, 10.0f));
		sliderScale9->setPosition(Vec2(widgetSize.width / 2.0f - 125.0f, widgetSize.height / 2.0f + 40.0f));
		_uiLayer->addChild(sliderScale9);

		return true;
	}
	return false;
}

// UISliderNewEventCallbackTest
bool UISliderNewEventCallbackTest::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UISliderDisabledDefaultTest, UISliderIssue12249Test);

		Size widgetSize = Director::getInstance()->getWinSize();

		_displayValueLabel = Text::create("", "Arial", 32);
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f - _displayValueLabel->getContentSize().width / 2.0f, widgetSize.height / 2.0f - 100.0f));
		_uiLayer->addChild(_displayValueLabel);

		Text* alert = Text::create("See console output for Slider Down and Up event.", "fonts/Marker Felt.ttf", 20);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2(widgetSize.width / 2.0f - alert->getContentSize().width / 2.0f, widgetSize.height / 2.0f + 100.0f));
		_uiLayer->addChild(alert);

		Slider* slider = Slider::create();
		slider->loadBarTexture("sliderTrack.png");
		slider->loadSlidBallTextures("sliderThumb.png", "sliderThumb.png", "");
		slider->loadProgressBarTexture("sliderProgress.png");
		slider->setMaxPercent(1000);
		slider->setPosition(Vec2(widgetSize.width / 2.0f - slider->getContentSize().width / 2.0f, widgetSize.height / 2.0f));
		slider->addEventListener([=](Ref* widget, Slider::EventType type)
		{
			Slider* slider = (Slider*)widget;
			if (type == Slider::EventType::ON_SLIDEBALL_DOWN)
			{
				OGLOG("slider button pressed!");
			}
			else if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
			{
				OGLOG("slider is moving! percent = %f", 100.0f * slider->getPercent() / slider->getMaxPercent());
			}
			else if (type == Slider::EventType::ON_SLIDEBALL_UP)
			{
				OGLOG("slider button is released.");
			}
		});
		_uiLayer->addChild(slider);

		return true;
	}
	return false;
}

// UISliderIssue12249Test
bool UISliderIssue12249Test::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UISliderNewEventCallbackTest, UISliderTest);

		Size widgetSize = Director::getInstance()->getWinSize();

		_displayValueLabel = TextBMFont::create("Move the slider thumb", "ccb/markerfelt24shadow.fnt");
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f - _displayValueLabel->getContentSize().width / 2.0f, widgetSize.height / 2.0f - 60.0f));
		_uiLayer->addChild(_displayValueLabel);

		Slider* slider = Slider::create();
		slider->setScale9Enabled(true);
		slider->loadBarTexture("sliderTrack.png");
		slider->loadSlidBallTextures("sliderThumb.png", "sliderThumb.png", "");
		slider->loadProgressBarTexture("sliderProgress.png");
		slider->setContentSize(Size(300, slider->getContentSize().height * 1.5));
		slider->setMaxPercent(10000);
		slider->setPosition(Vec2(widgetSize.width / 2.0f - 150.0f, widgetSize.height / 2.0f));
		slider->addEventListener(OG_CALLBACK_2(UISliderIssue12249Test::sliderEvent, this));
		_uiLayer->addChild(slider);

		return true;
	}
	return false;
}

void UISliderIssue12249Test::sliderEvent(Ref *pSender, Slider::EventType type)
{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		Slider* slider = dynamic_cast<Slider*>(pSender);
		int percent = slider->getPercent();
		int maxPercent = slider->getMaxPercent();
		_displayValueLabel->setString(StringUtils::format("Percent %f", 10000.0 * percent / maxPercent));
	}
}