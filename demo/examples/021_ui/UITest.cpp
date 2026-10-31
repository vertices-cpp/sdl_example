#include "HelloWorld.h" 
#include "orange.h"
#include "ui/UIText.h"

#include "TestNavigation.h"
#include "UIButtonTest/UIButtonTests.h"
#include "UICheckBoxTest/UICheckBoxTest.h"

using namespace ui;
USING_OG;
 




void HelloWorld::uiTest() {

	auto fileUtils = FileUtils::getInstance();
	std::vector<std::string> searchPaths;

	//添加目录
	searchPaths.push_back("021_ui");
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
	title->setPosition(Vec2(vo.x + vs.width / 2 - title->getContentSize().width, 40));
	addChild(title, 10);

	const float itemH = 45.0f;
	const float listTop = 80.0f;
	const float listBot = 20.0f;
	const float listH = vs.height - listTop - listBot;

	std::vector<std::pair<std::string, std::function<Layer*()>>> testList = {
		ADD_CLASS(UIButtonTests),
		ADD_CLASS(UICheckBoxTest),
		// 		{"CheckBox Test",      []() { return (TestBase*)(new (std::nothrow) UICheckBoxTests); }},
// 		{"RadioButton Test",   []() { return (TestBase*)(new (std::nothrow) UIRadioButtonTests); }},
// 		{"Slider Test",        []() { return (TestBase*)(new (std::nothrow) UISliderTests); }},
// 		{"ImageView Test",     []() { return (TestBase*)(new (std::nothrow) UIImageViewTests); }},
// 		{"LoadingBar Test",    []() { return (TestBase*)(new (std::nothrow) UILoadingBarTests); }},
// 		{"TextAtlas Test",     []() { return (TestBase*)(new (std::nothrow) UITextAtlasTests); }},
// 		{"Text Test",          []() { return (TestBase*)(new (std::nothrow) UITextTests); }},
// 		{"TextBMFont Test",    []() { return (TestBase*)(new (std::nothrow) UITextBMFontTests); }},
// 		{"TextField Test",     []() { return (TestBase*)(new (std::nothrow) UITextFieldTests); }},
// 		{"Layout Test",        []() { return (TestBase*)(new (std::nothrow) UILayoutTests); }},
// 		{"ScrollView Test",    []() { return (TestBase*)(new (std::nothrow) UIScrollViewTests); }},
// 		{"PageView Test",      []() { return (TestBase*)(new (std::nothrow) UIPageViewTests); }},
// 		{"ListView Test",      []() { return (TestBase*)(new (std::nothrow) UIListViewTests); }},
// 		{"WidgetAddNode Test", []() { return (TestBase*)(new (std::nothrow) UIWidgetAddNodeTests); }},
// 		{"RichText Test",      []() { return (TestBase*)(new (std::nothrow) UIRichTextTests); }},
// 		{"Focus Test",         []() { return (TestBase*)(new (std::nothrow) UIFocusTests); }},
// 		{"TabControl Test",    []() { return (TestBase*)(new (std::nothrow) UITabControlTests); }},
// 		{"Scale9Sprite Test",  []() { return (TestBase*)(new (std::nothrow) UIScale9SpriteTests); }},
	};

	// ★ 新增：创建一个专门装测试面板的容器
	auto testContainer = Node::create();
	testContainer->setName("TestContainer");
	addChild(testContainer, 10); // Z 轴放在最上面，盖住列表

	auto sv = ui::ScrollView::create();
	sv->setName("MyMenuList"); // ★ 加上这句，方便在 Back 里找到它
	sv->setDirection(ui::ScrollView::Direction::VERTICAL);
	sv->setBounceEnabled(true);
	sv->setContentSize(Size(vs.width, listH));
	// 多加 100 像素的底部空间
	sv->setInnerContainerSize(Size(vs.width, testList.size() * itemH + 100));

	// 改成 vo.y + listTop
	sv->setPosition(Vec2(vo.x, vo.y + listTop));
	sv->setAnchorPoint(Vec2(0, 0));
	addChild(sv, 5);

	for (size_t i = 0; i < testList.size(); ++i) {
		auto lbl = Label::createWithTTF(testList[i].first, "fonts/Marker Felt.ttf", 20);
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
				suite->release();
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
		float startY = 80.0f;
		mi->setPosition(Vec2(vs.width / 2, startY + i * itemH));
		auto menu = Menu::create(mi, nullptr);
		menu->setPosition(Vec2::ZERO);
		sv->getInnerContainer()->addChild(menu);
	}
}
 

