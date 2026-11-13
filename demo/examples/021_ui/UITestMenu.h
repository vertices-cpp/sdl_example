#ifndef __UI_TEST_MENU_H__
#define __UI_TEST_MENU_H__

#include "orange.h"
#include <functional>
#include <vector>
#include <string>

USING_OG;
using namespace ui;

class UITestMenu : public Layer {
public:
	CREATE_FUNC(UITestMenu);

	// 默认初始化
	virtual bool init() override;

	// 注册测试用例
	void addTestCase(const std::string& testName, std::function<Layer*()> callback);

	// 导航操作
	void restartCurrTest();
	virtual void enterNextTest();
	virtual void enterPreviousTest();

	// 如果是从外部唤起，可以直接跳到第一个测试
	virtual void startFirstTest();

protected:
	// 内部真正执行切换的逻辑
	void switchToIndex(int index);

	std::vector<std::pair<std::string, std::function<Layer*()>>> _testList;
	int _currentIndex = -1;

	Node* _contentContainer = nullptr; // 专门用来放测试界面的容器
	Node* _navContainer = nullptr;     // 专门用来放 Prev/Next 按钮的容器
};

#endif