
#ifndef __OGMENU_ITEM_H__
#define __OGMENU_ITEM_H__

// C++ includes
#include <functional>

// orange includes
#include "OGNode.h"
//#include "OGProtocols.h"

OG_BEGIN

typedef std::function<void(Ref*)> ogMenuCallback;

class Label;
class LabelAtlas;
class Sprite;
class SpriteFrame;
#define kItemSize 32
    
/**
 * @addtogroup _2d
 * @{
 */

/** @brief MenuItem base class.
 *
 *  Subclass MenuItem (or any subclass) to create your custom MenuItem objects.
 */
class  MenuItem : public Node
{
public: 
    static MenuItem* create(); 
    static MenuItem* create(const ogMenuCallback& callback); 
    Rect rect() const; 
    virtual void activate(); 
    virtual void selected(); 
    virtual void unselected(); 
    virtual bool isEnabled() const; 
    virtual void setEnabled(bool value); 
    virtual bool isSelected() const; 
    void setCallback(const ogMenuCallback& callback); 
   // virtual std::string getDescription() const override;
    
public: 
    MenuItem()
    : _selected(false)
    , _enabled(false)
	, _callback(nullptr)
    {
		_name = "MenuItem";
	} 
    virtual ~MenuItem(); 
    bool initWithCallback(const ogMenuCallback& callback);

protected:
    bool            _selected;
    bool            _enabled;
    // callback
    ogMenuCallback _callback;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(MenuItem);
};
 
class  MenuItemLabel : public MenuItem
{
public: 
    static MenuItemLabel * create(Node*label, const ogMenuCallback& callback); 
    static MenuItemLabel* create(Node *label); 
    void setString(const std::string& label); 
    std::string getString() const; 
    const Color3B& getDisabledColor() const { return _disabledColor; } 
    void setDisabledColor(const Color3B& color) { _disabledColor = color; } 
    Node* getLabel() const { return _label; } 
    void setLabel(Node* node);
    
    // Overrides
    virtual void activate() override;
    virtual void selected() override;
    virtual void unselected() override;
    virtual void setEnabled(bool enabled) override;
    
public: 
    MenuItemLabel()
    : _originalScale(0.0)
    , _label(nullptr)
    {
		_name = "MenuItemLabel";
	}
    /**
     * @js NA
     * @lua NA
     */
    virtual ~MenuItemLabel(); 
    bool initWithLabel(Node* label, const ogMenuCallback& callback);
    
protected:
    Color3B    _colorBackup;
    float      _originalScale; 
    Color3B _disabledColor; 
    Node* _label;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(MenuItemLabel);
};
 
class  MenuItemAtlasFont : public MenuItemLabel
{
public: 
    static MenuItemAtlasFont* create(const std::string& value, const std::string& charMapFile, int itemWidth, int itemHeight, char startCharMap);
 
    static MenuItemAtlasFont* create(const std::string& value, const std::string& charMapFile, int itemWidth, int itemHeight, char startCharMap, const ogMenuCallback& callback);
    
public: 
    MenuItemAtlasFont(){ _name = "MenuItemAtlasFont"; }
    virtual ~MenuItemAtlasFont(){}
    
     bool initWithString(const std::string& value, const std::string& charMapFile, int itemWidth, int itemHeight, char startCharMap, const ogMenuCallback& callback);

private:
    OG_DISALLOW_COPY_AND_ASSIGN(MenuItemAtlasFont);
};

 
class  MenuItemFont : public MenuItemLabel
{
public:
    
    static MenuItemFont * create(const std::string& value = ""); 
    static MenuItemFont * create(const std::string& value, const ogMenuCallback& callback); 
    static void setFontSize(int size); 
    static int getFontSize(); 
    static void setFontName(const std::string& name); 
    static const std::string& getFontName(); 
    void setFontSizeObj(int size); 
    int getFontSizeObj() const; 
    void setFontNameObj(const std::string& name); 
    const std::string& getFontNameObj() const;
    
public: 
    MenuItemFont(); 
    virtual ~MenuItemFont(); 
    bool initWithString(const std::string& value, const ogMenuCallback& callback);
    
protected:
    int _fontSize;
    std::string _fontName;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(MenuItemFont);
};
 
class  MenuItemSprite : public MenuItem
{
public: 
    static MenuItemSprite * create(Node* normalSprite, Node* selectedSprite, Node* disabledSprite = nullptr); 
    static MenuItemSprite * create(Node* normalSprite, Node* selectedSprite, const ogMenuCallback& callback); 
    static MenuItemSprite * create(Node* normalSprite, Node* selectedSprite, Node* disabledSprite, const ogMenuCallback& callback);
	 
    Node* getNormalImage() const { return _normalImage; } 
    void setNormalImage(Node* image); 
    Node* getSelectedImage() const { return _selectedImage; } 
    void setSelectedImage(Node* image); 
    Node* getDisabledImage() const { return _disabledImage; } 
    void setDisabledImage(Node* image); 
 
    virtual void selected(); 
    virtual void unselected(); 
    virtual void setEnabled(bool bEnabled);
    
public:
    MenuItemSprite()
    :_normalImage(nullptr)
    ,_selectedImage(nullptr)
    ,_disabledImage(nullptr)
    {
		_name = "MenuItemSprite";
	} 
    bool initWithNormalSprite(Node* normalSprite, Node* selectedSprite, Node* disabledSprite, const ogMenuCallback& callback);
    
protected:
    virtual void updateImagesVisibility(); 
    Node* _normalImage; 
    Node* _selectedImage; 
    Node* _disabledImage;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(MenuItemSprite);
};
 
class  MenuItemImage : public MenuItemSprite
{
public: 
    static MenuItemImage* create(); 
    static MenuItemImage* create(const std::string& normalImage, const std::string& selectedImage); 
    static MenuItemImage* create(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage);
 
    static MenuItemImage* create(const std::string&normalImage, const std::string&selectedImage, const ogMenuCallback& callback);
 
    static MenuItemImage* create(const std::string&normalImage, const std::string&selectedImage, const std::string&disabledImage, const ogMenuCallback& callback);
	 
    void setNormalSpriteFrame(SpriteFrame* frame); 
    void setSelectedSpriteFrame(SpriteFrame* frame); 
    void setDisabledSpriteFrame(SpriteFrame* frame);
    
public: 
    MenuItemImage(){ _name = "MenuItemImage"; }
    virtual ~MenuItemImage(){}
    
    bool init(); 
    bool initWithNormalImage(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const ogMenuCallback& callback);

private:
    OG_DISALLOW_COPY_AND_ASSIGN(MenuItemImage);
}; 
class  MenuItemToggle : public MenuItem
{
public: 
    static MenuItemToggle * createWithCallback(const ogMenuCallback& callback, const Vector<MenuItem*>& menuItems); 
	static MenuItemToggle* createWithCallback(const ogMenuCallback& callback, MenuItem* item, ...);/* OG_REQUIRES_NULL_TERMINATION;*/
 
    static MenuItemToggle* create(); 
    static MenuItemToggle* create(MenuItem *item); 
    void addSubItem(MenuItem *item); 
    MenuItem* getSelectedItem(); 
    unsigned int getSelectedIndex() const { return _selectedIndex; } 
    void setSelectedIndex(unsigned int index); 
    const Vector<MenuItem*>& getSubItems() const { return _subItems; }
    Vector<MenuItem*>& getSubItems() { return _subItems; } 
    void setSubItems(const Vector<MenuItem*>& items) {
        _subItems = items;
    }
    
    // Overrides
    virtual void activate() override;
    virtual void selected() override;
    virtual void unselected() override;
    virtual void setEnabled(bool var) override;
    virtual void cleanup() override;
    
public:
    /**
     * @js ctor
     */
    MenuItemToggle()
    : _selectedIndex(0)
    , _selectedItem(nullptr)
    {
		_name = "MenuItemToggle";
	} 
    bool initWithCallback(const ogMenuCallback& callback, MenuItem* item, va_list args); 
    bool initWithItem(MenuItem *item);

protected: 
    unsigned int _selectedIndex;
    MenuItem* _selectedItem; 
    Vector<MenuItem*> _subItems;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(MenuItemToggle);

}; 
OG_END

#endif //__OGMENU_ITEM_H__
