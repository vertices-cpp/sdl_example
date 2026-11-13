

#include "UIScrollViewTest.h"

#include "TestNavigation.h"
#include "base/OGRandom.h"

using namespace orange::ui;
 

UIScrollViewTests::UIScrollViewTests()
{
	ADD_TEST_CASE(UIScrollViewTest_Horizontal);
	ADD_TEST_CASE(UIScrollViewTest_Vertical);
	ADD_TEST_CASE(UIScrollViewTest_Both);
	ADD_TEST_CASE(UIScrollViewTest_ScrollToPercentBothDirection);
	ADD_TEST_CASE(UIScrollViewTest_ScrollToPercentBothDirection_Bounce);
	ADD_TEST_CASE(UIScrollViewNestTest);
	ADD_TEST_CASE(UIScrollViewRotated);
	ADD_TEST_CASE(UIScrollViewDisableTest);
	ADD_TEST_CASE(UIScrollViewInnerSize);
	ADD_TEST_CASE(UIScrollViewTestEvents);
	ADD_TEST_CASE(UIScrollViewStopScrollingTest);
}

// UIScrollViewTest_Vertical

UIScrollViewTest_Vertical::UIScrollViewTest_Vertical()
: _displayValueLabel(nullptr)
{
}

UIScrollViewTest_Vertical::~UIScrollViewTest_Vertical()
{
}
bool UIScrollViewTest_Vertical::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewStopScrollingTest, UIScrollViewTest_Horizontal);

		Size widgetSize = Director::getInstance()->getWinSize();

		// 标签居中
		_displayValueLabel = Text::create("Move by vertical direction", "fonts/Marker Felt.ttf", 32);
		_displayValueLabel->setPosition(Vec2((widgetSize.width - _displayValueLabel->getContentSize().width) / 2.0f, 50));
		_uiLayer->addChild(_displayValueLabel);

		Text* alert = Text::create("ScrollView vertical", "fonts/Marker Felt.ttf", 30);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2((widgetSize.width - alert->getContentSize().width) / 2.0f, 100));
		_uiLayer->addChild(alert);

		// ScrollView 绝对居中
		ui::ScrollView* scrollView = ui::ScrollView::create();
		scrollView->setContentSize(Size(280.0f, 100.0f));
		scrollView->setPosition(Vec2((widgetSize.width - scrollView->getContentSize().width) / 2.0f,
			(widgetSize.height - scrollView->getContentSize().height) / 2.0f));
		scrollView->setScrollBarWidth(4);
		scrollView->setScrollBarPositionFromCorner(Vec2(2, 2));
		scrollView->setScrollBarColor(Color3B::WHITE);
		scrollView->setGlobalZOrder(200);
		_uiLayer->addChild(scrollView);

		// =========================================================
		// ★ 核心修复：严格按原版从上往下的视觉顺序堆叠！
		// =========================================================
		float innerWidth = scrollView->getContentSize().width;

		Button* button = Button::create("animationbuttonnormal.png", "animationbuttonpressed.png");
		Button* titleButton = Button::create("backtotopnormal.png", "backtotoppressed.png");
		titleButton->setTitleText("Title Button");
		Button* button_scale9 = Button::create("button.png", "buttonHighlighted.png");
		button_scale9->setScale9Enabled(true);
		button_scale9->setContentSize(Size(100.0f, button_scale9->getVirtualRendererSize().height));
		ImageView* imageView = ImageView::create("ccicon.png");

		// 1. 第一个 (最上面)：button
		button->setPosition(Vec2((innerWidth - button->getContentSize().width) / 2.0f, 0));
		scrollView->addChild(button);

		// 2. 第二个：titleButton
		float y2 = button->getContentSize().height;
		titleButton->setPosition(Vec2((innerWidth - titleButton->getContentSize().width) / 2.0f, y2));
		scrollView->addChild(titleButton);

		// 3. 第三个：button_scale9
		float y3 = y2 + titleButton->getContentSize().height;
		button_scale9->setPosition(Vec2((innerWidth - button_scale9->getContentSize().width) / 2.0f, y3));
		scrollView->addChild(button_scale9);

		// 4. 第四个 (最下面)：imageView
		float y4 = y3 + button_scale9->getContentSize().height;
		imageView->setPosition(Vec2((innerWidth - imageView->getContentSize().width) / 2.0f, y4));
		scrollView->addChild(imageView);

		// 5. 计算总高度，刚好包住所有元素，不多留一点空隙
		float innerHeight = y4 + imageView->getContentSize().height;
		scrollView->setInnerContainerSize(Size(innerWidth, innerHeight));

		// 滚到底部
		scrollView->scrollToBottom(2.0, true);
		return true;
	}

	return false;
}

// UIScrollViewTest_Horizontal

UIScrollViewTest_Horizontal::UIScrollViewTest_Horizontal()
: _displayValueLabel(nullptr)
{
}

UIScrollViewTest_Horizontal::~UIScrollViewTest_Horizontal()
{
}

bool UIScrollViewTest_Horizontal::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewTest_Vertical, UIScrollViewTest_Both);

		Size widgetSize = Director::getInstance()->getWinSize();

		// Add a label in which the scrollview alert will be displayed
		_displayValueLabel = Text::create("Move by horizontal direction", "fonts/Marker Felt.ttf", 32);
		// ★ 修复：取消锚点，用绝对居中
		_displayValueLabel->setPosition(Vec2((widgetSize.width - _displayValueLabel->getContentSize().width) / 2.0f,
			widgetSize.height / 2.0f + _displayValueLabel->getContentSize().height * 2.5f));
		_uiLayer->addChild(_displayValueLabel);

		Text* alert = Text::create("ScrollView horizontal", "fonts/Marker Felt.ttf", 30);
		alert->setColor(Color3B(159, 168, 176));
		// ★ 修复：居中
		alert->setPosition(Vec2((widgetSize.width - alert->getContentSize().width) / 2.0f,
			widgetSize.height / 2.0f - alert->getContentSize().height * 3.075f));
		_uiLayer->addChild(alert);

		// Create the scrollview by horizontal
				// Create the scrollview by horizontal
		ui::ScrollView* scrollView = ui::ScrollView::create();
		scrollView->setBounceEnabled(true);
		scrollView->setDirection(ui::ScrollView::Direction::HORIZONTAL);
		scrollView->setContentSize(Size(280.0f, 150.0f));
		scrollView->setScrollBarPositionFromCorner(Vec2(4, 4));
		scrollView->setScrollBarColor(Color3B::YELLOW);

		// ★ 绝对居中
		scrollView->setPosition(Vec2((widgetSize.width - scrollView->getContentSize().width) / 2.0f,
			(widgetSize.height - scrollView->getContentSize().height) / 2.0f));
		_uiLayer->addChild(scrollView);

		ImageView* imageView = ImageView::create("ccicon.png");
		float innerHeight = scrollView->getContentSize().height;

		// =========================================================
		// ★ 核心修复：动态流水线排版，算好每个元素的位置和最终容器宽
		// =========================================================
		float currentX = 10.0f; // 初始左边距 10 像素
		float spacing = 20.0f;  // 元素之间的间距 20 像素

		// 1. 普通 Button
		Button* button = Button::create("animationbuttonnormal.png", "animationbuttonpressed.png");
		button->setPosition(Vec2(currentX, (innerHeight - button->getContentSize().height) / 2.0f));
		scrollView->addChild(button);
		currentX += button->getContentSize().width + spacing; // 累加：自身宽度 + 间距

		// 2. Title Button
		Button* titleButton = Button::create("backtotopnormal.png", "backtotoppressed.png");
		titleButton->setTitleText("Title Button");
		titleButton->setPosition(Vec2(currentX, (innerHeight - titleButton->getContentSize().height) / 2.0f));
		scrollView->addChild(titleButton);
		currentX += titleButton->getContentSize().width + spacing;

		// 3. Scale9 Button
		Button* button_scale9 = Button::create("button.png", "buttonHighlighted.png");
		button_scale9->setScale9Enabled(true);
		button_scale9->setContentSize(Size(100.0f, button_scale9->getVirtualRendererSize().height));
		button_scale9->setPosition(Vec2(currentX, (innerHeight - button_scale9->getContentSize().height) / 2.0f));
		scrollView->addChild(button_scale9);
		currentX += button_scale9->getContentSize().width + spacing;

		// 4. ImageView
		imageView->setPosition(Vec2(currentX, (innerHeight - imageView->getContentSize().height) / 2.0f));
		scrollView->addChild(imageView);
		currentX += imageView->getContentSize().width + 20.0f; // 最后留 20 像素的右边距

		// ★ 终极设置：容器宽度就是流水线的最终长度！
		scrollView->setInnerContainerSize(Size(currentX, innerHeight));

		scrollView->jumpToRight();

		return true;
	}
	return false;
}

// UIScrollViewTest_Both
UIScrollViewTest_Both::UIScrollViewTest_Both()
: _displayValueLabel(nullptr)
{
    
}

UIScrollViewTest_Both::~UIScrollViewTest_Both()
{
}

bool UIScrollViewTest_Both::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewTest_Horizontal, UIScrollViewTest_ScrollToPercentBothDirection);

		Size widgetSize = Director::getInstance()->getWinSize();

		_displayValueLabel = Text::create("Move by any direction", "fonts/Marker Felt.ttf", 32);
		// ★ 修复：绝对居中
		_displayValueLabel->setPosition(Vec2((widgetSize.width - _displayValueLabel->getContentSize().width) / 2.0f,
			widgetSize.height / 2.0f + _displayValueLabel->getContentSize().height * 1.5f));
		_uiLayer->addChild(_displayValueLabel);

		Text* alert = Text::create("ScrollView both", "fonts/Marker Felt.ttf", 30);
		alert->setColor(Color3B(159, 168, 176));
		// ★ 修复：绝对居中
		alert->setPosition(Vec2((widgetSize.width - alert->getContentSize().width) / 2.0f,
			widgetSize.height / 2.0f - alert->getContentSize().height * 3.075f));
		_uiLayer->addChild(alert);

		ui::ScrollView* scrollView = ui::ScrollView::create();
		scrollView->setDirection(ui::ScrollView::Direction::BOTH);
		scrollView->setTouchEnabled(true);
		scrollView->setBounceEnabled(true);
		scrollView->setBackGroundImageScale9Enabled(true);
		scrollView->setBackGroundImage("green_edit.png");
		scrollView->setContentSize(Size(210, 122.5));
		scrollView->setScrollBarWidth(4);
		scrollView->setScrollBarPositionFromCorner(Vec2(6, 6));

		// ★ 修复：绝对居中
		scrollView->setPosition(Vec2((widgetSize.width - scrollView->getContentSize().width) / 2.0f,
			(widgetSize.height - scrollView->getContentSize().height) / 2.0f));

		ImageView* imageView = ImageView::create("Hello.png");

		scrollView->setInnerContainerSize(imageView->getContentSize());
		// ★ 修复：图片左上角对齐 (0,0) 即可，因为容器大小就是图片大小
		imageView->setPosition(Vec2::ZERO);

		scrollView->addChild(imageView);
		_uiLayer->addChild(scrollView);

		scrollView->jumpToBottomRight();

		return true;
	}
	return false;
}

// UIScrollViewTest_ScrollToPercentBothDirection
UIScrollViewTest_ScrollToPercentBothDirection::UIScrollViewTest_ScrollToPercentBothDirection()
: _displayValueLabel(nullptr)
{
    
}

UIScrollViewTest_ScrollToPercentBothDirection::~UIScrollViewTest_ScrollToPercentBothDirection()
{
}

bool UIScrollViewTest_ScrollToPercentBothDirection::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewTest_Both, UIScrollViewTest_ScrollToPercentBothDirection_Bounce);

		Size widgetSize = Director::getInstance()->getWinSize();

		_displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 30);
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f + 50));
		_uiLayer->addChild(_displayValueLabel);

		Text* alert = Text::create("ScrollView scroll to percent both direction without scroll bar", "fonts/Marker Felt.ttf", 20);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - 80));
		_uiLayer->addChild(alert);

		ui::ScrollView* sc = ui::ScrollView::create();
		sc->setBackGroundColor(Color3B::GREEN);
		sc->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
		sc->setDirection(ui::ScrollView::Direction::BOTH);
		sc->setInnerContainerSize(Size(480.0f, 320.0f));
		sc->setContentSize(Size(100.0f, 100.0f));
		sc->setScrollBarEnabled(false);

		// ★ 唯一改动 1：无锚点居中
		sc->setPosition(Vec2((widgetSize.width - sc->getContentSize().width) / 2.0f,
			(widgetSize.height - sc->getContentSize().height) / 2.0f));

		// 保留官方的自动滚动
		sc->scrollToPercentBothDirection(Vec2(50, 50), 1, true);

		ImageView* iv = ImageView::create("Hello.png");
		// ★ 唯一改动 2：原版是居中锚点，无锚点下必须放到左上角 (0,0)，否则图片跑出 480x320 的容器
		iv->setPosition(Vec2::ZERO);
		sc->addChild(iv);
		_uiLayer->addChild(sc);

		return true;
	}
	return false;
}

// UIScrollViewTest_ScrollToPercentBothDirection_Bounce
UIScrollViewTest_ScrollToPercentBothDirection_Bounce::UIScrollViewTest_ScrollToPercentBothDirection_Bounce()
: _displayValueLabel(nullptr)
{
    
}

UIScrollViewTest_ScrollToPercentBothDirection_Bounce::~UIScrollViewTest_ScrollToPercentBothDirection_Bounce()
{
}

bool UIScrollViewTest_ScrollToPercentBothDirection_Bounce::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewTest_ScrollToPercentBothDirection, UIScrollViewNestTest);

		Size widgetSize = Director::getInstance()->getWinSize();

		_displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f + 50));
		_uiLayer->addChild(_displayValueLabel);

		Text* alert = Text::create("ScrollView scroll to percent both direction bounce", "fonts/Marker Felt.ttf", 20);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - 80));
		_uiLayer->addChild(alert);

		ui::ScrollView* sc = ui::ScrollView::create();
		sc->setBackGroundColor(Color3B::GREEN);
		sc->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
		sc->setBounceEnabled(true);
		sc->setDirection(ui::ScrollView::Direction::BOTH);
		sc->setInnerContainerSize(Size(480.0f, 320.0f));
		sc->setContentSize(Size(100.0f, 100.0f));

		// 官方的滚动条位置（保持原样）
		sc->setScrollBarPositionFromCornerForHorizontal(Vec2(5.0f, sc->getContentSize().height - 5));
		sc->setScrollBarPositionFromCornerForVertical(Vec2(sc->getContentSize().width - 5, 5.0f));
		sc->setScrollBarAutoHideEnabled(false);

		// ★ 唯一改动 1：无锚点居中
		sc->setPosition(Vec2((widgetSize.width - sc->getContentSize().width) / 2.0f,
			(widgetSize.height - sc->getContentSize().height) / 2.0f));

		// 保留官方的自动滚动
		sc->scrollToPercentBothDirection(Vec2(50.0f, 50.0f), 1, true);

		ImageView* iv = ImageView::create("Hello.png");
		// ★ 唯一改动 2：放到左上角
		iv->setPosition(Vec2::ZERO);
		sc->addChild(iv);
		_uiLayer->addChild(sc);
		return true;
	}

	return false;
}

// UIScrollViewNestTest

UIScrollViewNestTest::UIScrollViewNestTest()
: _displayValueLabel(nullptr)
{
}

UIScrollViewNestTest::~UIScrollViewNestTest()
{
}

bool UIScrollViewNestTest::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewTest_ScrollToPercentBothDirection_Bounce, UIScrollViewRotated);

		Size widgetSize = Director::getInstance()->getWinSize();

		// Add a label in which the scrollview alert will be displayed
		_displayValueLabel = Text::create("Move by vertical direction", "fonts/Marker Felt.ttf", 32);
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, 50)); // 无锚点居中修正
		_uiLayer->addChild(_displayValueLabel);

		// Add the alert
		Text* alert = Text::create("ScrollView vertical", "fonts/Marker Felt.ttf", 30);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2(widgetSize.width / 2.0f, 100)); // 无锚点居中修正
		_uiLayer->addChild(alert);

		// Create the scrollview by vertical
		ui::ScrollView* scrollView = ui::ScrollView::create();
		scrollView->setContentSize(Size(280.0f, 150.0f));
		scrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
		scrollView->setScrollBarPositionFromCorner(Vec2(4, 4));

		// ★ 唯一改动 1：无锚点居中
		scrollView->setPosition(Vec2((widgetSize.width - scrollView->getContentSize().width) / 2.0f,
			(widgetSize.height - scrollView->getContentSize().height) / 2.0f));

		scrollView->setScrollBarPositionFromCornerForVertical(Vec2(scrollView->getContentSize().width - 4, 4.0f));
		scrollView->setScrollBarColor(Color3B::BLUE);
		_uiLayer->addChild(scrollView);

		ImageView* imageView = ImageView::create("ccicon.png");

		float innerWidth = scrollView->getContentSize().width;
		float innerHeight = scrollView->getContentSize().height + imageView->getContentSize().height;

		scrollView->setInnerContainerSize(Size(innerWidth, innerHeight));

		// ★ 唯一改动 2：按左上角坐标系从上往下排，不用 getBottomBoundary 倒推
		Button* button = Button::create("animationbuttonnormal.png", "animationbuttonpressed.png");
		button->setPosition(Vec2((innerWidth - button->getContentSize().width) / 2.0f, 0)); // 贴顶
		scrollView->addChild(button);

		Button* titleButton = Button::create("backtotopnormal.png", "backtotoppressed.png");
		titleButton->setTitleText("Title Button");
		titleButton->setPosition(Vec2((innerWidth - titleButton->getContentSize().width) / 2.0f,
			button->getContentSize().height)); // 在 button 下面
		scrollView->addChild(titleButton);

		Button* button_scale9 = Button::create("button.png", "buttonHighlighted.png");
		button_scale9->setScale9Enabled(true);
		button_scale9->setContentSize(Size(100.0f, button_scale9->getVirtualRendererSize().height));
		button_scale9->setPosition(Vec2((innerWidth - button_scale9->getContentSize().width) / 2.0f,
			button->getContentSize().height + titleButton->getContentSize().height)); // 在 titleButton 下面
		scrollView->addChild(button_scale9);

		// ★ 图片在最底下
		imageView->setPosition(Vec2((innerWidth - imageView->getContentSize().width) / 2.0f,
			innerHeight - imageView->getContentSize().height));
		scrollView->addChild(imageView);


		// Create the scrollview by both
		ui::ScrollView* sc = ui::ScrollView::create();
		sc->setBackGroundColor(Color3B::GREEN);
		sc->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
		sc->setBounceEnabled(true);
		sc->setDirection(ui::ScrollView::Direction::BOTH);
		sc->setInnerContainerSize(Size(480.0f, 320.0f));
		sc->setContentSize(Size(100.0f, 100.0f));
		sc->setPropagateTouchEvents(false);

		// ★ 唯一改动 3：原版 (180,100) 在 280x150 的框里会超出右下角，稍微往左上挪一点，让它能完整显示在内部
		sc->setPosition(Vec2(20.0f, 20.0f));

		sc->scrollToPercentBothDirection(Vec2(50.0f, 50.0f), 1, true);
		sc->setScrollBarPositionFromCorner(Vec2(4.0f, 4.0f));

		ImageView* iv = ImageView::create("Hello.png");
		iv->setPosition(Vec2::ZERO); // ★ 放在内部容器的左上角
		sc->addChild(iv);

		scrollView->addChild(sc);

		return true;
	}

	return false;
}




// UIScrollViewRotated

UIScrollViewRotated::UIScrollViewRotated()
: _displayValueLabel(nullptr)
{
}

UIScrollViewRotated::~UIScrollViewRotated()
{
}
bool UIScrollViewRotated::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewNestTest, UIScrollViewDisableTest);

		Size widgetSize = Director::getInstance()->getWinSize();

		// ... 前面的 label 保持不变 ...

		// Create the scrollview by vertical
		ui::ScrollView* scrollView = ui::ScrollView::create();
		scrollView->setContentSize(Size(280.0f, 150.0f));
		scrollView->setDirection(ui::ScrollView::Direction::VERTICAL);

		// ★ 核心修复 1：往左上角移动，因为 45 度旋转会向右下延伸
		// 放在屏幕左侧三分之一、上方三分之一处，斜向拉伸刚好铺满屏幕
		scrollView->setPosition(Vec2(widgetSize.width / 4.0f, widgetSize.height / 6.0f));
		scrollView->setRotation(45);

		// ★ 核心修复 2：关闭裁剪（SDL 不支持斜切）
		scrollView->setClippingEnabled(false);

		scrollView->setScrollBarPositionFromCorner(Vec2(4, 4));
		scrollView->setScrollBarColor(Color3B::WHITE);
		_uiLayer->addChild(scrollView);

		ImageView* imageView = ImageView::create("ccicon.png");

		float innerWidth = scrollView->getContentSize().width;
		float innerHeight = scrollView->getContentSize().height + imageView->getContentSize().height;

		scrollView->setInnerContainerSize(Size(innerWidth, innerHeight));

		// ★ 核心修复 3：按无锚点从上往下排（不要再管 getBottomBoundary 了）
		Button* button = Button::create("animationbuttonnormal.png", "animationbuttonpressed.png");
		button->setPosition(Vec2((innerWidth - button->getContentSize().width) / 2.0f, 0)); // 贴顶
		scrollView->addChild(button);

		Button* titleButton = Button::create("backtotopnormal.png", "backtotoppressed.png");
		titleButton->setTitleText("Title Button");
		titleButton->setPosition(Vec2((innerWidth - titleButton->getContentSize().width) / 2.0f,
			button->getContentSize().height)); // 在 button 下面
		scrollView->addChild(titleButton);

		Button* button_scale9 = Button::create("button.png", "buttonHighlighted.png");
		button_scale9->setScale9Enabled(true);
		button_scale9->setContentSize(Size(100.0f, button_scale9->getVirtualRendererSize().height));
		button_scale9->setPosition(Vec2((innerWidth - button_scale9->getContentSize().width) / 2.0f,
			button->getContentSize().height + titleButton->getContentSize().height)); // 在 titleButton 下面
		scrollView->addChild(button_scale9);

		imageView->setPosition(Vec2((innerWidth - imageView->getContentSize().width) / 2.0f,
			innerHeight - imageView->getContentSize().height)); // 图片在最底部
		scrollView->addChild(imageView);

		return true;
	}

	return false;
}

// UIScrollViewDisableTest

UIScrollViewDisableTest::UIScrollViewDisableTest()
: _displayValueLabel(nullptr)
{
}

UIScrollViewDisableTest::~UIScrollViewDisableTest()
{
}
bool UIScrollViewDisableTest::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewRotated, UIScrollViewInnerSize);

		Size widgetSize = Director::getInstance()->getWinSize();

		// 标签居中
		_displayValueLabel = Text::create("ScrollView Disable Test", "fonts/Marker Felt.ttf", 32);
		_displayValueLabel->setPosition(Vec2((widgetSize.width - _displayValueLabel->getContentSize().width) / 2.0f, 50));
		_uiLayer->addChild(_displayValueLabel);

		Text* alert = Text::create("ScrollView vertical", "fonts/Marker Felt.ttf", 30);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2((widgetSize.width - alert->getContentSize().width) / 2.0f, 100));
		_uiLayer->addChild(alert);

		// ScrollView 居中
		ui::ScrollView* scrollView = ui::ScrollView::create();
		scrollView->setContentSize(Size(280.0f, 100.0f));
		scrollView->setPosition(Vec2((widgetSize.width - scrollView->getContentSize().width) / 2.0f,
			(widgetSize.height - scrollView->getContentSize().height) / 2.0f));

		scrollView->setScrollBarWidth(4);
		scrollView->setTouchEnabled(false); // 禁用触摸
		scrollView->setScrollBarPositionFromCorner(Vec2(2, 2));
		scrollView->setScrollBarColor(Color3B::WHITE);
		_uiLayer->addChild(scrollView);

		// =========================================================
		// ★ 核心修复：按无锚点从上往下排列所有元素
		// =========================================================
		float innerWidth = scrollView->getContentSize().width;

		Button* button = Button::create("animationbuttonnormal.png", "animationbuttonpressed.png");
		Button* titleButton = Button::create("backtotopnormal.png", "backtotoppressed.png");
		titleButton->setTitleText("Title Button");
		Button* button_scale9 = Button::create("button.png", "buttonHighlighted.png");
		button_scale9->setScale9Enabled(true);
		button_scale9->setContentSize(Size(100.0f, button_scale9->getVirtualRendererSize().height));
		ImageView* imageView = ImageView::create("ccicon.png");

		// 1. Button (最上面)
		button->setPosition(Vec2((innerWidth - button->getContentSize().width) / 2.0f, 0));
		scrollView->addChild(button);

		// 2. TitleButton
		float y2 = button->getContentSize().height;
		titleButton->setPosition(Vec2((innerWidth - titleButton->getContentSize().width) / 2.0f, y2));
		scrollView->addChild(titleButton);

		// 3. Scale9 Button
		float y3 = y2 + titleButton->getContentSize().height;
		button_scale9->setPosition(Vec2((innerWidth - button_scale9->getContentSize().width) / 2.0f, y3));
		scrollView->addChild(button_scale9);

		// 4. ImageView (最下面)
		float y4 = y3 + button_scale9->getContentSize().height;
		imageView->setPosition(Vec2((innerWidth - imageView->getContentSize().width) / 2.0f, y4));
		scrollView->addChild(imageView);

		// 5. 算准总高度，保证能刚好包住图片
		float innerHeight = y4 + imageView->getContentSize().height;
		scrollView->setInnerContainerSize(Size(innerWidth, innerHeight));

		return true;
	}

	return false;
}

// UIScrollViewInnerSize

UIScrollViewInnerSize::UIScrollViewInnerSize()
    : _displayValueLabel(nullptr)
{
}

UIScrollViewInnerSize::~UIScrollViewInnerSize()
{
}
bool UIScrollViewInnerSize::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewDisableTest, UIScrollViewTestEvents);

		Size widgetSize = Director::getInstance()->getWinSize();

		// Add a label in which the scrollview alert will be displayed
		_displayValueLabel = Text::create("ScrollView InnerSize Test", "fonts/Marker Felt.ttf", 32);
		// ★ 修复：无锚点居中
		_displayValueLabel->setPosition(Vec2((widgetSize.width - _displayValueLabel->getContentSize().width) / 2.0f,
			50));
		_uiLayer->addChild(_displayValueLabel);

		// Add the alert
		Text* alert = Text::create("ScrollView & InnerContainer should be align to the left and top point", "fonts/Marker Felt.ttf", 10);
		alert->setColor(Color3B(159, 168, 176));
		// ★ 修复：无锚点居中
		alert->setPosition(Vec2((widgetSize.width - alert->getContentSize().width) / 2.0f,
			100));
		_uiLayer->addChild(alert);

		// Create the scrollview by vertical
		ui::ScrollView* scrollView = ui::ScrollView::create();
		scrollView->setContentSize(Size(200.0f, 50.0f));

		// ★ 修复：无锚点绝对居中
		scrollView->setPosition(Vec2((widgetSize.width - scrollView->getContentSize().width) / 2.0f,
			(widgetSize.height - scrollView->getContentSize().height) / 2.0f));

		scrollView->setScrollBarWidth(4);
		scrollView->setTouchEnabled(false);
		scrollView->setScrollBarPositionFromCorner(Vec2(2, 2));
		scrollView->setScrollBarColor(Color3B::WHITE);
		scrollView->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
		scrollView->setBackGroundColor(Color3B(150, 150, 100));
		scrollView->getInnerContainer()->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
		scrollView->getInnerContainer()->setBackGroundColor(Color3B(50, 100, 50));
		scrollView->getInnerContainer()->setBackGroundColorOpacity(100);
		scrollView->setClippingEnabled(false); // 关闭裁剪，方便肉眼看对齐
		_uiLayer->addChild(scrollView);

		scrollView->setInnerContainerSize(Size(250.f, 100.f));

		// ★ 核心修复：强制把内部容器拉回到左上角对齐！
		// 因为原版在设置内层尺寸后，默认是把内层往左/上推的，在无锚点下会导致 Y 为负数。
		scrollView->getInnerContainer()->setPosition(Vec2::ZERO);

		return true;
	}

	return false;
}
// UIScrollViewTestEvents

UIScrollViewTestEvents::UIScrollViewTestEvents()
    : _displayValueLabel(nullptr)
{
    
}
bool UIScrollViewTestEvents::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewInnerSize, UIScrollViewStopScrollingTest);

		Size widgetSize = Director::getInstance()->getWinSize();

		// 修正标签居中
		_displayValueLabel = Text::create("(no events)", "fonts/Marker Felt.ttf", 32);
		_displayValueLabel->setPosition(Vec2((widgetSize.width - _displayValueLabel->getContentSize().width) / 2.0f,
			widgetSize.height / 2.0f - 80));
		_uiLayer->addChild(_displayValueLabel);

		// 修正提示居中
		Text* alert = Text::create("ScrollView events", "fonts/Marker Felt.ttf", 30);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2((widgetSize.width - alert->getContentSize().width) / 2.0f,
			widgetSize.height / 2.0f + 50));
		_uiLayer->addChild(alert);

		// Create the dragpanel
		ui::ScrollView* scrollView = ui::ScrollView::create();
		scrollView->setDirection(ui::ScrollView::Direction::BOTH);
		scrollView->setTouchEnabled(true);
		scrollView->setBounceEnabled(true);
		scrollView->setBackGroundImageScale9Enabled(true);
		scrollView->setBackGroundImage("green_edit.png");
		scrollView->setContentSize(Size(210, 122.5));
		scrollView->setScrollBarWidth(4);
		scrollView->setScrollBarPositionFromCorner(Vec2(6, 6));

		// ★ 核心修复 1：ScrollView 绝对居中
		scrollView->setPosition(Vec2((widgetSize.width - scrollView->getContentSize().width) / 2.0f,
			(widgetSize.height - scrollView->getContentSize().height) / 2.0f));

		ImageView* imageView = ImageView::create("Hello.png");
		scrollView->addChild(imageView);

		scrollView->setInnerContainerSize(imageView->getContentSize());
		Size innerSize = scrollView->getInnerContainerSize();

		// ★ 核心修复 2：图片在容器内左上角对齐，铺满整个内层
		imageView->setPosition(Vec2::ZERO);

		_uiLayer->addChild(scrollView);

		// Jump to right bottom
		scrollView->jumpToBottomRight();

		auto getRandomColor = [] {
			return Color4B(random(0, 255), random(0, 255), random(0, 255), 255);
		};
		scrollView->addEventListener([&](Ref*, ui::ScrollView::EventType e) {
			switch (e) {
			case ui::ScrollView::EventType::SCROLLING_BEGAN:
				_displayValueLabel->setString("scrolling began!");
				_displayValueLabel->setTextColor(getRandomColor());
				break;
			case ui::ScrollView::EventType::SCROLLING_ENDED:
				_displayValueLabel->setString("scrolling ended!");
				_displayValueLabel->setTextColor(getRandomColor());
				break;
			default: break;
			}
		});

		return true;
	}

	return false;
}

// UIScrollViewStopScrollingTest

UIScrollViewStopScrollingTest::UIScrollViewStopScrollingTest()
    : _displayValueLabel(nullptr)
    , _scrollView(nullptr)
    , _remainingTime(0.0f)
{

}
bool UIScrollViewStopScrollingTest::init()
{
	if (MyLayer::init())
	{
		//NAV_BOTH(UIScrollViewTestEvents, UIScrollViewTest_Vertical);

		Size widgetSize = Director::getInstance()->getWinSize();

		// Add a label in which the time remaining till scrolling stop will be displayed.
		_displayValueLabel = Text::create("Scrolling stop isn't scheduled", "fonts/Marker Felt.ttf", 32);
		_displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f, 50));
		_uiLayer->addChild(_displayValueLabel);

		// Add the alert
		Text* alert = Text::create("Click the button and start to scroll", "fonts/Marker Felt.ttf", 30);
		alert->setColor(Color3B(159, 168, 176));
		alert->setPosition(Vec2(widgetSize.width / 2.0f, 100));
		_uiLayer->addChild(alert);

		// Create the dragpanel
		_scrollView = ui::ScrollView::create();
		_scrollView->setDirection(ui::ScrollView::Direction::BOTH);
		_scrollView->setTouchEnabled(true);
		_scrollView->setBounceEnabled(true);
		_scrollView->setBackGroundImageScale9Enabled(true);
		_scrollView->setBackGroundImage("green_edit.png");
		_scrollView->setContentSize(Size(210, 122.5));
		_scrollView->setScrollBarWidth(4);
		_scrollView->setScrollBarPositionFromCorner(Vec2(6, 6));

		// ★ 核心修复 1：无锚点居中
		_scrollView->setPosition(Vec2((widgetSize.width - _scrollView->getContentSize().width) / 2.0f,
			(widgetSize.height - _scrollView->getContentSize().height) / 2.0f));

		ImageView* imageView = ImageView::create("Hello.png");
		_scrollView->addChild(imageView);
		_scrollView->setInnerContainerSize(imageView->getContentSize());
		Size innerSize = _scrollView->getInnerContainerSize();

		// ★ 核心修复 2：图片在容器内左上角对齐
		imageView->setPosition(Vec2::ZERO);

		_uiLayer->addChild(_scrollView);

		// Log some ScrollView events.
		_scrollView->addEventListener([&](Ref*, ui::ScrollView::EventType e)
		{
			switch (e)
			{
			case ui::ScrollView::EventType::SCROLLING_BEGAN:
				OGLOG("scrolling began!");
				break;
			case ui::ScrollView::EventType::SCROLLING_ENDED:
				OGLOG("scrolling ended!");
				break;
			case ui::ScrollView::EventType::AUTOSCROLL_ENDED:
				OGLOG("auto-scrolling ended!");
				break;
			default: break;
			}
		});

		// Jump to right bottom
		_scrollView->jumpToBottomRight();

		// Add button that will schedule scrolling stop when it is clicked.
		Button* button_scale9 = Button::create("button.png", "buttonHighlighted.png");
		button_scale9->setTitleText("Stop scrolling in 3 sec.");
		button_scale9->setScale9Enabled(true);
		button_scale9->setContentSize(Size(120.0f, button_scale9->getVirtualRendererSize().height));

		// ★ 核心修复 3：按钮在容器内居中
		button_scale9->setPosition(Vec2((innerSize.width - button_scale9->getContentSize().width) / 2.0f,
			(innerSize.height - button_scale9->getContentSize().height) / 2.0f));

		button_scale9->addClickEventListener([this](Ref*) { this->_remainingTime = 3.0f; });
		_scrollView->addChild(button_scale9);

		// Schedule update for this scene.
		Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);

		return true;
	}

	return false;
}

void UIScrollViewStopScrollingTest::update(float dt)
{
    MyLayer::update(dt);

    if (_remainingTime > 0.0f)
    {
        _remainingTime -= dt;
        
        if (_remainingTime > 0.0f)
        {
            // Update timer caption.
            char strRemainingTime[100];
            sprintf(strRemainingTime, "Stop scrolling in %.1f sec.", _remainingTime);
            _displayValueLabel->setString(std::string(strRemainingTime));
        }
        else
        {
            _scrollView->stopOverallScroll();

            // Update timer caption.
            std::string strRemainingTime = "Scrolling stop isn't scheduled";
            _displayValueLabel->setString(strRemainingTime);

            // Show hint label.
            auto hintLabel = Label::createWithTTF("Stopped!", "fonts/Marker Felt.ttf", 32.0f);
            Size contentSize = _uiLayer->getContentSize();
            hintLabel->setPosition(Vec2(contentSize.width / 2.0f, contentSize.height / 2.0f));
            hintLabel->runAction(Spawn::createWithTwoActions(
                FadeOut::create(0.3f),
                ScaleTo::create(0.3f, 2.0f)
                ));
            _uiLayer->addChild(hintLabel);
        }
    }
}
