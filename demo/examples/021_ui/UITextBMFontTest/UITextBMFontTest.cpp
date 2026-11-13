
#include "UITextBMFontTest.h" 

using namespace orange::ui;
 
UITextBMFontTests::UITextBMFontTests()
{
	ADD_TEST_CASE(UITextBMFontTest);

}
// UITextBMFontTest

bool UITextBMFontTest::init()
{
    if (MyLayer::init())
    {
        Size widgetSize = Director::getInstance()->getWinSize();
        
        Text* alert = Text::create("TextBMFont","TextBMFont",50);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 10.75f));
        _uiLayer->addChild(alert);
        
        // Create the TextBMFont
        TextBMFont* textBMFont = TextBMFont::create("BMFont", "bitmapFontTest2.fnt");
        textBMFont->setPosition(Vec2(
			(widgetSize - textBMFont->getContentSize())/2.0f
		));
        _uiLayer->addChild(textBMFont);
        
        _textBMFont = textBMFont;

        TTFConfig ttfConfig("fonts/arial.ttf", 15);
        auto label1 = Label::createWithTTF(ttfConfig, "Print Resources");
        auto item1 = MenuItemLabel::create(label1, OG_CALLBACK_1(UITextBMFontTest::printWidgetResources, this));
        item1->setPosition(Vec2( 60,   item1->getContentSize().height * 3));
        auto pMenu1 = Menu::create(item1, nullptr);
        pMenu1->setPosition(Vec2(0, 0));
        this->addChild(pMenu1, 10);

        return true;
    }
    return false;
}
void UITextBMFontTest::printWidgetResources(orange::Ref* sender)
{
    orange::ResourceData textureFile = _textBMFont->getRenderFile();
    OGLOG("textureFile  Name : %s, Type: %d", textureFile.file.c_str(), textureFile.type);
}
