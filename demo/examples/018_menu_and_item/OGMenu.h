
#ifndef __OGMENU_H_
#define __OGMENU_H_

#include "OGMenuItem.h"
#include "OGLayer.h"
#include "OGValue.h"


OG_BEGIN

class Camera;

class Touch; 

class Menu : public Layer
{
public: 
    enum class State
    {
        WAITING,
        TRACKING_TOUCH,
    }; 
    static Menu* create();
    
    /** Creates a Menu with MenuItem objects. */
    static Menu* create(MenuItem* item, ...)/* OG_REQUIRES_NULL_TERMINATION*/; 
    static Menu* createWithArray(const Vector<MenuItem*>& arrayOfItems); 
    static Menu* createWithItem(MenuItem* item); 
    static Menu* createWithItems(MenuItem *firstItem, va_list args); 
    void alignItemsVertically(); 
    void alignItemsVerticallyWithPadding(float padding); 
    void alignItemsHorizontally(); 
    void alignItemsHorizontallyWithPadding(float padding); 
    void alignItemsInColumns(int columns, ...) /*OG_REQUIRES_NULL_TERMINATION*/; 
    void alignItemsInColumns(int columns, va_list args); 
    void alignItemsInColumnsWithArray(const ValueVector& rows); 
    void alignItemsInRows(int rows, ...) /*OG_REQUIRES_NULL_TERMINATION*/; 
    void alignItemsInRows(int rows, va_list args); 
    void alignItemsInRowsWithArray(const ValueVector& columns); 
    virtual bool isEnabled() const { return _enabled; } 
    virtual void setEnabled(bool value) { _enabled = value; };

    virtual bool onTouchBegan(Touch* touch, Event* event) override;
    virtual void onTouchEnded(Touch* touch, Event* event) override;
    virtual void onTouchCancelled(Touch* touch, Event* event) override;
    virtual void onTouchMoved(Touch* touch, Event* event) override;
    
    // overrides
    virtual void removeChild(Node* child, bool cleanup) override;
    
    virtual void addChild(Node * child) override;
    virtual void addChild(Node * child, int zOrder) override;
    virtual void addChild(Node * child, int zOrder, int tag) override;
    virtual void addChild(Node * child, int zOrder, const std::string &name) override;
    
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void setOpacityModifyRGB(bool value) override;
    virtual bool isOpacityModifyRGB() const override;

 //   virtual std::string getDescription() const override;

public:
    /**
     * @js ctor
     */
    Menu() : _selectedItem(nullptr), _selectedWithCamera(nullptr) {}
    virtual ~Menu(); 
    bool init() override; 
    bool initWithArray(const Vector<MenuItem*>& arrayOfItems);

protected:



    /** whether or not the menu will receive events */
    bool _enabled;

    virtual MenuItem* getItemForTouch(Touch * touch, const Camera *camera);
    State _state;
    MenuItem *_selectedItem;
    const Camera *_selectedWithCamera;
private:
    OG_DISALLOW_COPY_AND_ASSIGN(Menu);
};

// end of _2d group
/// @}

OG_END

#endif//__OGMENU_H_
