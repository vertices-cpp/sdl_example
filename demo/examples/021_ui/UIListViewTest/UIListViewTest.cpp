
#include "UIListViewTest.h"
 
using namespace orange::ui;

const char* font_UIListViewTest = "fonts/Marker Felt.ttf";
 

UIListViewTests::UIListViewTests()
{
	ADD_TEST_CASE(UIListViewTest_Horizontal);
	ADD_TEST_CASE(UIListViewTest_Vertical);
	ADD_TEST_CASE(UIListViewTest_ScrollToItemVertical);
	ADD_TEST_CASE(UIListViewTest_ScrollToItemHorizontal);
	ADD_TEST_CASE(UIListViewTest_MagneticVertical);
	ADD_TEST_CASE(UIListViewTest_MagneticHorizontal);
	ADD_TEST_CASE(UIListViewTest_PaddingVertical);
	ADD_TEST_CASE(UIListViewTest_PaddingHorizontal);
	ADD_TEST_CASE(Issue12692);
	ADD_TEST_CASE(Issue8316);
}

// UIListViewTest_Vertical

UIListViewTest_Vertical::UIListViewTest_Vertical()
{
}

UIListViewTest_Vertical::~UIListViewTest_Vertical()
{
}

bool UIListViewTest_Vertical::init()
{
    if (MyLayer::init())
    {

		//NAV_BOTH(UIListViewTest_PaddingHorizontal, UIListViewTest_Horizontal);

        Size widgetSize =  Director::getInstance()->getWinSize();
        
        _displayValueLabel = Text::create("There are 50 items, but we only create 5 templates", "fonts/Marker Felt.ttf", 20);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f,
                                              widgetSize.height / 2.0f + _displayValueLabel->getContentSize().height * 1.5f + 30));
        _uiLayer->addChild(_displayValueLabel);
        
        
        Text* alert = Text::create("ListView vertical", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                 widgetSize.height / 2.0f - alert->getContentSize().height * 3.075f));
        _uiLayer->addChild(alert);
        
//         Layout* root = static_cast<Layout*>(_uiLayer->getChildByTag(81));
//         
//         Layout* background = dynamic_cast<Layout*>(root->getChildByName("background_Panel"));
		Size backgroundSize = widgetSize;// background->getContentSize();
        
        
        // create list view ex data
       
        for (int i = 0; i < _totalCount; ++i)
        {
            std::string ogstr = StringUtils::format("listview_item_%d", i);
            _array.push_back(ogstr);
        }
        
        
        // Create the list view ex
        _listView = ListView::create();
        // set list view ex direction
        _listView->setDirection(ui::ScrollView::Direction::VERTICAL);
        _listView->setBounceEnabled(true);
        _listView->setBackGroundImage("green_edit.png");
        _listView->setGlobalZOrder(200);
        _listView->setBackGroundImageScale9Enabled(true);
        _listView->setContentSize(Size(240.0f, 130.0f));
		_listView->setPosition(Vec2((widgetSize - _listView->getContentSize()) / 2.0f));
        _listView->addEventListener((ui::ListView::ogListViewCallback)OG_CALLBACK_2(UIListViewTest_Vertical::selectedItemEvent, this));
        _listView->addEventListener((ui::ListView::ogScrollViewCallback)OG_CALLBACK_2(UIListViewTest_Vertical::selectedItemEventScrollView,this));
		_listView->setScrollBarPositionFromCorner(Vec2(7, 7));
        _uiLayer->addChild(_listView);
        
        
        // create model
        Button* default_button = Button::create("backtotoppressed.png", "backtotopnormal.png");
        default_button->setName("Title Button");
        
        Layout* default_item = Layout::create();
        default_item->setTouchEnabled(true);
        default_item->setContentSize(default_button->getContentSize());
		default_button->setPosition(Vec2::ZERO);//设置完0紧靠着了
        default_item->addChild(default_button);
        
        //auto clippingNode = ClippingNode::create();
       // auto sprite = Sprite::create("clippingHead.jpg");
        //clippingNode->addChild(sprite);
       // auto stencil = Sprite::create("clippingStencil.jpg");
      //  clippingNode->setStencil(stencil);
        auto custom_item_contentSize = default_item->getContentSize();
     //   clippingNode->setPosition(sprite->getContentSize().width/2,
      //                            custom_item_contentSize.height/2);
      //  clippingNode->setScale(0.5);
      //  default_item->addChild(clippingNode);
        
        // set model
        _listView->setItemModel(default_item);
        
        // set all items layout gravity
        _listView->setGravity(ListView::Gravity::CENTER_VERTICAL);
        
        //initial the data
        for (int i = 0; i < _totalCount; ++i) {
            if (i < _spawnCount) {
                Widget* item = default_item->clone();
                item->setTag(i);
                Button* btn = (Button*)item->getChildByName("Title Button");
                btn->setTitleText(_array.at(i));
                _listView->pushBackCustomItem(item);
            }
        }
        
        // set items margin
        float spacing = 4;
        _listView->setItemsMargin(spacing);
        _itemTemplateHeight = default_item->getContentSize().height;
        this->_reuseItemOffset = (_itemTemplateHeight + spacing) * _spawnCount;

        //update listview data
        this->scheduleUpdate();
        
        _listView->forceDoLayout();
		float totalHeight = _itemTemplateHeight * _totalCount + (_totalCount - 1) * spacing;
		_listView->setInnerContainerSize(Size(_listView->getInnerContainerSize().width, totalHeight));
        _listView->jumpToTop();
        

        // Show the indexes of items on each boundary.
        {
            float position = 75;
            // Labels
            _indexLabels[0] = Text::create(" ", "fonts/Marker Felt.ttf", 12);
            _indexLabels[0]->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            _indexLabels[0]->setPosition(_uiLayer->getContentSize() / 2 + Size(0.0f, position));
            _uiLayer->addChild(_indexLabels[0]);
            _indexLabels[1] = Text::create("  ", "fonts/Marker Felt.ttf", 12);
            _indexLabels[1]->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            _indexLabels[1]->setPosition(_uiLayer->getContentSize() / 2 + Size(140.0f, 0.0f));
            _uiLayer->addChild(_indexLabels[1]);
            _indexLabels[2] = Text::create(" ", "fonts/Marker Felt.ttf", 12);
            _indexLabels[2]->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            _indexLabels[2]->setPosition(_uiLayer->getContentSize() / 2 + Size(0.0f, -position));
            _uiLayer->addChild(_indexLabels[2]);
            
            // Callback
            _listView->ScrollView::addEventListener([this](Ref* ref, ScrollView::EventType eventType) {
                ListView* listView = dynamic_cast<ListView*>(ref);
                if(listView == nullptr || eventType != ScrollView::EventType::CONTAINER_MOVED)
                {
                    return;
                }
                auto bottom = listView->getBottommostItemInCurrentView();
                auto center = listView->getCenterItemInCurrentView();
                auto top = listView->getTopmostItemInCurrentView();
                
                _indexLabels[0]->setString(StringUtils::format("Top index=%zd", listView->getIndex(top)));
                _indexLabels[1]->setString(StringUtils::format("Center\nindex=%zd", listView->getIndex(center)));
                _indexLabels[2]->setString(StringUtils::format("Bottom index=%zd", listView->getIndex(bottom)));
            });
        }

        return true;
    }
    
    return false;
}

float UIListViewTest_Vertical::getItemPositionYInView(orange::ui::Widget *item) const
{
	auto worldPos = item->getParent()->convertToWorldSpaceAR(item->getPosition());
	auto viewPos = this->_listView->convertToNodeSpaceAR(worldPos);
	return viewPos.y;
}

void UIListViewTest_Vertical::updateItem(int itemID, int templateID)
{
    auto itemTemplate = _listView->getItems().at(templateID);
    Button* btn = (Button*)itemTemplate->getChildByName("Title Button");

	// ★ 加这几行防呆代码
	if (itemID < 0 || itemID >= _array.size()) {
		// OGLOG("越界拦截: itemID = %d", itemID);
		return;
	}
    //If you have  the item ID and templateID, you could fill in the data here
    itemTemplate->setTag(itemID);
    btn->setTitleText(_array.at(itemID));
}

void UIListViewTest_Vertical::update(float dt)
{
    this->_updateTimer += dt;
    if (this->_updateTimer < this->_updateInterval) {
        return;
    }

    float totalHeight = _itemTemplateHeight * _totalCount + (_totalCount - 1) * 4;

    auto listViewHeight = _listView->getContentSize().height;
    
    this->_updateTimer = 0;
    auto isDown = this->_listView->getInnerContainerPosition().y < this->_lastContentPosY;
    auto items = _listView->getItems();
    
	for (int i = 0; i < _spawnCount && i < _totalCount; ++i) {
		auto item = items.at(i);
		auto itemPos = this->getItemPositionYInView(item);

		if (isDown) {
			// ★ 必须加上 item->getContentSize().height，保证底边不超过总高度
			if (itemPos < -_bufferZone && (item->getPosition().y + _reuseItemOffset + item->getContentSize().height) <= totalHeight) {
				int itemID = item->getTag() + (int)items.size();
				item->setPositionY(item->getPositionY() + _reuseItemOffset);
				this->updateItem(itemID, i);
			}
		}
		else {
			if (itemPos > /*_bufferZone +*/ listViewHeight &&
				item->getPosition().y - _reuseItemOffset >= 0) {

				item->setPositionY(item->getPositionY() - _reuseItemOffset);

				// ★ 核心修复 2：减号！
				int itemID = item->getTag() - (int)items.size();

				//OGLOG("itemPos = %f, itemID = %d, templateID = %d", itemPos, itemID, i);
				this->updateItem(itemID, i);
			}
		}
	}
    //update ListView Items
    this->_lastContentPosY = this->_listView->getInnerContainer()->getPosition().y;
}

void UIListViewTest_Vertical::selectedItemEvent(Ref *pSender, ListView::EventType type)
{
    switch (type)
    {
        case orange::ui::ListView::EventType::ON_SELECTED_ITEM_START:
        {
            ListView* listView = static_cast<ListView*>(pSender);
            auto item = listView->getItem(listView->getCurSelectedIndex());
            log("select child start index = %d", item->getTag());
            break;
        }
        case orange::ui::ListView::EventType::ON_SELECTED_ITEM_END:
        {
            ListView* listView = static_cast<ListView*>(pSender);
            auto item = listView->getItem(listView->getCurSelectedIndex());
            log("select child end index = %d", item->getTag());
            break;
        }
        default:
            break;
    }
}

void UIListViewTest_Vertical::selectedItemEventScrollView(Ref* pSender, ui::ScrollView::EventType type)
{
    switch (type) {
        case ui::ScrollView::EventType::SCROLL_TO_BOTTOM:
            OGLOG("SCROLL_TO_BOTTOM");
            break;
        case ui::ScrollView::EventType::SCROLL_TO_TOP:
            OGLOG("SCROLL_TO_TOP");
            break;
        default:
            break;
    }
}


// UIListViewTest_Horizontal
 

bool UIListViewTest_Horizontal::init()
{
    if (MyLayer::init())
    {
		//NAV_BOTH(UIListViewTest_Vertical, Issue12692);
        Size widgetSize =  Director::getInstance()->getWinSize();
        
        _displayValueLabel = Text::create("There are 50 items, but we only create 5 templates",
                                          "fonts/Marker Felt.ttf",
                                          20);
        
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(widgetSize.width / 2.0f,
                                              widgetSize.height / 2.0f
                                             + _displayValueLabel->getContentSize().height * 2 + 20));
        
        _uiLayer->addChild(_displayValueLabel);
        
        
        Text* alert = Text::create("ListView horizontal", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                widgetSize.height / 2.0f - alert->getContentSize().height * 3.075f));
        _uiLayer->addChild(alert);
        
//         Layout* root = static_cast<Layout*>(_uiLayer->getChildByTag(81));
//         
//         Layout* background = static_cast<Layout*>(root->getChildByName("background_Panel"));
		Size backgroundSize = widgetSize;// background->getContentSize();
        
        
        // create listview  data
        //The data is usually fetch from the server
        for (int i = 0; i < _totalCount; ++i)
        {
            std::string ogstr = StringUtils::format("listview_item_%d", i);
            _array.push_back(ogstr);
        }
        
        
        // Create the list view ex
        _listView = ListView::create();
        // set list view ex direction
        _listView->setDirection(ui::ScrollView::Direction::HORIZONTAL);
        _listView->setTouchEnabled(true);
        _listView->setBounceEnabled(true);
        _listView->setBackGroundImage("green_edit.png");
        _listView->setBackGroundImageScale9Enabled(true);
        _listView->setContentSize(Size(240.0f, 130.0f));
        _listView->setPosition(Vec2((widgetSize.width - backgroundSize.width) / 2.0f +
                                    (backgroundSize.width - _listView->getContentSize().width) / 2.0f,
                                    (widgetSize.height - backgroundSize.height) / 2.0f +
                                    (backgroundSize.height - _listView->getContentSize().height) / 2.0f));
        _listView->addEventListener((ui::ListView::ogListViewCallback)OG_CALLBACK_2(UIListViewTest_Horizontal::selectedItemEvent, this));
		_listView->setScrollBarPositionFromCorner(Vec2(7, 7));
        _uiLayer->addChild(_listView);
        
        
        // create model
        Button* default_button = Button::create("backtotoppressed.png",
                                                "backtotopnormal.png");
        default_button->setName("Title Button");
        
        Layout *default_item = Layout::create();
        default_item->setTouchEnabled(true);
        default_item->setContentSize(default_button->getContentSize());
		default_button->setPosition(Vec2::ZERO);
        default_item->addChild(default_button);
        
        // set model
        _listView->setItemModel(default_item);
        
        // set all items layout gravity
        _listView->setGravity(ListView::Gravity::CENTER_VERTICAL);
        
        //initial the data
        for (int i = 0; i < _totalCount; ++i) {
            if (i < _spawnCount) {
                Widget* item = default_item->clone();
                item->setTag(i);
                Button* btn = (Button*)item->getChildByName("Title Button");
                btn->setTitleText(_array.at(i));
                _listView->pushBackCustomItem(item);
            }
        }
      
        // set items margin
        float spacing = 4;
        _listView->setItemsMargin(spacing);
        _itemTemplateWidth = default_item->getContentSize().width;
        this->_reuseItemOffset = (_itemTemplateWidth + spacing) * _spawnCount;
        
        _listView->forceDoLayout();
        float totalWidth = _itemTemplateWidth * _totalCount + (_totalCount - 1) * 4;
        _listView->getInnerContainer()->setContentSize(Size(totalWidth, _listView->getInnerContainerSize().height));
        
        //update listview data
        this->scheduleUpdate();
        
        return true;
    }
    
    return false;
}

float UIListViewTest_Horizontal::getItemPositionXInView(orange::ui::Widget *item) const
{
    auto worldPos = item->getParent()->convertToWorldSpaceAR(item->getPosition());
    auto viewPos = this->_listView->convertToNodeSpaceAR(worldPos);
    return viewPos.x;
}

void UIListViewTest_Horizontal::updateItem(int itemID, int templateID)
{
    auto itemTemplate = _listView->getItems().at(templateID);
    Button* btn = (Button*)itemTemplate->getChildByName("Title Button");
    //If you have  the item ID and templateID, you could fill in the data here
    itemTemplate->setTag(itemID);
    btn->setTitleText(_array.at(itemID));
}

void UIListViewTest_Horizontal::update(float dt)
{
    this->_updateTimer += dt;
    if (this->_updateTimer < this->_updateInterval) {
        return;
    }
    float totalWidth = _itemTemplateWidth * _totalCount + (_totalCount - 1) * 4;
    
    this->_updateTimer = 0;
    auto isRight = this->_listView->getInnerContainer()->getPosition().x < this->_lastContentPosX;
    auto items = _listView->getItems();
    
    for (int i = 0; i < _spawnCount && i < _totalCount; ++i) {
        auto item = items.at(i);
        auto itemPos = this->getItemPositionXInView(item);
        if (isRight) {
            if (itemPos < -_bufferZone && item->getPosition().x + _reuseItemOffset < totalWidth) {
                int itemID = item->getTag() + (int)items.size();
                item->setPositionX(item->getPositionX() + _reuseItemOffset);
                OGLOG("itemPos = %f, itemID = %d, templateID = %d", itemPos, itemID, i);
                this->updateItem(itemID, i);
            }
        }
        else {
            if (itemPos > _bufferZone + _listView->getContentSize().width &&
                item->getPosition().x - _reuseItemOffset >= 0) {
                
                item->setPositionX(item->getPositionX() - _reuseItemOffset);
                int itemID = item->getTag() - (int)items.size();
                OGLOG("itemPos = %f, itemID = %d, templateID = %d", itemPos, itemID, i);
                this->updateItem(itemID, i);
            }
        }
    }
    //update ListView Items
    this->_lastContentPosX = this->_listView->getInnerContainer()->getPosition().x;
}

void UIListViewTest_Horizontal::selectedItemEvent(Ref *pSender, ListView::EventType type)
{
    switch (type)
    {
        case orange::ui::ListView::EventType::ON_SELECTED_ITEM_START:
        {
            ListView* listView = static_cast<ListView*>(pSender);
            auto item = listView->getItem(listView->getCurSelectedIndex());
            log("select child start index = %d", item->getTag());
            break;
        }
        case orange::ui::ListView::EventType::ON_SELECTED_ITEM_END:
        {
            ListView* listView = static_cast<ListView*>(pSender);
            auto item = listView->getItem(listView->getCurSelectedIndex());
            log("select child end index = %d", item->getTag());
            break;
        }
        default:
            break;
    }
}

bool Issue12692::init()
{
    if (MyLayer::init())
    {
		//NAV_BOTH(UIListViewTest_Horizontal, Issue8316);

        Size widgetSize =  Director::getInstance()->getWinSize();
        
        auto label = Text::create("Issue 12692", "fonts/Marker Felt.ttf", 32);
        label->setName("Text Title");
        label->setAnchorPoint(Vec2(0.5f, -1.0f));
        label->setPosition(Vec2(widgetSize.width / 2.0f,
                                widgetSize.height / 2.0f + label->getContentSize().height * 1.5f));
        _uiLayer->addChild(label);
        
        
        Text* alert = Text::create("ListView in ListView enable Scissor Clipping", "fonts/Marker Felt.ttf", 20);
        alert->setName("Text Alert");
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                widgetSize.height / 2.0f - alert->getContentSize().height * 3.075f));
        _uiLayer->addChild(alert);
        
//         Layout* root = static_cast<Layout*>(_uiLayer->getChildByTag(81));
//         
//         Layout* background = dynamic_cast<Layout*>(root->getChildByName("background_Panel"));
		Size backgroundSize = widgetSize;// background->getContentSize();
        
        // Create the list view ex
        ListView* listView = ListView::create();
        // set list view ex direction
        listView->setDirection(ui::ScrollView::Direction::VERTICAL);
        listView->setBounceEnabled(true);
        listView->setBackGroundImage("green_edit.png");
        listView->setBackGroundImageScale9Enabled(true);
        listView->setContentSize(Size(240.0f, 130.0f));
        listView->setPosition(Vec2((widgetSize.width - backgroundSize.width) / 2.0f +
                                   (backgroundSize.width - listView->getContentSize().width) / 2.0f,
                                   (widgetSize.height - backgroundSize.height) / 2.0f +
                                   (backgroundSize.height - listView->getContentSize().height) / 2.0f));
        listView->setScrollBarPositionFromCorner(Vec2(7, 7));
        listView->setClippingEnabled(true);
        listView->setClippingType(ui::Layout::ClippingType::SCISSOR);
        listView->setName("listview1");
        _uiLayer->addChild(listView);
        
        auto list2 = ListView::create();
        list2->setDirection(ui::ScrollView::Direction::VERTICAL);
        list2->setBounceEnabled(true);
        list2->setBackGroundImage("green_edit.png");
        list2->setBackGroundImageScale9Enabled(true);
        list2->setContentSize(Size(240, 65));
        list2->setClippingEnabled(true);
        list2->setClippingType(ui::Layout::ClippingType::SCISSOR);
        list2->setName("listview2");
        listView->insertCustomItem(list2, 0);
        
        {
            Button* default_button = Button::create("backtotoppressed.png", "backtotopnormal.png");
            default_button->setName("Title Button");
            
            Layout* default_item = Layout::create();
            default_item->setTouchEnabled(true);
            default_item->setContentSize(default_button->getContentSize());
            default_button->setPosition(Vec2(default_item->getContentSize().width / 2.0f,
                                             default_item->getContentSize().height / 2.0f));
            default_item->addChild(default_button);
            
            // set model
            listView->setItemModel(default_item);
            listView->pushBackDefaultItem();
            listView->pushBackDefaultItem();
            listView->pushBackDefaultItem();
        }
        {
            Button* default_button = Button::create("backtotoppressed.png", "backtotopnormal.png");
            default_button->setName("Title Button 2");
            
            Layout* default_item = Layout::create();
            default_item->setTouchEnabled(true);
            default_item->setContentSize(default_button->getContentSize());
            default_button->setPosition(Vec2(default_item->getContentSize().width / 2.0f,
                                             default_item->getContentSize().height / 2.0f));
            default_item->addChild(default_button);
            
            // set model
            list2->setItemModel(default_item);
            list2->pushBackDefaultItem();
            list2->pushBackDefaultItem();
            list2->pushBackDefaultItem();
        }
        return true;
    }
    
    return false;
}


bool Issue8316::init()
{
    if (MyLayer::init())
    {

		//NAV_BOTH(Issue12692, UIListViewTest_ScrollToItemVertical);

        Size widgetSize =  Director::getInstance()->getWinSize();
        
        auto label = Text::create("Issue 8316", "fonts/Marker Felt.ttf", 32);
        label->setAnchorPoint(Vec2(0.5f, -1.0f));
        label->setPosition(Vec2(widgetSize.width / 2.0f,
                                widgetSize.height / 2.0f + label->getContentSize().height * 2.5f));
        _uiLayer->addChild(label);
        
        Text* alert = Text::create("Can not scroll list view", "fonts/Marker Felt.ttf", 20);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(Vec2(widgetSize.width / 2.0f,
                                widgetSize.height / 2.0f - alert->getContentSize().height * 3.075f));
        _uiLayer->addChild(alert);
        
//         Layout* root = static_cast<Layout*>(_uiLayer->getChildByTag(81));
//         
//         Layout* background = dynamic_cast<Layout*>(root->getChildByName("background_Panel"));
		Size backgroundSize = widgetSize;// background->getContentSize();
        
        // Create the list view ex
        ListView* listView = ListView::create();
        // set list view ex direction
        listView->setDirection(ui::ScrollView::Direction::VERTICAL);
        listView->setBounceEnabled(true);
        listView->setTouchEnabled(false);
        listView->setBackGroundImage("green_edit.png");
        listView->setBackGroundImageScale9Enabled(true);
        listView->setContentSize(Size(240.0f, 130.0f));
        listView->setPosition(Vec2((widgetSize.width - backgroundSize.width) / 2.0f +
                                   (backgroundSize.width - listView->getContentSize().width) / 2.0f,
                                   (widgetSize.height - backgroundSize.height) / 2.0f +
                                   (backgroundSize.height - listView->getContentSize().height) / 2.0f));
        listView->setScrollBarPositionFromCorner(Vec2(7, 7));
        listView->setClippingEnabled(true);
        listView->setClippingType(ui::Layout::ClippingType::SCISSOR);
        listView->setName("listview1");
        
        {
            Button* default_button = Button::create("backtotoppressed.png", "backtotopnormal.png");
            default_button->setName("Title Button");
            
            Layout* default_item = Layout::create();
            default_item->setTouchEnabled(true);
            default_item->setContentSize(default_button->getContentSize());
            default_button->setPosition(Vec2(default_item->getContentSize().width / 2.0f,
                                             default_item->getContentSize().height / 2.0f));
            default_item->addChild(default_button);
            
            // set model
            listView->setItemModel(default_item);
            listView->pushBackDefaultItem();
            listView->pushBackDefaultItem();
            listView->pushBackDefaultItem();
        }
        
        _uiLayer->addChild(listView);
        
      
        return true;
    }
    
    return false;
}


// UIListViewTest_ScrollToItem
bool UIListViewTest_ScrollToItem::init()
{
	if (!MyLayer::init()) return false;

	

	Size layerSize = _uiLayer->getContentSize();
	static int NUMBER_OF_ITEMS = 31;
	_nextIndex = 0;
	_titleLabel = Text::create("Scroll to item", "fonts/Marker Felt.ttf", 32);
	_titleLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_titleLabel->setPosition(Vec2(layerSize / 2) + Vec2(0.0f, _titleLabel->getContentSize().height * 3.15f));
	_uiLayer->addChild(_titleLabel, 3);

	// Create the list view
	_listView = ListView::create();
	_listView->setDirection(getListViewDirection());
	_listView->setBounceEnabled(true);
	_listView->setBackGroundImage("green_edit.png");
	_listView->setBackGroundImageScale9Enabled(true);
	_listView->setContentSize(layerSize/2 );
	_listView->setScrollBarPositionFromCorner(Vec2(7, 7));
	_listView->setItemsMargin(2.0f);
	//_listView->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_listView->setPosition(layerSize / 4);
	_uiLayer->addChild(_listView);

	// Guide line for center align
	{
		DrawNode* pNode = DrawNode::create();
		Vec2 center = layerSize / 2;
		if (getListViewDirection() == ScrollView::Direction::HORIZONTAL)
		{
			float halfY = 110;
			pNode->drawLine(Vec2(center.x, center.y - halfY), Vec2(center.x, center.y + halfY), Color4F(0, 0, 0, 1));
		}
		else
		{
			float halfX = 150;
			pNode->drawLine(Vec2(center.x - halfX, center.y), Vec2(center.x + halfX, center.y), Color4F(0, 0, 0, 1));
		}
		//pNode->setLineWidth(2);
		_uiLayer->addChild(pNode);
	}

	// Button
	auto pButton = Button::create("backtotoppressed.png", "backtotopnormal.png");
	pButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	pButton->setScale(0.8f);
	pButton->setPosition(Vec2(layerSize / 2) + Vec2(120.0f, -60.0f));
	pButton->setTitleText(StringUtils::format("Go to '%d'", _nextIndex));
	pButton->addClickEventListener([this, pButton](Ref*) {
		_listView->scrollToItem(_nextIndex, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
		_nextIndex = (_nextIndex + (NUMBER_OF_ITEMS / 2)) % NUMBER_OF_ITEMS;
		pButton->setTitleText(StringUtils::format("Go to '%d'", _nextIndex));
	});
	_uiLayer->addChild(pButton);

	// Add list items
	static const Size BUTTON_SIZE(50, 40);
	for (int i = 0; i < NUMBER_OF_ITEMS; ++i)
	{
		auto pButton = Button::create("button.png", "buttonHighlighted.png");
		pButton->setContentSize(BUTTON_SIZE);
		pButton->setScale9Enabled(true);
		pButton->setTitleText(StringUtils::format("Button-%d", i));
		_listView->pushBackCustomItem(pButton);
	}

	return true;
}



// UIListViewTest_Magnetic
bool UIListViewTest_Magnetic::init()
{
	if (!MyLayer::init()) return false;
	 

	Size layerSize = _uiLayer->getContentSize();
	float centerX = layerSize.width / 2.0f;
	float centerY = layerSize.height / 2.0f;

	_titleLabel = Text::create("Magnetic scroll", "fonts/Marker Felt.ttf", 32);
	_titleLabel->setPosition(Vec2((layerSize.width - _titleLabel->getContentSize().width) / 2.0f, 30.0f));
	_uiLayer->addChild(_titleLabel, 3);

	float listW = layerSize.width * 0.7f;
	float listH = layerSize.height * 0.7f;
	_listView = ListView::create();
	_listView->setDirection(getListViewDirection());
	_listView->setBounceEnabled(true);
	_listView->setBackGroundImage("green_edit.png");
	_listView->setBackGroundImageScale9Enabled(true);
	_listView->setContentSize(Size(listW, listH));
	_listView->setScrollBarPositionFromCorner(Vec2(7, 7));
	_listView->setItemsMargin(2.0f);
	_listView->setPosition(Vec2((layerSize.width - listW) / 2.0f, (layerSize.height - listH) / 2.0f));
	_uiLayer->addChild(_listView);

	// 辅助线
	{
		DrawNode* pNode = DrawNode::create();
		if (getListViewDirection() == ScrollView::Direction::HORIZONTAL)
			pNode->drawLine(Vec2(centerX, centerY - 110), Vec2(centerX, centerY + 110), Color4F(0, 0, 0, 1));
		else
			pNode->drawLine(Vec2(centerX - 150, centerY), Vec2(centerX + 150, centerY), Color4F(0, 0, 0, 1));
		_uiLayer->addChild(pNode);
	}

	// 周围的 5 个 Label（用绝对坐标居中）
	{
		for (int i = 0; i < 5; ++i)
		{
			_indexLabels[i] = Text::create(" ", "fonts/Marker Felt.ttf", 12);
			_uiLayer->addChild(_indexLabels[i]);
		}
		float deltaX = 280, deltaY = 180; // 往两边散开一点
		_indexLabels[0]->setPosition(Vec2(centerX - deltaX, centerY));
		_indexLabels[1]->setPosition(Vec2(centerX + deltaX, centerY));
		_indexLabels[2]->setPosition(Vec2(centerX, centerY - deltaY)); // Y向下，上面变小
		_indexLabels[3]->setPosition(Vec2(centerX, centerY + deltaY)); // Y向下，下面变大
		_indexLabels[4]->setPosition(Vec2(centerX + deltaX, centerY - deltaY));

		_listView->ScrollView::addEventListener([this, centerX, centerY](Ref* ref, ScrollView::EventType eventType) {
			ListView* listView = dynamic_cast<ListView*>(ref);
			if (!listView || eventType != ScrollView::EventType::CONTAINER_MOVED) return;
			auto left = listView->getLeftmostItemInCurrentView();
			auto right = listView->getRightmostItemInCurrentView();
			auto top = listView->getTopmostItemInCurrentView();
			auto bottom = listView->getBottommostItemInCurrentView();
			auto center = listView->getCenterItemInCurrentView();

			_indexLabels[0]->setString(StringUtils::format("Left\nidx=%zd", listView->getIndex(left)));
			_indexLabels[1]->setString(StringUtils::format("Right\nidx=%zd", listView->getIndex(right)));
			_indexLabels[2]->setString(StringUtils::format("Top idx=%zd", listView->getIndex(top)));
			_indexLabels[3]->setString(StringUtils::format("Bottom idx=%zd", listView->getIndex(bottom)));
			_indexLabels[4]->setString(StringUtils::format("Center\nidx=%zd", listView->getIndex(center)));
		});
	}

	// 磁吸控制按钮（放到下面居中）
	_listView->setMagneticType(ListView::MagneticType::NONE);
	// 磁吸控制按钮（放到下面居中）
	_listView->setMagneticType(ListView::MagneticType::NONE);
	_titleLabel->setString("MagneticType - NONE");

	auto pButton = Button::create("backtotoppressed.png", "backtotopnormal.png");
	pButton->setScale(0.8f);
	pButton->setPosition(Vec2((layerSize.width - pButton->getContentSize().width * 0.8f) / 2.0f, (layerSize.height + listH) / 2.0f + 20.0f));
	pButton->setTitleText("Next Magnetic");

	// ★ 补上这段完整的回调逻辑！
	pButton->addClickEventListener([this](Ref*) {
		ListView::MagneticType eCurrentType = _listView->getMagneticType();
		ListView::MagneticType eNextType = ListView::MagneticType::NONE;
		std::string sString;

		if (eCurrentType == ListView::MagneticType::NONE)
		{
			eNextType = ListView::MagneticType::CENTER;
			sString = "CENTER";
		}
		else if (eCurrentType == ListView::MagneticType::CENTER)
		{
			eNextType = ListView::MagneticType::BOTH_END;
			sString = "BOTH_END";
		}
		else if (eCurrentType == ListView::MagneticType::BOTH_END)
		{
			if (getListViewDirection() == ScrollView::Direction::HORIZONTAL)
			{
				eNextType = ListView::MagneticType::LEFT;
				sString = "LEFT";
			}
			else
			{
				eNextType = ListView::MagneticType::TOP;
				sString = "TOP";
			}
		}
		else if (eCurrentType == ListView::MagneticType::LEFT)
		{
			eNextType = ListView::MagneticType::RIGHT;
			sString = "RIGHT";
		}
		else if (eCurrentType == ListView::MagneticType::RIGHT)
		{
			eNextType = ListView::MagneticType::NONE;
			sString = "NONE";
		}
		else if (eCurrentType == ListView::MagneticType::TOP)
		{
			eNextType = ListView::MagneticType::BOTTOM;
			sString = "BOTTOM";
		}
		else if (eCurrentType == ListView::MagneticType::BOTTOM)
		{
			eNextType = ListView::MagneticType::NONE;
			sString = "NONE";
		}

		_listView->setMagneticType(eNextType);
		_titleLabel->setString(StringUtils::format("MagneticType - %s", sString.c_str()));
	});

	_uiLayer->addChild(pButton);

	static const Size BUTTON_SIZE(100, 70);
	for (int i = 0; i < 40; ++i) {
		auto b = Button::create("button.png", "buttonHighlighted.png");
		b->setScale9Enabled(true);
		b->setContentSize(BUTTON_SIZE);
		b->setTitleText(StringUtils::format("Button-%d", i));
		_listView->pushBackCustomItem(b);
	}
	return true;
}



// UIListViewTest_Padding
bool UIListViewTest_Padding::init()
{
	if (!MyLayer::init()) return false;
	 
	Size layerSize = _uiLayer->getContentSize();

	_titleLabel = Text::create("Set Padding", "fonts/Marker Felt.ttf", 32);
	_titleLabel->setPosition(Vec2((layerSize.width - _titleLabel->getContentSize().width) / 2.0f, 30.0f));
	_uiLayer->addChild(_titleLabel, 3);

	// ListView 整体往右偏一点，给左边滑块腾出空间
	float listW = layerSize.width * 0.6f;
	float listH = layerSize.height * 0.7f;
	_listView = ListView::create();
	_listView->setDirection(getListViewDirection());
	_listView->setBounceEnabled(true);
	_listView->setBackGroundImage("green_edit.png");
	_listView->setBackGroundImageScale9Enabled(true);
	_listView->setContentSize(Size(listW, listH));
	_listView->setScrollBarPositionFromCorner(Vec2(7, 7));
	_listView->setItemsMargin(2.0f);
	_listView->setGravity(ListView::Gravity::TOP);
	_listView->setPosition(Vec2((layerSize.width - listW) / 2.0f + 100.0f, (layerSize.height - listH) / 2.0f)); // 右移 100 像素
	_uiLayer->addChild(_listView);

	// 辅助线
	{
		DrawNode* pNode = DrawNode::create();
		float centerX = (layerSize.width - listW) / 2.0f + 100.0f + listW / 2.0f;
		float centerY = layerSize.height / 2.0f;
		if (getListViewDirection() == ScrollView::Direction::HORIZONTAL)
			pNode->drawLine(Vec2(centerX, centerY - 110), Vec2(centerX, centerY + 110), Color4F(0, 0, 0, 1));
		else
			pNode->drawLine(Vec2(centerX - 150, centerY), Vec2(centerX + 150, centerY), Color4F(0, 0, 0, 1));
		_uiLayer->addChild(pNode);
	}

	// 左侧 Slider 区域
	{
		auto title = Text::create("Padding", "fonts/Marker Felt.ttf", 14);
		title->setPosition(Vec2(100.0f, 150.0f));
		_uiLayer->addChild(title);

		for (int i = 0; i < 4; ++i)
		{
			auto slider = Slider::create();
			slider->loadBarTexture("sliderTrack.png");
			slider->loadSlidBallTextures("sliderThumb.png", "sliderThumb.png", "");
			slider->loadProgressBarTexture("sliderProgress.png");
			slider->setScale9Enabled(true);
			slider->setCapInsets(Rect(0.0f, 0.0f, 0.0f, 0.0f));
			slider->setContentSize(Size(100.0f, 10.0f)); // 拉长一点
			slider->setPosition(Vec2(100.0f, 200.0f + (60 * i))); // 往下排
			slider->addEventListener(OG_CALLBACK_2(UIListViewTest_Padding::sliderEvent, this));
			slider->setTag(i);
			_uiLayer->addChild(slider);

			std::string str;
			if (i == 0) str = "Left";
			else if (i == 1) str = "Top";
			else if (i == 2) str = "Right";
			else if (i == 3) str = "Bottom";

			auto text = Text::create(str, "fonts/Marker Felt.ttf", 12);
			text->setPosition(Vec2(30.0f, 200.0f + (60 * i)));
			_uiLayer->addChild(text);

			auto valueText = Text::create(str + "\nPadding=0", "fonts/Marker Felt.ttf", 12);
			valueText->setPosition(Vec2(layerSize.width - 100.0f, 200.0f + (60 * i)));
			_uiLayer->addChild(valueText);
			_paddingLabels[i] = valueText;
		}
	}

	// 边界 Label（和上面一样）
	{
		for (int i = 0; i < 5; ++i) {
			_indexLabels[i] = Text::create(" ", "fonts/Marker Felt.ttf", 12);
			_uiLayer->addChild(_indexLabels[i]);
		}
		// ... 绑定事件和 Update 保持不变 ...
	}

	// 添加按钮
	static const Size BUTTON_SIZE(100, 70);
	for (int i = 0; i < 40; ++i) {
		auto b = Button::create("button.png", "buttonHighlighted.png");
		b->setScale9Enabled(true);
		b->setContentSize(BUTTON_SIZE);
		b->setTitleText(StringUtils::format("Button-%d", i));
		_listView->pushBackCustomItem(b);
	}
	return true;
}

void UIListViewTest_Padding::sliderEvent(Ref *pSender, Slider::EventType type)
{
    if (type == Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        auto slider = dynamic_cast<Slider*>(pSender);
        // left
        if (slider && slider->getTag() == 0)
        {
            int left = slider->getPercent() / 100.f * 50.f;
            OGLOG("Left Padding: %d", left);
            _listView->setLeftPadding(left);
            _paddingLabels[0]->setString(StringUtils::format("Left\nPadding=%d", left));
        }
        // top
        if (slider && slider->getTag() == 1)
        {
            int top = slider->getPercent() / 100.f * 50.f;
            OGLOG("Top Padding: %d", top);
            _listView->setTopPadding(top);
            _paddingLabels[1]->setString(StringUtils::format("Top\nPadding=%d", top));
        }
        // right
        if (slider && slider->getTag() == 2)
        {
            int right = slider->getPercent() / 100.f * 50.f;
            OGLOG("Right Padding: %d", right);
            _listView->setRightPadding(right);
            _paddingLabels[2]->setString(StringUtils::format("Right\nPadding=%d", right));
        }
        // bottom
        if (slider && slider->getTag() == 3)
        {
            int bottom = slider->getPercent() / 100.f * 50.f;
            OGLOG("Bottom Padding: %d", bottom);
            _listView->setBottomPadding(bottom);
            _paddingLabels[3]->setString(StringUtils::format("Bottom\nPadding=%d", bottom));
        }
    }
}

bool UIListViewTest_ScrollToItemVertical::init()
{
	UIListViewTest_ScrollToItem::init();
	//NAV_BOTH(Issue8316, UIListViewTest_ScrollToItemHorizontal);

	return true;
}

bool UIListViewTest_ScrollToItemHorizontal::init()
{
	UIListViewTest_ScrollToItem::init();
	//NAV_BOTH(UIListViewTest_ScrollToItemVertical, UIListViewTest_MagneticVertical);
	return true;
}

bool UIListViewTest_MagneticVertical::init()
{
	UIListViewTest_Magnetic::init();
	//NAV_BOTH(UIListViewTest_ScrollToItemHorizontal, UIListViewTest_MagneticHorizontal);
	return true;
}

bool UIListViewTest_MagneticHorizontal::init()
{
	UIListViewTest_Magnetic::init();
	//NAV_BOTH(UIListViewTest_MagneticVertical, UIListViewTest_PaddingVertical);
	return true;
}

bool UIListViewTest_PaddingVertical::init()
{
	UIListViewTest_Padding::init();
	//NAV_BOTH(UIListViewTest_MagneticHorizontal, UIListViewTest_PaddingHorizontal);
	return true;
}

bool UIListViewTest_PaddingHorizontal::init()
{
	UIListViewTest_Padding::init();
	//NAV_BOTH(UIListViewTest_PaddingVertical, UIListViewTest_Vertical);
	return true;
}
