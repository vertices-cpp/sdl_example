

#include "ui/UITabControl.h"
#include "UITabControlTest.h"

USING_OG;
using namespace orange::ui;
 
UITabControlTests::UITabControlTests()
{
	ADD_TEST_CASE(UITabControlTest);
}

UITabControlTest::UITabControlTest()
{
}

UITabControlTest::~UITabControlTest()
{

}

bool UITabControlTest::init()
{

    if (MyLayer::init())
    {
 

        Size widgetSize = Director::getInstance()->getWinSize();

        auto displayText = Text::create();
        displayText->setPosition( Vec2(0, 50));
        _uiLayer->addChild(displayText);

        auto tab = TabControl::create();
        tab->setContentSize(Size(200.f, 200.f));
        tab->setHeaderHeight(20.f);
        tab->setHeaderWidth(70.f);
        tab->setHeaderSelectedZoom(.1f);
        tab->setHeaderDockPlace(TabControl::Dock::TOP);

        auto header1 = TabHeader::create();
        header1->setTitleText("background");
        header1->loadTextureBackGround("check_box_normal_disable.png");
        auto header2 = TabHeader::create("cross", "check_box_normal_disable.png", "check_box_active.png");
        auto header3 = TabHeader::create("press&cross", "check_box_normal.png",
            "check_box_normal_press.png",
            "check_box_active.png",
            "check_box_normal_disable.png",
            "check_box_active_disable.png");

        auto container1 = Layout::create();
        container1->setOpacity(255);
        container1->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
        container1->setBackGroundColor(Color3B::GRAY);
        container1->setBackGroundColorOpacity(255);
        auto container2 = Layout::create();
        container2->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
        container2->setOpacity(255);
        container2->setBackGroundColor(Color3B::BLUE);
        container2->setBackGroundColorOpacity(255);
        auto container3 = Layout::create();
        container3->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
        container3->setOpacity(255);
        container3->setBackGroundColor(Color3B::RED);
        container3->setBackGroundColorOpacity(255);

        tab->insertTab(0, header1, container1);
        tab->insertTab(1, header2, container2);
        tab->insertTab(2, header3, container3);

        tab->setSelectTab(2);
        
        _uiLayer->addChild(tab);
        tab->setPosition(Vec2(widgetSize.width * .5f, widgetSize.height * .5f));

        tab->setTabChangedEventListener(
            [displayText](int index, TabControl::EventType evtType)
        {
            displayText->retain();
            char display[20];
            sprintf(display, "tab %d selected", index);
            displayText->setString(display);
            displayText->release();
        }
            );

        return true;
    }
    return false;
}

