 

#include "UIEditBoxTest.h"
 

using namespace orange::ui;

UIEditBoxTests::UIEditBoxTests()
{
    ADD_TEST_CASE(UIEditBoxTest);
    ADD_TEST_CASE(UIEditBoxTestToggleVisibility);
    ADD_TEST_CASE(UIEditBoxTestTextHorizontalAlignment);
    ADD_TEST_CASE(UIEditBoxTestPressedAndDisabled);
}

// UIEditBoxTest
bool UIEditBoxTest::init()
{
	if (MyLayer::init())
	{
		auto visibleSize = Director::getInstance()->getWinSize();

		// 1. 背景居中
		auto pBg = Sprite::create("Images/HelloWorld.png");
		pBg->setPosition(Vec2(visibleSize.width / 2.0f - pBg->getContentSize().width / 2.0f,
			visibleSize.height / 2.0f - pBg->getContentSize().height / 2.0f));
		addChild(pBg);

		// 2. 顶部状态文字
		_TTFShowEditReturn = Label::createWithSystemFont("No edit control return!", "Arial", 30);
		_TTFShowEditReturn->setPosition(Vec2(visibleSize.width / 2.0f - _TTFShowEditReturn->getContentSize().width / 2.0f, 20));
		addChild(_TTFShowEditReturn);

		// ★ 3. 把输入框宽度缩小，高度固定为 60，这样就不会像香肠了
		auto editBoxSize = Size(visibleSize.width * 0.45f, 60.0f);

		// ================== Top (Name) ==================
		std::string pNormalSprite = "extensions/green_edit.png";
		std::string pressSprite = "extensions/switch-mask.png";
		_editName = ui::EditBox::create(editBoxSize, ui::Scale9Sprite::create(pNormalSprite),
			ui::Scale9Sprite::create(pressSprite)
		//	,ui::Scale9Sprite::create("slidbar.png")
			);
		//_editName->setBright(false);  // 调暗/关闭高亮
		_editName->setEnabled(false); // 禁用控件，不可点击
		// X在左侧 15%，Y放在 120 高度
		_editName->setPosition(Vec2(visibleSize.width * 0.15f, 120));
		_editName->setFontColor(Color3B::RED);
		_editName->setPlaceHolder("Name:");
		_editName->setPlaceholderFontColor(Color3B::WHITE);
		_editName->setMaxLength(8);
		_editName->setFontSize(24); // 固定字号，避免太大
		_editName->setText("v123456测试");
		_editName->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		_editName->setDelegate(this);
		_editName->setVisible(true);
		addChild(_editName);

		Button* button = Button::create("animationbuttonnormal.png", "animationbuttonpressed.png");
		button->setTitleText("Single Line");
		// 按钮放在输入框右边，垂直居中
		button->setPosition(Vec2(_editName->getPositionX() + editBoxSize.width + 80.0f,
			_editName->getPositionY() + editBoxSize.height / 2.0f));
		button->addClickEventListener([=](Ref* ref) {
			_editName->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
		});
		addChild(button);

		// ================== Middle (Password) ==================
		// 不使用 9 宫格背景，直接普通贴图，免得被拉伸
		_editPassword = ui::EditBox::create(editBoxSize, "extensions/orange_edit.png");
		_editPassword->setPosition(Vec2(visibleSize.width * 0.15f, 250));
		_editPassword->setFontColor(Color3B::GREEN);
		_editPassword->setPlaceHolder("Password:");
		_editPassword->setMaxLength(6);
		_editPassword->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
		_editPassword->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
		_editPassword->setFontSize(24);
		_editPassword->setDelegate(this);
		_editPassword->setVisible(true);
		addChild(_editPassword);

		auto buttonPassword = (ui::Button*)button->clone();
		buttonPassword->setTitleText("Multiline");
		buttonPassword->setPosition(Vec2(_editPassword->getPositionX() + editBoxSize.width + 80.0f,
			_editPassword->getPositionY() + editBoxSize.height / 2.0f));
		buttonPassword->addClickEventListener([=](Ref* ref) {
			_editPassword->setInputMode(ui::EditBox::InputMode::ANY);
		});
		addChild(buttonPassword);

		// ================== Bottom (Email) ==================
		_editEmailParent = Node::create();
		_editEmailParent->setPosition(Vec2(visibleSize.width * 0.15f, 380));
		addChild(_editEmailParent);

		_editEmail = ui::EditBox::create(editBoxSize, "extensions/yellow_edit.png");
		_editEmail->setPosition(Vec2(0, 0));
		_editEmail->setPlaceHolder("Email:");
		_editEmail->setInputMode(ui::EditBox::InputMode::EMAIL_ADDRESS);
		_editEmail->setDelegate(this);
		_editEmail->setVisible(true);
		_editEmailParent->addChild(_editEmail);

		_editEmail->setFontSize(24);
		_editEmail->setContentSize(editBoxSize);

		auto buttonEmail = (ui::Button*)button->clone();
		buttonEmail->setTitleText("Multiline");
		button->setTitleText("Single Line");
		buttonEmail->setPosition(Vec2(_editEmail->getPositionX() + editBoxSize.width + 80.0f,
			_editEmail->getPositionY() + editBoxSize.height / 2.0f));
		buttonEmail->addClickEventListener([=](Ref* ref) {
			_editEmail->setInputMode(ui::EditBox::InputMode::ANY);
		});
		addChild(buttonEmail);

		return true;
	}
	return false;
}

void UIEditBoxTest::editBoxEditingDidBegin(orange::ui::EditBox* editBox)
{
    log("editBox %p DidBegin !", editBox);
}

void UIEditBoxTest::editBoxEditingDidEndWithAction(orange::ui::EditBox* editBox, orange::ui::EditBoxDelegate::EditBoxEndAction action)
{
    log("editBox %p DidEnd with action %d!", editBox, action);
}

void UIEditBoxTest::editBoxTextChanged(orange::ui::EditBox* editBox, const std::string& text)
{
    log("editBox %p TextChanged, text: %s ", editBox, text.c_str());
    editBox->setText(text.c_str());
}

void UIEditBoxTest::editBoxReturn(ui::EditBox* editBox)
{
    log("editBox %p was returned !",editBox);
    
    if (_editName == editBox)
    {
        _TTFShowEditReturn->setString("Name EditBox return !");
    }
    else if (_editPassword == editBox)
    {
        _TTFShowEditReturn->setString("Password EditBox return !");
    }
    else if (_editEmail == editBox)
    {
        _TTFShowEditReturn->setString("Email EditBox return !");
    }
}

// UIEditBoxTestToggleVisibility

bool UIEditBoxTestToggleVisibility::init()
{
	if (MyLayer::init())
	{
		auto visibleSize = Director::getInstance()->getWinSize();

		// 背景居中
		auto pBg = Sprite::create("Images/HelloWorld.png");
		pBg->setPosition(Vec2(visibleSize.width / 2.0f - pBg->getContentSize().width / 2.0f,
			visibleSize.height / 2.0f - pBg->getContentSize().height / 2.0f));
		addChild(pBg);

		// 顶部文字居中
		_TTFShowEditReturn = Label::createWithSystemFont("No edit control return!", "Arial", 30);
		_TTFShowEditReturn->setPosition(Vec2(visibleSize.width / 2.0f - _TTFShowEditReturn->getContentSize().width / 2.0f, 20));
		addChild(_TTFShowEditReturn);

		// 稍微缩小宽度，给右边的按钮留出空间
		auto editBoxSize = Size(visibleSize.width * 0.6f, visibleSize.height * 0.1f);

		// ================== Top (Name) ==================
		std::string pNormalSprite = "extensions/green_edit.png";
		_editName = ui::EditBox::create(editBoxSize + Size(0, 40), ui::Scale9Sprite::create(pNormalSprite));
		_editName->setPosition(Vec2(100, 120)); // 左上角坐标系：x=100, y=120
		_editName->setFontColor(Color3B::RED);
		_editName->setPlaceHolder("Name:");
		_editName->setPlaceholderFontColor(Color3B::WHITE);
		_editName->setMaxLength(8);
		_editName->setFontSize((int)editBoxSize.height / 2);
		_editName->setText("v👐👊💝");
		_editName->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		_editName->setDelegate(this);
		_editName->setVisible(true);
		addChild(_editName);

		Button* button = Button::create("animationbuttonnormal.png", "animationbuttonpressed.png");
		auto buttonSize = button->getContentSize();
		button->setTitleText("Toggle Visibility");
		// 按钮排在输入框右侧，垂直居中
		button->setPosition(Vec2(100 + editBoxSize.width + 20, 120 + (editBoxSize.height + 40) / 2.0f - buttonSize.height / 2.0f));
		button->addClickEventListener([=](Ref* ref) {
			_editName->setVisible(!_editName->isVisible());
		});
		addChild(button);

		// ================== Middle (Password) ==================
		_editPassword = ui::EditBox::create(editBoxSize, "extensions/orange_edit.png");
		_editPassword->setPosition(Vec2(100, 280));
		_editPassword->setFontColor(Color3B::GREEN);
		_editPassword->setPlaceHolder("Password:");
		_editPassword->setMaxLength(6);
		_editPassword->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
		_editPassword->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
		_editPassword->setFontSize((float)editBoxSize.height / 2);
		_editPassword->setDelegate(this);
		_editPassword->setVisible(true);
		addChild(_editPassword);

		auto buttonPassword = (ui::Button*)button->clone();
		buttonPassword->setTitleText("Toggle Visibility");
		buttonPassword->setPosition(Vec2(100 + editBoxSize.width + 20, 280 + editBoxSize.height / 2.0f - buttonSize.height / 2.0f));
		buttonPassword->addClickEventListener([=](Ref* ref) {
			_editPassword->setVisible(!_editPassword->isVisible());
		});
		addChild(buttonPassword);

		// ================== Bottom (Email) ==================
		_editEmailParent = Node::create();
		_editEmailParent->setPosition(Vec2(100, 450));
		addChild(_editEmailParent);

		auto bottomButtonSize = Size(editBoxSize.width, editBoxSize.height + 10);
		_editEmail = ui::EditBox::create(bottomButtonSize, "extensions/yellow_edit.png");
		_editEmail->setPosition(Vec2::ZERO); // 父节点在 100,450，内部从 0,0 开始
		_editEmail->setPlaceHolder("Email:");
		_editEmail->setInputMode(ui::EditBox::InputMode::EMAIL_ADDRESS);
		_editEmail->setDelegate(this);
		_editEmail->setVisible(true);
		_editEmailParent->addChild(_editEmail);

		_editEmail->setFontSize(30);
		_editEmail->setContentSize(bottomButtonSize);

		auto buttonEmail = (ui::Button*)button->clone();
		buttonEmail->setTitleText("Toggle Visibility");
		buttonEmail->setPosition(Vec2(editBoxSize.width + 20, (editBoxSize.height + 10) / 2.0f - buttonSize.height / 2.0f));
		buttonEmail->addClickEventListener([=](Ref* ref) {
			_editEmail->setVisible(!_editEmail->isVisible());
		});
		addChild(buttonEmail);

		return true;
	}
	return false;
}

void UIEditBoxTestToggleVisibility::editBoxEditingDidBegin(orange::ui::EditBox* editBox)
{
    log("editBox %p DidBegin !", editBox);
}

void UIEditBoxTestToggleVisibility::editBoxTextChanged(orange::ui::EditBox* editBox, const std::string& text)
{
    log("editBox %p TextChanged, text: %s ", editBox, text.c_str());
}

void UIEditBoxTestToggleVisibility::editBoxReturn(ui::EditBox* editBox)
{
    log("editBox %p was returned !",editBox);
    
    if (_editName == editBox)
    {
        _TTFShowEditReturn->setString("Name EditBox return !");
    }
    else if (_editPassword == editBox)
    {
        _TTFShowEditReturn->setString("Password EditBox return !");
    }
    else if (_editEmail == editBox)
    {
        _TTFShowEditReturn->setString("Email EditBox return !");
    }
}

// UIEditBoxTestTextHorizontalAlignment

bool UIEditBoxTestTextHorizontalAlignment::init() {
	if (!MyLayer::init()) {
		return false;
	}

	auto visibleSize = Director::getInstance()->getWinSize();
	const auto editBoxSize = Size(visibleSize.width * 0.6f, visibleSize.height * 0.1f);

	// 修改了捕获列表，使用左上角起点 X = 100
	const auto createEditBox = [this, editBoxSize](const std::string& text, const TextHAlignment alignment, const int position_y) {
		ui::EditBox* editbox = ui::EditBox::create(editBoxSize + Size(0, 40), ui::Scale9Sprite::create("extensions/green_edit.png"));
		editbox->setPosition(Vec2(100, (float)position_y)); // X 固定在 100，Y 传入
		editbox->setFontColor(Color3B::RED);
		editbox->setPlaceHolder(text.c_str());
		editbox->setPlaceholderFontColor(Color3B::WHITE);
		editbox->setPlaceholderFontSize((int)editBoxSize.height / 2);
		editbox->setFontSize((int)editBoxSize.height / 2);
		editbox->setText(text.c_str());
		editbox->setTextHorizontalAlignment(alignment);
		editbox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		addChild(editbox);
	};

	// 直接写死 Y 坐标：120, 280, 440
	createEditBox("horizontal left text", TextHAlignment::LEFT, 120);
	createEditBox("horizontal center text", TextHAlignment::CENTER, 280);
	createEditBox("horizontal right text", TextHAlignment::RIGHT, 440);

	return true;
}

// UIEditBoxTestPressedAndDisabled
bool UIEditBoxTestPressedAndDisabled::init() {
	if (!MyLayer::init()) {
		return false;
	}

	auto visibleSize = Director::getInstance()->getWinSize();
	const auto editBoxSize = Size(visibleSize.width * 0.6f, visibleSize.height * 0.1f);

	// 第一个：正常与按下
	ui::EditBox* editbox = ui::EditBox::create(editBoxSize + Size(0, 40), "extensions/yellow_edit.png", "extensions/orange_edit.png");
	editbox->setPosition(Vec2(100, 120)); // X=100, Y=120
	editbox->setFontColor(Color3B::GREEN);
	editbox->setPlaceHolder("Test Pressed Image");
	editbox->setPlaceholderFontColor(Color3B::WHITE);
	editbox->setPlaceholderFontSize((int)editBoxSize.height / 2);
	editbox->setFontSize((int)editBoxSize.height / 2);
	editbox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	addChild(editbox);

	// 第二个：禁用带图
	editbox = ui::EditBox::create(editBoxSize + Size(0, 40), "extensions/yellow_edit.png", "extensions/orange_edit.png", "extensions/green_edit.png");
	editbox->setPosition(Vec2(100, 280));
	editbox->setFontColor(Color3B::GREEN);
	editbox->setPlaceHolder("Test Disabled Image - green");
	editbox->setPlaceholderFontColor(Color3B::WHITE);
	editbox->setPlaceholderFontSize((int)editBoxSize.height / 2);
	editbox->setFontSize((int)editBoxSize.height / 2);
	editbox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	addChild(editbox);
	editbox->setEnabled(false);

	// 第三个：禁用变灰
	editbox = ui::EditBox::create(editBoxSize + Size(0, 40), "extensions/yellow_edit.png");
	editbox->setPosition(Vec2(100, 440));
	editbox->setFontColor(Color3B::GREEN);
	editbox->setPlaceHolder("Disabled Gray effect");
	editbox->setPlaceholderFontColor(Color3B::WHITE);
	editbox->setPlaceholderFontSize((int)editBoxSize.height / 2);
	editbox->setFontSize((int)editBoxSize.height / 2);
	editbox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	addChild(editbox);
	editbox->setEnabled(false);

	return true;
}
