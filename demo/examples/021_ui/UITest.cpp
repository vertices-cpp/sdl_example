#include "HelloWorld.h" 
#include "orange.h"
#include "ui/UIText.h" 


#include "TestNavigation.h"
#include "UIButtonTest/UIButtonTests.h"
#include "UICheckBoxTest/UICheckBoxTest.h"
#include "UIFocusTest/UIFocusTest.h"
#include "UIImageViewTest/UIImageViewTest.h"
#include "UILayoutTest/UILayoutTest.h"
#include "UIListViewTest/UIListViewTest.h"
#include "UILoadingBarTest/UILoadingBarTest.h"
#include "UIPageViewTest/UIPageViewTest.h"
#include "UIRadioButtonTest/UIRadioButtonTest.h"
#include "UIScrollViewTest/UIScrollViewTest.h"

#include "UISliderTest/UISliderTest.h"
#include "UITabControlTest/UITabControlTest.h"
#include "UITextAtlasTest/UITextAtlasTest.h"
#include "UITextBMFontTest/UITextBMFontTest.h"
#include "UITextFieldTest/UITextFieldTest.h"
#include "UITextTest/UITextTest.h"
#include "UIWidgetAddNodeTest/UIWidgetAddNodeTest.h"


#include "UIEditBoxTest.h"

#include "UIRichTextTest/UIRichTextTest.h"

#include <string>

using namespace ui;
USING_OG;
 




void HelloWorld::uiTest() { 

	auto fileUtils = FileUtils::getInstance();
	std::vector<std::string> searchPaths;

	//添加目录
	searchPaths.push_back("021_ui");
	searchPaths.push_back("Images");
	searchPaths.push_back("fonts");
	fileUtils->setSearchPaths(searchPaths);

	auto director = Director::getInstance();
	auto glview = director->getSDLView();

	Size screenSize = director->getWinSize();

	auto vs = Director::getInstance()->getWinSize();
	auto vo = Vec2::ZERO;

	auto bg = LayerColor::create(Color4B(30, 30, 35, 255), vs.width, vs.height);
	bg->setPosition(vo);
	addChild(bg);

	auto title = Label::createWithTTF("Official UI Tests", "fonts/Marker Felt.ttf", 24);
	title->setPosition(Vec2(vo.x + vs.width / 3 - title->getContentSize().width, 40));
	addChild(title, 10);

	const float itemH = 45.0f;
	const float listTop = 40.0f;
	const float listBot = 20.0f;
	const float listH = vs.height - listTop - listBot;

	std::vector<std::pair<std::string, std::function<Layer*()>>> testList = {
		ADD_TEST(UIButtonTests)
		,ADD_TEST(UICheckBoxTests)
		,ADD_TEST(UIFocusTests)
		,ADD_TEST(UIImageViewTests)
		,ADD_TEST(UILayoutTests)
		,ADD_TEST(UIListViewTests)
		,ADD_TEST(UILoadingBarTests)
		,ADD_TEST(UIPageViewTests)
		,ADD_TEST(UIRadioButtonTests)
		,ADD_TEST(UIScrollViewTests)
		,ADD_TEST(UISliderTests) 
		,ADD_TEST(UITabControlTests)

		,ADD_TEST(UITextAtlasTests)
		,ADD_TEST(UITextBMFontTests)
		,ADD_TEST(UITextFieldTests)
		,ADD_TEST(UITextTests) 
		,ADD_TEST(UIWidgetAddNodeTests)

		,ADD_TEST(UIEditBoxTests)
		
		,ADD_TEST(UIRichTextTests)
	
		
		//,ADD_TEST(UIScale9SpriteTests)
	};

	// ★ 新增：创建一个专门装测试面板的容器
	auto testContainer = Node::create();
	testContainer->setName("TestContainer");
	addChild(testContainer, 10); // Z 轴放在最上面，盖住列表

	auto sv = ui::ScrollView::create();
	sv->setName("MyMenuList"); // ★ 加上这句，方便在 Back 里找到它
	sv->setDirection(ui::ScrollView::Direction::VERTICAL);
	sv->setBounceEnabled(true);
	sv->setContentSize(Size(vs.width, listH ));
	// 总高度 = 顶部留白(80) + 所有元素高度 + 底部留白(20)
	float startY = 80.0f;
	float totalInnerHeight = startY + testList.size() * itemH + 20.0f;
	sv->setInnerContainerSize(Size(vs.width, totalInnerHeight));

	// 改成 vo.y + listTop
	sv->setPosition(Vec2(vo.x, vo.y + listTop));
	sv->setAnchorPoint(Vec2(0, 0));
	addChild(sv, 5);

	for (size_t i = 0; i < testList.size(); ++i) {
		auto lbl = Label::createWithTTF(std::to_string(i+1) +"-" + testList[i].first, "fonts/Marker Felt.ttf", 20);
		auto factory = testList[i].second;

		// ★ 修改点击回调
		auto mi = MenuItemLabel::create(lbl, [factory, testContainer, sv](Ref*) {
			// 1. 清空上一次的测试面板
			testContainer->removeAllChildren();

			// 2. 创建新的测试面板 (注意 factory 返回的现在是 Layer*)
			auto suite = factory();
			if (suite) {
				// 3. 把测试面板塞进容器
				testContainer->addChild(suite);

				// 4. 让套件释放自己，由容器接管生命周期
				//suite->release();
			}

			// 隐藏菜单列表，防止点穿（可选）
			sv->setVisible(false);
			sv->setTouchEnabled(false);

			auto inner = sv->getInnerContainer();
			if (inner) {
				for (auto child : inner->getChildren()) {
					auto menu = dynamic_cast<Menu*>(child);
					if (menu) {
						menu->setEnabled(false); // 禁用菜单点击
						menu->setVisible(false); // 顺便隐藏
					}
				}
			}
		});
		//mi->setEnabled(false);
		// 定义顶部留白 80 像素（避开标题）
		
		mi->setPosition(Vec2(vs.width / 2 - 100, startY + i * itemH));
		auto menu = Menu::create(mi, nullptr);
		menu->setPosition(Vec2::ZERO);
		sv->getInnerContainer()->addChild(menu);
	}
}
 

