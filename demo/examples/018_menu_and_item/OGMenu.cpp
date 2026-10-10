#include "OGMenu.h"
#include "OGCamera.h"
#include "OGDirector.h" 
#include "OGEventDispatcher.h"
#include "iconv_wrapper.h" 
#include "OGEventListenerTouch.h"

#include <vector>
  
OG_BEGIN

enum 
{
    kDefaultPadding =  5,
};

//
//OGMenu
//

Menu::~Menu()
{
	_name = "Menu";
    OGLOGINFO("In the destructor of Menu. %p", this);
}


Menu* Menu::create()
{
    return Menu::create(nullptr, nullptr);
}

Menu * Menu::create(MenuItem* item, ...)
{
    va_list args;
    va_start(args,item);
    
    Menu *ret = Menu::createWithItems(item, args);
    
    va_end(args);
    
    return ret;
}

Menu* Menu::createWithArray(const Vector<MenuItem*>& arrayOfItems)
{
    auto ret = new (std::nothrow) Menu();
    if (ret && ret->initWithArray(arrayOfItems))
    {
        ret->autorelease();
    }
    else
    {
        OG_SAFE_DELETE(ret);
    }
    
    return ret;
}

Menu* Menu::createWithItems(MenuItem* item, va_list args)
{
    Vector<MenuItem*> items;
    if( item )
    {
        items.pushBack(item);
        MenuItem *i = va_arg(args, MenuItem*);
        while(i)
        {
            items.pushBack(i);
            i = va_arg(args, MenuItem*);
        }
    }
    
    return Menu::createWithArray(items);
}

Menu* Menu::createWithItem(MenuItem* item)
{
    return Menu::create(item, nullptr);
}

bool Menu::init()
{
    return initWithArray(Vector<MenuItem*>());
}

bool Menu::initWithArray(const Vector<MenuItem*>& arrayOfItems)
{
    if (Layer::init())
    {
        _enabled = true;
        // menu in the center of the screen
        Size s = Director::getInstance()->getWinSize();

       // this->setIgnoreAnchorPointForPosition(true);
        //setAnchorPoint(Vec2(0.5f, 0.5f));
        this->setContentSize(s);

        setPosition(s.width/2, s.height/2);
        
        int z=0;
        
        for (auto& item : arrayOfItems)
        {
            this->addChild(item, z);
            z++;
        }
    
        _selectedItem = nullptr;
        _state = Menu::State::WAITING;
        
        // enable cascade color and opacity on menus
        setCascadeColorEnabled(true);
        setCascadeOpacityEnabled(true);
        
        
        auto touchListener = EventListenerTouchOneByOne::create();
        touchListener->setSwallowTouches(true);
        
        touchListener->onTouchBegan = OG_CALLBACK_2(Menu::onTouchBegan, this);
        touchListener->onTouchMoved = OG_CALLBACK_2(Menu::onTouchMoved, this);
        touchListener->onTouchEnded = OG_CALLBACK_2(Menu::onTouchEnded, this);
        touchListener->onTouchCancelled = OG_CALLBACK_2(Menu::onTouchCancelled, this);
        
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
        
        return true;
    }
    return false;
}

/*
* override add:
*/
void Menu::addChild(Node * child)
{
    Layer::addChild(child);
}

void Menu::addChild(Node * child, int zOrder)
{
    Layer::addChild(child, zOrder);
	child->setParent(this);//菜单需要
}

void Menu::addChild(Node * child, int zOrder, int tag)
{
    OGASSERT( dynamic_cast<MenuItem*>(child) != nullptr, "Menu only supports MenuItem objects as children");
    Layer::addChild(child, zOrder, tag);
}

void Menu::addChild(Node * child, int zOrder, const std::string &name)
{
    OGASSERT( dynamic_cast<MenuItem*>(child) != nullptr, "Menu only supports MenuItem objects as children");
    Layer::addChild(child, zOrder, name);
}

void Menu::onEnter()
{
#if OG_ENABLE_SCRIPT_BINDING
// 	if (_scriptType == kScriptTypeJavascript)
// 	{
// 		if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnter))
// 			return;
// 	}
#endif
    
    Layer::onEnter();
}

void Menu::onExit()
{
#if OG_ENABLE_SCRIPT_BINDING
//     if (_scriptType == kScriptTypeJavascript)
//     {
//         if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExit))
//             return;
//     }
#endif
    
    if (_state == Menu::State::TRACKING_TOUCH)
    {
        if (_selectedItem)
        {
            _selectedItem->unselected();
            _selectedItem = nullptr;
        }
        
        _state = Menu::State::WAITING;
    }

    Layer::onExit();
}

void Menu::removeChild(Node* child, bool cleanup)
{
    OGASSERT(dynamic_cast<MenuItem*>(child) != nullptr, "Menu only supports MenuItem objects as children");
    
    if (_selectedItem == child)
    {
        _selectedItem = nullptr;
    }
    
    Node::removeChild(child, cleanup);
}

//Menu - Events

bool Menu::onTouchBegan(Touch* touch, Event* /*event*/)
{
    auto camera = Camera::getVisitingCamera();
    if (_state != Menu::State::WAITING || ! _visible || !_enabled || !camera)
    {
        return false;
    }
    
    for (Node *c = this->_parent; c != nullptr; c = c->getParent())
    {
        if (c->isVisible() == false)
        {
            return false;
        }
    }
    _selectedItem = this->getItemForTouch(touch, camera);

    if (_selectedItem)
    {
        _state = Menu::State::TRACKING_TOUCH;
        _selectedWithCamera = camera;
        _selectedItem->selected();
        
        return true;
    }
    
    return false;
}

void Menu::onTouchEnded(Touch* /*touch*/, Event* /*event*/)
{
    OGASSERT(_state == Menu::State::TRACKING_TOUCH, "[Menu ogTouchEnded] -- invalid state");
    this->retain();
    if (_selectedItem)
    {
        _selectedItem->unselected();
        _selectedItem->activate();
    }
    _state = Menu::State::WAITING;
    _selectedWithCamera = nullptr;
    this->release();
}

void Menu::onTouchCancelled(Touch* /*touch*/, Event* /*event*/)
{
    OGASSERT(_state == Menu::State::TRACKING_TOUCH, "[Menu ogTouchCancelled] -- invalid state");
    this->retain();
    if (_selectedItem)
    {
        _selectedItem->unselected();
    }
    _state = Menu::State::WAITING;
    this->release();
}

void Menu::onTouchMoved(Touch* touch, Event* /*event*/)
{
    OGASSERT(_state == Menu::State::TRACKING_TOUCH, "[Menu ogTouchMoved] -- invalid state");
    MenuItem *currentItem = this->getItemForTouch(touch, _selectedWithCamera);
    if (currentItem != _selectedItem)
    {
        if (_selectedItem)
        {
            _selectedItem->unselected();
        }
        _selectedItem = currentItem;
        if (_selectedItem)
        {
            _selectedItem->selected();
        }
    }
}

//Menu - Alignment
void Menu::alignItemsVertically()
{
    this->alignItemsVerticallyWithPadding(kDefaultPadding);
}

void Menu::alignItemsVerticallyWithPadding(float padding)
{
    float height = -padding;

    for(const auto &child : _children)
        height += child->getContentSize().height * child->getScaleY() + padding;

    float y = height / 2.0f;
    
    for(const auto &child : _children) {
        child->setPosition(0, y - child->getContentSize().height * child->getScaleY() / 2.0f);
        y -= child->getContentSize().height * child->getScaleY() + padding;
    }
}

void Menu::alignItemsHorizontally()
{
    this->alignItemsHorizontallyWithPadding(kDefaultPadding);
}

void Menu::alignItemsHorizontallyWithPadding(float padding)
{
    float width = -padding;
    for(const auto &child : _children)
        width += child->getContentSize().width * child->getScaleX() + padding;

    float x = -width / 2.0f;
    
    for(const auto &child : _children) {
        child->setPosition(x + child->getContentSize().width * child->getScaleX() / 2.0f, 0);
        x += child->getContentSize().width * child->getScaleX() + padding;
    }
}

void Menu::alignItemsInColumns(int columns, ...)
{
    va_list args;
    va_start(args, columns);

    this->alignItemsInColumns(columns, args);

    va_end(args);
}

void Menu::alignItemsInColumns(int columns, va_list args)
{
    OGASSERT(columns >= 0, "Columns must be >= 0");
    ValueVector rows;
    while (columns)
    {
        rows.push_back(Value(columns));
        columns = va_arg(args, int);
    }
    alignItemsInColumnsWithArray(rows);
}

void Menu::alignItemsInColumnsWithArray(const ValueVector& rows)
{
    int height = -5;
    size_t row = 0;
    int rowHeight = 0;
    int columnsOccupied = 0;
    int rowColumns = 0;

    for(const auto &child : _children) {
        OGASSERT(row < rows.size(), "row should less than rows.size()!");
        
        rowColumns = rows[row].asInt();
        // can not have zero columns on a row
        OGASSERT(rowColumns, "rowColumns can't be 0.");
        
        float tmp = child->getContentSize().height;
        rowHeight = (unsigned int)((rowHeight >= tmp || isnan(tmp)) ? rowHeight : tmp);
        
        ++columnsOccupied;
        if (columnsOccupied >= rowColumns)
        {
            height += rowHeight + 5;
            
            columnsOccupied = 0;
            rowHeight = 0;
            ++row;
        }
    }

    // check if too many rows/columns for available menu items
    OGASSERT(! columnsOccupied, "columnsOccupied should be 0.");

    Size winSize = getContentSize();

    row = 0;
    rowHeight = 0;
    rowColumns = 0;
    float w = 0.0;
    float x = 0.0;
    float y = (float)(height / 2);

    for(const auto &child : _children) {
        if (rowColumns == 0)
        {
            rowColumns = rows[row].asInt();
            w = winSize.width / (1 + rowColumns);
            x = w;
        }

        float tmp = child->getContentSize().height;
        rowHeight = (unsigned int)((rowHeight >= tmp || isnan(tmp)) ? rowHeight : tmp);

        child->setPosition(x - winSize.width / 2,
                               y - child->getContentSize().height / 2);

        x += w;
        ++columnsOccupied;

        if (columnsOccupied >= rowColumns)
        {
            y -= rowHeight + 5;

            columnsOccupied = 0;
            rowColumns = 0;
            rowHeight = 0;
            ++row;
        }
    }
}

void Menu::alignItemsInRows(int rows, ...)
{
    va_list args;
    va_start(args, rows);

    this->alignItemsInRows(rows, args);

    va_end(args);
}

void Menu::alignItemsInRows(int rows, va_list args)
{
    ValueVector array;
    while (rows)
    {
        array.push_back(Value(rows));
        rows = va_arg(args, int);
    }
    alignItemsInRowsWithArray(array);
}

void Menu::alignItemsInRowsWithArray(const ValueVector& columns)
{
    std::vector<int> columnWidths;
	std::vector<int> columnHeights;

    int width = -10;
    int columnHeight = -5;
    size_t column = 0;
    int columnWidth = 0;
    int rowsOccupied = 0;
    int columnRows;

    for(const auto &child : _children) {
        // check if too many menu items for the amount of rows/columns
        OGASSERT(column < columns.size(), "column should be less than columns.size().");

        columnRows = columns[column].asInt();
        // can't have zero rows on a column
        OGASSERT(columnRows, "columnRows can't be 0.");

        // columnWidth = fmaxf(columnWidth, [item contentSize].width);
        float tmp = child->getContentSize().width;
        columnWidth = (unsigned int)((columnWidth >= tmp || isnan(tmp)) ? columnWidth : tmp);

        columnHeight += (int)(child->getContentSize().height + 5);
        ++rowsOccupied;

        if (rowsOccupied >= columnRows)
        {
            columnWidths.push_back(columnWidth);
            columnHeights.push_back(columnHeight);
            width += columnWidth + 10;

            rowsOccupied = 0;
            columnWidth = 0;
            columnHeight = -5;
            ++column;
        }
    }

    // check if too many rows/columns for available menu items.
    OGASSERT(! rowsOccupied, "rowsOccupied should be 0.");

    Size winSize = getContentSize();

    column = 0;
    columnWidth = 0;
    columnRows = 0;
    float x = (float)(-width / 2);
    float y = 0.0;

    for(const auto &child : _children) {
        if (columnRows == 0)
        {
            columnRows = columns[column].asInt();
            y = (float) columnHeights[column];
        }

        // columnWidth = fmaxf(columnWidth, [item contentSize].width);
        float tmp = child->getContentSize().width;
        columnWidth = (unsigned int)((columnWidth >= tmp || isnan(tmp)) ? columnWidth : tmp);

        child->setPosition(x + columnWidths[column] / 2,
                               y - winSize.height / 2);

        y -= child->getContentSize().height + 10;
        ++rowsOccupied;

        if (rowsOccupied >= columnRows)
        {
            x += columnWidth + 5;
            rowsOccupied = 0;
            columnRows = 0;
            columnWidth = 0;
            ++column;
        }
    }
}

MenuItem* Menu::getItemForTouch(Touch *touch, const Camera *camera)
{
	Vec2 touchLocation = touch->getLocation();
	for (const auto &item : _children)
	{
		MenuItem* child = dynamic_cast<MenuItem*>(item);
		if (nullptr == child || false == child->isVisible() || false == child->isEnabled())
		{
			continue;
		}
		Rect rect;
		rect.size = child->getContentSize();
		if (isScreenPointInRect(touchLocation, camera, child->getWorldToNodeTransform(), rect, nullptr))
		{ 
			return child;
		}
	}
	return nullptr;
}

void Menu::setOpacityModifyRGB(bool /*value*/)
{}

bool Menu::isOpacityModifyRGB() const
{
    return false;
}
 

OG_END
