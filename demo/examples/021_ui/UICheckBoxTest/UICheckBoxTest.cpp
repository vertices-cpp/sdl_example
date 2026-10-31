 

#include "UICheckBoxTest.h"
#include "TestNavigation.h"

USING_OG;
using namespace ui;
 
// UICheckBoxTest
UICheckBoxTest::UICheckBoxTest()
: _displayValueLabel(nullptr)
{ 
}
void UICheckBoxTest::Tests()
{
	Size widgetSize = Director::getInstance()->getWinSize();
	_uiLayer = Layer::create();
	_uiLayer->setContentSize(widgetSize);
	_uiLayer->setName("_uiLayer");
	addChild(_uiLayer);
}

UICheckBoxTest::~UICheckBoxTest()
{
	OGLOG("======== UICheckBoxTest 被 delete 了 ========");
}

bool UICheckBoxTest::init()
{
    if (Layer::init())
    {
		Tests();
		this->setName("UICheckBoxTest");

		NAV_BOTH(UICheckBoxCopyTest, UICheckBoxDefaultBehaviorTest);

        Size widgetSize = Director::getInstance()->getWinSize();
        
        // Add a label in which the checkbox events will be displayed
        _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
		_displayValueLabel->setName("_displayValueLabel");

        _uiLayer->addChild(_displayValueLabel);
        
        // Add the alert
        Text* alert = Text::create("CheckBox","fonts/Marker Felt.ttf",30 );
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 1.75f));
		alert->setName("alert");

		_uiLayer->addChild(alert);   
		
        
        // Create the checkbox
        _checkBox = CheckBox::create("check_box_normal.png",
                                              "check_box_normal_press.png",
                                              "check_box_active.png",
                                              "check_box_normal_disable.png",
                                              "check_box_active_disable.png");
        _checkBox->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
        
        _checkBox->addEventListener(OG_CALLBACK_2(UICheckBoxTest::selectedEvent, this));
		_checkBox->setName("_checkBox");

        _uiLayer->addChild(_checkBox);
		

        TTFConfig ttfConfig("fonts/arial.ttf", 15);
        auto label1 = Label::createWithTTF(ttfConfig, "Print Resources");
        auto item1 = MenuItemLabel::create(label1, OG_CALLBACK_1(UICheckBoxTest::printWidgetResources, this));
        item1->setPosition(Vec2(60, 51));
		item1->setName("item1");

        auto pMenu1 = Menu::create(item1, nullptr);
		pMenu1->setName("pMenu1");

        pMenu1->setPosition(Vec2(0.0f, 0.0f));
        this->addChild(pMenu1, 10);
        
        return true;
    }
    return false;
}

void UICheckBoxTest::selectedEvent(Ref* pSender,CheckBox::EventType type)
{
    switch (type)
    {
        case CheckBox::EventType::SELECTED:
            _displayValueLabel->setString(StringUtils::format("Selected"));
            break;
            
        case CheckBox::EventType::UNSELECTED:
            _displayValueLabel->setString(StringUtils::format("Unselected"));
            break;
            
        default:
            break;
    }
    
}

void UICheckBoxTest::printWidgetResources(Ref* sender)
{
    ResourceData backGroundFileName = _checkBox->getBackNormalFile();
    OGLOG("backGroundFile  Name : %s, Type: %d", backGroundFileName.file.c_str(),backGroundFileName.type);
    ResourceData backGroundSelectedFileName = _checkBox->getBackPressedFile();
    OGLOG("backGroundSelectedFile  Name : %s, Type: %d", backGroundSelectedFileName.file.c_str(), backGroundSelectedFileName.type);
    ResourceData backGroundDisabledFileName = _checkBox->getBackDisabledFile();
    OGLOG("backGroundDisabledFile  Name : %s, Type: %d", backGroundDisabledFileName.file.c_str(), backGroundDisabledFileName.type);
    ResourceData frontCrossFileName = _checkBox->getCrossNormalFile();
    OGLOG("frontCrossFile  Name : %s, Type: %d", frontCrossFileName.file.c_str(), frontCrossFileName.type);
    ResourceData frontCrossDisabledFileName = _checkBox->getCrossDisabledFile();
    OGLOG("frontCrossDisabledFile  Name : %s, Type: %d", frontCrossDisabledFileName.file.c_str(), frontCrossDisabledFileName.type);
}

 

bool UICheckBoxDefaultBehaviorTest::init()
{
    if (Layer::init())
    {
		this->setName("UICheckBoxDefaultBehaviorTest");

		Tests();NAV_BOTH(UICheckBoxTest, UICheckBoxCopyTest);

        Size widgetSize = Director::getInstance()->getWinSize();
        
        // Add a label in which the checkbox events will be displayed
        _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
		_displayValueLabel->setName("_displayValueLabel");

        _uiLayer->addChild(_displayValueLabel);
        
        // Add the alert
        Text* alert = Text::create("Only left two and the last checkbox can be clicked!","fonts/Marker Felt.ttf",20 );
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                widgetSize.height / 2.0f - alert->getContentSize().height * 1.75f));
		alert->setName("alert");

        _uiLayer->addChild(alert);
        
        // Create the checkbox
        CheckBox* checkBox = CheckBox::create("check_box_normal.png",
                                              "check_box_active.png");
        checkBox->setPosition(Vec2(widgetSize.width / 2.0f - 50, widgetSize.height / 2.0f));
		checkBox->setName("checkBox");

        _uiLayer->addChild(checkBox);
        
        
        // Create the checkbox
        CheckBox* checkBox2 = CheckBox::create("check_box_normal.png",
                                              "check_box_active.png");
		checkBox2->setName("checkBox2");

        checkBox2->setPosition(Vec2(widgetSize.width / 2.0f - 150, widgetSize.height / 2.0f));
        checkBox2->ignoreContentAdaptWithSize(false);
        checkBox2->setZoomScale(0.5f);
        checkBox2->setContentSize(Size(80.0f,80.0f));
        checkBox2->setName("bigCheckBox");
        _uiLayer->addChild(checkBox2);
        
        
        // Create the checkbox
        CheckBox* checkBoxDisabled = CheckBox::create("check_box_normal.png",
                                              "check_box_active.png");
        checkBoxDisabled->setPosition(Vec2(widgetSize.width / 2.0f + 20, widgetSize.height / 2.0f));
        checkBoxDisabled->setEnabled(false);
        checkBoxDisabled->setBright(false);//没有灰色
		checkBoxDisabled->setName("checkBoxDisabled");

        _uiLayer->addChild(checkBoxDisabled);
        
        CheckBox* checkBoxDisabled2 = CheckBox::create("check_box_normal.png",
                                              "check_box_active.png");
        checkBoxDisabled2->setPosition(Vec2(widgetSize.width / 2.0f + 70, widgetSize.height / 2.0f));
        checkBoxDisabled2->setBright(false);
        checkBoxDisabled2->setSelected(true);
		checkBoxDisabled2->setName("checkBoxDisabled2");

        _uiLayer->addChild(checkBoxDisabled2);
        return true;
    }
    return false;
}


// UICheckBoxCopyTest
 

bool UICheckBoxCopyTest::init()
{
    if (Layer::init())
    {
		this->setName("UICheckBoxCopyTest");

		Tests();NAV_BOTH(UICheckBoxDefaultBehaviorTest, UICheckBoxTest);

        Size widgetSize = Director::getInstance()->getWinSize();

        // Add a label in which the checkbox events will be displayed
        _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));
		_displayValueLabel->setName("_displayValueLabel");

        _uiLayer->addChild(_displayValueLabel);

        // Add the alert
        Text* alert = Text::create("Two checkbox are identical.","fonts/Marker Felt.ttf",20 );
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                widgetSize.height / 2.0f - alert->getContentSize().height * 1.75f));
		alert->setName("alert");

        _uiLayer->addChild(alert);

        // Create the checkbox
        CheckBox* checkBox = CheckBox::create("check_box_normal.png",
                                              "check_box_active.png");
        checkBox->setPosition(Vec2(widgetSize.width / 2.0f - 50, widgetSize.height / 2.0f));
		checkBox->setName("checkBox");
        _uiLayer->addChild(checkBox);

        auto checkboxCopy = checkBox->clone();
        checkboxCopy->setPosition(checkBox->getPosition() + Vec2(50.0f,0.0f));
		checkboxCopy->setName("checkboxCopy");

        _uiLayer->addChild(checkboxCopy);


        return true;
    }
    return false;
}
