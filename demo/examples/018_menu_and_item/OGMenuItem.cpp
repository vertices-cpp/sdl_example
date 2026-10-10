
#include "OGMenuItem.h"
#include "OGActionInterval.h" 
#include "OGSprite.h"
#include "OGLabelAtlas.h"
#include "OGLabel.h"
#include "iconv_wrapper.h"

#include "OGActionManager.h"

#include <stdarg.h>

OG_BEGIN
    
static int _globalFontSize = kItemSize;
static std::string _globalFontName = "fonts/Marker Felt.ttf";
static bool _globalFontNameRelease = false;

const unsigned int    kZoomActionTag = 0xc0c05002;

//
// MenuItem
//

MenuItem* MenuItem::create()
{
    return MenuItem::create((const ogMenuCallback&)nullptr);
}

MenuItem* MenuItem::create( const ogMenuCallback& callback)
{
    MenuItem *ret = new (std::nothrow) MenuItem();
    ret->initWithCallback(callback);
    ret->autorelease();
    return ret;
}

bool MenuItem::initWithCallback(const ogMenuCallback& callback)
{
   // setAnchorPoint(Vec2(0.5f, 0.5f));
    _callback = callback;
    _enabled = true;
    _selected = false;
    return true;
}

MenuItem::~MenuItem()
{
}

void MenuItem::selected()
{
    _selected = true;
}

void MenuItem::unselected()
{
    _selected = false;
}

void MenuItem::activate()
{
    if (_enabled)
    {
        if( _callback )
        {
            _callback(this);
        }
// #if OG_ENABLE_SCRIPT_BINDING
//         if (kScriptTypeLua == _scriptType)
//         {
//             BasicScriptData data(this);
//             ScriptEvent scriptEvent(kMenuClickedEvent, &data);
//             ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&scriptEvent);
//         }
// #endif
    }
}

void MenuItem::setEnabled(bool enabled)
{
    _enabled = enabled;
}

bool MenuItem::isEnabled() const
{
    return _enabled;
}

Rect MenuItem::rect() const
{
    return Rect( _position.x - _contentSize.width/* * _anchorPoint.x*/,
                      _position.y - _contentSize.height/* * _anchorPoint.y*/,
                      _contentSize.width, _contentSize.height);
}

bool MenuItem::isSelected() const
{
    return _selected;
}

void MenuItem::setCallback(const ogMenuCallback& callback)
{
    _callback = callback;
}

// std::string MenuItem::getDescription() const
// {
//     return StringUtils::format("<MenuItem | tag = %d>", _tag);
// }

//
//OGMenuItemLabel
//

void MenuItemLabel::setLabel(Node* var)
{
    if (var)
    {
        //var->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        setContentSize(var->getContentSize()); 
        addChild(var);
    }
    
    if (_label)
    {
        removeChild(_label, true);
    }
    
    _label = var;
}

MenuItemLabel * MenuItemLabel::create(Node*label, const ogMenuCallback& callback)
{
    MenuItemLabel *ret = new (std::nothrow) MenuItemLabel();
    ret->initWithLabel(label, callback);
    ret->autorelease();
    return ret;
}

MenuItemLabel* MenuItemLabel::create(Node *label)
{
    MenuItemLabel *ret = new (std::nothrow) MenuItemLabel();
    ret->initWithLabel(label, (const ogMenuCallback&) nullptr);
    ret->autorelease();
    return ret;
}

bool MenuItemLabel::initWithLabel(Node* label, const ogMenuCallback& callback)
{
    MenuItem::initWithCallback(callback);
    _originalScale = 1.0f;
    _colorBackup = Color3B::WHITE;
    setDisabledColor(Color3B(126,126,126));
    this->setLabel(label);

    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    return true;
}


MenuItemLabel::~MenuItemLabel()
{
}

void MenuItemLabel::setString(const std::string& label)
{
    dynamic_cast<LabelProtocol*>(_label)->setString(label);
    this->setContentSize(_label->getContentSize());
}

std::string MenuItemLabel::getString() const
{
    auto label = dynamic_cast<LabelProtocol*>(_label);
    return label->getString();
}

void MenuItemLabel::activate()
{
    if(_enabled)
    {
        this->stopAllActions();
        this->setScale( _originalScale );
        MenuItem::activate();
    }
}

void MenuItemLabel::selected()
{
    // subclass to change the default action
    if(_enabled)
    {
        MenuItem::selected();
        
        Action *action = getActionByTag(kZoomActionTag);
        if (action)
        {
            this->stopAction(action);
        }
        else
        {
            _originalScale = this->getScale();
        }
        
        Action *zoomAction = ScaleTo::create(0.1f, _originalScale * 1.2f);
        zoomAction->setTag(kZoomActionTag);
        this->runAction(zoomAction);
    }
}

void MenuItemLabel::unselected()
{
    // subclass to change the default action
    if(_enabled)
    {
        MenuItem::unselected();
        this->stopActionByTag(kZoomActionTag);
        Action *zoomAction = ScaleTo::create(0.1f, _originalScale);
        zoomAction->setTag(kZoomActionTag);
        this->runAction(zoomAction);
    }
}

void MenuItemLabel::setEnabled(bool enabled)
{
    if( _enabled != enabled ) 
    {
        if(enabled == false)
        {
            _colorBackup = this->getColor();
            this->setColor(_disabledColor);
        }
        else
        {
            this->setColor(_colorBackup);
        }
    }
    MenuItem::setEnabled(enabled);
}

//
//OGMenuItemAtlasFont
//

MenuItemAtlasFont * MenuItemAtlasFont::create(const std::string& value, const std::string& charMapFile, int itemWidth, int itemHeight, char startCharMap)
{
    return MenuItemAtlasFont::create(value, charMapFile, itemWidth, itemHeight, startCharMap, (const ogMenuCallback&)nullptr);
}

MenuItemAtlasFont * MenuItemAtlasFont::create(const std::string& value, const std::string& charMapFile, int itemWidth, int itemHeight, char startCharMap, const ogMenuCallback& callback)
{
    MenuItemAtlasFont *ret = new (std::nothrow) MenuItemAtlasFont();
    ret->initWithString(value, charMapFile, itemWidth, itemHeight, startCharMap, callback);
    ret->autorelease();
    return ret;
}

bool MenuItemAtlasFont::initWithString(const std::string& value, const std::string& charMapFile, int itemWidth, int itemHeight, char startCharMap, const ogMenuCallback& callback)
{
    OGASSERT( value.size() != 0, "value length must be greater than 0");
    LabelAtlas *label = LabelAtlas::create();
    label->initWithString(value, charMapFile, itemWidth, itemHeight, startCharMap);
    if (MenuItemLabel::initWithLabel(label, callback))
    {
        // do something ?
    }
    return true;
}

//
//OGMenuItemFont
//

void MenuItemFont::setFontSize(int s)
{
    _globalFontSize = s;
}

int MenuItemFont::getFontSize()
{
    return _globalFontSize;
}

void MenuItemFont::setFontName(const std::string& name)
{
    if (_globalFontNameRelease)
    {
        _globalFontName.clear();
    }
    _globalFontName = name;
    _globalFontNameRelease = true;
}

const std::string& MenuItemFont::getFontName()
{
    return _globalFontName;
}

MenuItemFont * MenuItemFont::create(const std::string& value, const ogMenuCallback& callback)
{
    MenuItemFont *ret = new (std::nothrow) MenuItemFont();
    ret->initWithString(value, callback);
    ret->autorelease();
    return ret;
}


MenuItemFont * MenuItemFont::create(const std::string& value)
{
    MenuItemFont *ret = new (std::nothrow) MenuItemFont();
    ret->initWithString(value, (const ogMenuCallback&)nullptr);
    ret->autorelease();
    return ret;
}

MenuItemFont::MenuItemFont()
 : _fontSize(0), _fontName("")
{
	_name = "MenuItemFont";
}

MenuItemFont::~MenuItemFont()
{
    OGLOGINFO("In the destructor of MenuItemFont (%p).", this);
}

bool MenuItemFont::initWithString(const std::string& value, const ogMenuCallback& callback)
{
    OGASSERT( !value.empty(), "Value length must be greater than 0");

    _fontName = _globalFontName;
    _fontSize = _globalFontSize;

    Label *label = Label::createWithTTF(value, _fontName, _fontSize);
	label->updateContent();
	if (!label)
	{
		OGLOGERROR("MenuItemFont: create Label failed, font=%s size=%d",
			_fontName.c_str(), _fontSize);
		return false;
	}
    if (MenuItemLabel::initWithLabel(label, callback))
    {
        // do something ?
    }
    return true;
}

void MenuItemFont::setFontSizeObj(int s)
{
    _fontSize = s;
    dynamic_cast<Label*>(_label)->setSystemFontSize(_fontSize);
    this->setContentSize(dynamic_cast<Label*>(_label)->getContentSize());
}

int MenuItemFont::getFontSizeObj() const
{
    return _fontSize;
}

void MenuItemFont::setFontNameObj(const std::string& name)
{
    _fontName = name;
    dynamic_cast<Label*>(_label)->setSystemFontName(_fontName);
    this->setContentSize(dynamic_cast<Label*>(_label)->getContentSize());
}

const std::string& MenuItemFont::getFontNameObj() const
{
    return _fontName;
}

//
//OGMenuItemSprite
//

void MenuItemSprite::setNormalImage(Node* image)
{
    if (image != _normalImage)
    {
        if (image)
        {
            addChild(image);
            //image->setAnchorPoint(Vec2(0, 0));
        }

        if (_normalImage)
        {
            removeChild(_normalImage, true);
        }

        _normalImage = image;
        if (_normalImage != nullptr)
        {
            this->setContentSize(_normalImage->getContentSize());
        }
        this->updateImagesVisibility();
    }
}

void MenuItemSprite::setSelectedImage(Node* image)
{
    if (image != _normalImage)
    {
        if (image)
        {
            addChild(image);
           // image->setAnchorPoint(Vec2(0, 0));
        }

        if (_selectedImage)
        {
            removeChild(_selectedImage, true);
        }

        _selectedImage = image;
        this->updateImagesVisibility();
    }
}

void MenuItemSprite::setDisabledImage(Node* image)
{
    if (image != _normalImage)
    {
        if (image)
        {
            addChild(image);
            //image->setAnchorPoint(Vec2(0, 0));
        }

        if (_disabledImage)
        {
            removeChild(_disabledImage, true);
        }

        _disabledImage = image;
        this->updateImagesVisibility();
    }
}

//
//OGMenuItemSprite
//

MenuItemSprite * MenuItemSprite::create(Node* normalSprite, Node* selectedSprite, Node* disabledSprite)
{
    return MenuItemSprite::create(normalSprite, selectedSprite, disabledSprite, (const ogMenuCallback&)nullptr);
}

MenuItemSprite * MenuItemSprite::create(Node* normalSprite, Node* selectedSprite, const ogMenuCallback& callback)
{
    return MenuItemSprite::create(normalSprite, selectedSprite, nullptr, callback);
}

MenuItemSprite * MenuItemSprite::create(Node *normalSprite, Node *selectedSprite, Node *disabledSprite, const ogMenuCallback& callback)
{
    MenuItemSprite *ret = new (std::nothrow) MenuItemSprite();
    ret->initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, callback);
    ret->autorelease();
    return ret;
}

bool MenuItemSprite::initWithNormalSprite(Node* normalSprite, Node* selectedSprite, Node* disabledSprite, const ogMenuCallback& callback)
{
    MenuItem::initWithCallback(callback);
    setNormalImage(normalSprite);
    setSelectedImage(selectedSprite);
    setDisabledImage(disabledSprite);

    if(_normalImage)
    {
        this->setContentSize(_normalImage->getContentSize());
    }

    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    return true;
}

/**
 @since v0.99.5
 */
 
void MenuItemSprite::selected()
{
    MenuItem::selected();

    if (_normalImage)
    { 
		if (_disabledImage)
		{
			_disabledImage->setVisible(false);
		}

		if (_selectedImage)
		{
			_normalImage->setVisible(false);
			_selectedImage->setVisible(true);
			_selectedImage->setTransformUpdatedDirty();//处理后置脏
		}
		else
		{
			_normalImage->setVisible(true);
		}
		
    }
}

void MenuItemSprite::unselected()
{
    MenuItem::unselected();
    this->updateImagesVisibility();
}

void MenuItemSprite::setEnabled(bool bEnabled)
{
    if( _enabled != bEnabled ) 
    {
        MenuItem::setEnabled(bEnabled);
        this->updateImagesVisibility();
    }
}

// Helper 
void MenuItemSprite::updateImagesVisibility()
{
    if (_enabled)
    {
		if (_normalImage) { 
			_normalImage->setVisible(true);
			_normalImage->setTransformUpdatedDirty();//处理后置脏
		}
		if (_selectedImage) { 
			_selectedImage->setVisible(false);
		}
		if (_disabledImage) { 
			_selectedImage->setVisible(false);
			_disabledImage->setVisible(false);
		}
		
    }
    else
    {
        if (_disabledImage)
        {
            if (_normalImage)   _normalImage->setVisible(false);
            if (_selectedImage) _selectedImage->setVisible(false);
			if (_disabledImage) {
				_disabledImage->setTransformUpdatedDirty();//处理后置脏
				_disabledImage->setVisible(true);
				
			}
        }
        else
        {
			if (_normalImage) {
				_normalImage->setTransformUpdatedDirty();//处理后置脏
				_normalImage->setVisible(true);
			}
            if (_selectedImage) _selectedImage->setVisible(false);
            if (_disabledImage) _disabledImage->setVisible(false);
 
        }
    }
	
}

///
/// MenuItemImage
///

MenuItemImage* MenuItemImage::create()
{
    MenuItemImage *ret = new (std::nothrow) MenuItemImage();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    OG_SAFE_DELETE(ret);
    return nullptr;
}

bool MenuItemImage::init()
{
    return initWithNormalImage("", "", "", (const ogMenuCallback&)nullptr);
}

MenuItemImage * MenuItemImage::create(const std::string& normalImage, const std::string& selectedImage)
{
    return MenuItemImage::create(normalImage, selectedImage, "", (const ogMenuCallback&)nullptr);
}

MenuItemImage * MenuItemImage::create(const std::string& normalImage, const std::string& selectedImage, const ogMenuCallback& callback)
{
    return MenuItemImage::create(normalImage, selectedImage, "", callback);
}

MenuItemImage * MenuItemImage::create(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const ogMenuCallback& callback)
{
    MenuItemImage *ret = new (std::nothrow) MenuItemImage();
    if (ret && ret->initWithNormalImage(normalImage, selectedImage, disabledImage, callback))
    {
        ret->autorelease();
        return ret;
    }
    OG_SAFE_DELETE(ret);
    return nullptr;
}

MenuItemImage * MenuItemImage::create(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage)
{
    MenuItemImage *ret = new (std::nothrow) MenuItemImage();
    if (ret && ret->initWithNormalImage(normalImage, selectedImage, disabledImage, (const ogMenuCallback&)nullptr))
    {
        ret->autorelease();
        return ret;
    }
    OG_SAFE_DELETE(ret);
    return nullptr;
}

bool MenuItemImage::initWithNormalImage(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const ogMenuCallback& callback)
{
    Sprite *normalSprite = nullptr;
	Sprite *selectedSprite = nullptr;
	Sprite *disabledSprite = nullptr;

    if (normalImage.size() >0)
    {
        normalSprite = Sprite::create(normalImage);
    }

    if (selectedImage.size() >0)
    {
        selectedSprite = Sprite::create(selectedImage);
    }

    if(disabledImage.size() >0)
    {
        disabledSprite = Sprite::create(disabledImage);
    }
    return initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, callback);
}

//
// Setter of sprite frames
//
void MenuItemImage::setNormalSpriteFrame(SpriteFrame * frame)
{
    setNormalImage(Sprite::createWithSpriteFrame(frame));
}

void MenuItemImage::setSelectedSpriteFrame(SpriteFrame * frame)
{
    setSelectedImage(Sprite::createWithSpriteFrame(frame));
}

void MenuItemImage::setDisabledSpriteFrame(SpriteFrame * frame)
{
    setDisabledImage(Sprite::createWithSpriteFrame(frame));
}

//
// MenuItemToggle
//

MenuItemToggle * MenuItemToggle::createWithCallback(const ogMenuCallback &callback, const Vector<MenuItem*>& menuItems)
{
    MenuItemToggle *ret = new (std::nothrow) MenuItemToggle();
    ret->MenuItem::initWithCallback(callback);
    ret->autorelease();
// #if OG_ENABLE_GC_FOR_NATIVE_OBJECTS
//     auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
//     if (sEngine)
//     {
//         for (const auto &item : menuItems)
//         {
//             if (item)
//             {
//                 sEngine->retainScriptObject(ret, item);
//             }
//         }
//     }
// #endif // OG_ENABLE_GC_FOR_NATIVE_OBJECTS
    ret->_subItems = menuItems;
    ret->_selectedIndex = UINT_MAX;
    ret->setSelectedIndex(0);
    return ret;
}

MenuItemToggle * MenuItemToggle::createWithCallback(const ogMenuCallback &callback, MenuItem* item, ...)
{
    va_list args;
    va_start(args, item);
    MenuItemToggle *ret = new (std::nothrow) MenuItemToggle();
    ret->initWithCallback(callback, item, args);
    ret->autorelease();
    va_end(args);
    return ret;
}

MenuItemToggle * MenuItemToggle::create()
{
    MenuItemToggle *ret = new (std::nothrow) MenuItemToggle();
    ret->initWithItem(nullptr);
    ret->autorelease();
    return ret;
}

bool MenuItemToggle::initWithCallback(const ogMenuCallback &callback, MenuItem *item, va_list args)
{
    MenuItem::initWithCallback(callback);

    int z = 0;
    MenuItem *i = item;
    
// #if OG_ENABLE_GC_FOR_NATIVE_OBJECTS
//     auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
// #endif // OG_ENABLE_GC_FOR_NATIVE_OBJECTS
    
    while(i)
    {
        z++;
// #if OG_ENABLE_GC_FOR_NATIVE_OBJECTS
//         if (sEngine)
//         {
//             sEngine->retainScriptObject(this, i);
//         }
// #endif // OG_ENABLE_GC_FOR_NATIVE_OBJECTS
        _subItems.pushBack(i);
        i = va_arg(args, MenuItem*);
    }
    _selectedIndex = UINT_MAX;
    this->setSelectedIndex(0);
    return true;
}

MenuItemToggle* MenuItemToggle::create(MenuItem *item)
{
    MenuItemToggle *ret = new (std::nothrow) MenuItemToggle();
    ret->initWithItem(item);
    ret->autorelease();
    return ret;
}

bool MenuItemToggle::initWithItem(MenuItem *item)
{
    MenuItem::initWithCallback((const ogMenuCallback&)nullptr);

    if (item)
    {
        addSubItem(item);
    }
    _selectedIndex = UINT_MAX;
    this->setSelectedIndex(0);
    
    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);
    
    return true;
}

void MenuItemToggle::addSubItem(MenuItem *item)
{
#if OG_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        sEngine->retainScriptObject(this, item);
    }
#endif // OG_ENABLE_GC_FOR_NATIVE_OBJECTS
    _subItems.pushBack(item);
}

void MenuItemToggle::cleanup()
{
    for(const auto &item : _subItems) {
#if defined(OG_NATIVE_CONTROL_SCRIPT) && !OG_NATIVE_CONTROL_SCRIPT
        ScriptEngineManager::getInstance()->getScriptEngine()->releaseScriptObject(this, item);
#endif
        item->cleanup();
    }
    MenuItem::cleanup();
}

void MenuItemToggle::setSelectedIndex(unsigned int index)
{
    if( index != _selectedIndex && _subItems.size() > 0 )
    {
        _selectedIndex = index;
        if (_selectedItem)
        {
            _selectedItem->removeFromParentAndCleanup(false);
        }

        _selectedItem = _subItems.at(_selectedIndex);
        this->addChild(_selectedItem);
        Size s = _selectedItem->getContentSize();
        this->setContentSize(s);
        //_selectedItem->setPosition(s.width / 2, s.height / 2);//(w/2, h/2) 就变成了“中心”，所以偏.注释掉了
    }
}

void MenuItemToggle::selected()
{
    MenuItem::selected();
    _subItems.at(_selectedIndex)->selected();
}

void MenuItemToggle::unselected()
{
    MenuItem::unselected();
    _subItems.at(_selectedIndex)->unselected();
}

void MenuItemToggle::activate()
{
    // update index
    if( _enabled ) 
    {
        unsigned int newIndex = (_selectedIndex + 1) % _subItems.size();
        this->setSelectedIndex(newIndex);
    }
    MenuItem::activate();
}
void MenuItemToggle::setEnabled(bool enabled)
{
    if (_enabled != enabled)
    {
        MenuItem::setEnabled(enabled);

        for(const auto &item : _subItems) {
            item->setEnabled(enabled);
        }
    }
}

MenuItem* MenuItemToggle::getSelectedItem()
{
    return _subItems.at(_selectedIndex);
}

OG_END
