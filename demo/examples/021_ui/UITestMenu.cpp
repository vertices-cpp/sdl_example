#include "UITestMenu.h"

bool UITestMenu::init()
{
	if (!Layer::init()) return false;

	// 创建内容容器
	_contentContainer = Node::create();
	addChild(_contentContainer, 1);

	// 创建导航容器（保证始终在最上层）
	_navContainer = Node::create();
	addChild(_navContainer, 100);

	return true;
}

void UITestMenu::addTestCase(const std::string& testName, std::function<Layer*()> callback)
{
	if (!testName.empty() && callback)
	{
		_testList.emplace_back(testName, callback);
	}
}

void UITestMenu::startFirstTest()
{
	if (!_testList.empty()) {
		switchToIndex(0);
	}
}

void UITestMenu::restartCurrTest()
{
	if (_currentIndex >= 0 && _currentIndex < (int)_testList.size()) {
		switchToIndex(_currentIndex);
	}
}

void UITestMenu::enterNextTest()
{
	if (_testList.empty()) return;
	_currentIndex = (_currentIndex + 1) % _testList.size();
	switchToIndex(_currentIndex);
}

void UITestMenu::enterPreviousTest()
{
	if (_testList.empty()) return;
	if (_currentIndex > 0) {
		_currentIndex -= 1;
	}
	else {
		_currentIndex = (int)_testList.size() - 1;
	}
	switchToIndex(_currentIndex);
}

void UITestMenu::switchToIndex(int index)
{
	_currentIndex = index;

	// 1. 清空旧界面和旧导航
	_contentContainer->removeAllChildren();
	_navContainer->removeAllChildren();

	// 2. 实例化并添加新测试界面
	auto newLayer = _testList[_currentIndex].second();
	if (newLayer) {
		_contentContainer->addChild(newLayer);
	}

	// 3. 自动生成 Prev/Next/Back 按钮（放在右下角）
	Size vs = Director::getInstance()->getWinSize();

	// 返回按钮 (Back)
		// 返回按钮 (Back)
	auto backItem = MenuItemLabel::create(Label::createWithTTF("<- Back", "fonts/Marker Felt.ttf", 24),
		[this](Ref*) {
		// 去找 HelloWorld 里的 MyMenuList 并显示出来
		auto scene = Director::getInstance()->getRunningScene();
		if (scene) {
			auto sv = (ui::ScrollView*)scene->getChildByName("MyMenuList");
			if (sv) {
				sv->setVisible(true);
				sv->setTouchEnabled(true);

				// ★★★ 核心修复：把刚才隐藏的列表项重新显示出来！ ★★★
				auto inner = sv->getInnerContainer();
				if (inner) {
					for (auto child : inner->getChildren()) {
						auto menu = dynamic_cast<Menu*>(child);
						if (menu) {
							menu->setEnabled(true);
							menu->setVisible(true);
						}
					}
				}
			}
		}
		// 销毁当前测试面板
		this->removeFromParentAndCleanup(true);
	});
	backItem->setPosition(Vec2(vs.width - 350, 40));

	auto prevItem = MenuItemLabel::create(Label::createWithTTF("< Prev", "fonts/Marker Felt.ttf", 24),
		[this](Ref*) { this->enterPreviousTest(); });
	prevItem->setPosition(Vec2(vs.width - 250, 40));

	auto nextItem = MenuItemLabel::create(Label::createWithTTF("Next >", "fonts/Marker Felt.ttf", 24),
		[this](Ref*) { this->enterNextTest(); });
	nextItem->setPosition(Vec2(vs.width - 150, 40));

	// 当前是第几个的提示 (比如 1/19)
	std::string idxStr = StringUtils::format("%d / %d", _currentIndex + 1, (int)_testList.size());
	auto idxLabel = Label::createWithTTF(idxStr, "fonts/Marker Felt.ttf", 20);
	idxLabel->setPosition(Vec2(vs.width - 200, 80));
	_navContainer->addChild(idxLabel);

	auto navMenu = Menu::create(backItem, prevItem, nextItem, nullptr);
	navMenu->setPosition(Vec2::ZERO);
	_navContainer->addChild(navMenu);
}