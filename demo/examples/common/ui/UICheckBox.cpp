

#include "ui/UICheckBox.h"

OG_BEGIN

namespace ui {
    
IMPLEMENT_CLASS_GUI_INFO(CheckBox)

CheckBox::CheckBox()
: _checkBoxEventListener(nullptr)
{
}

CheckBox::~CheckBox()
{
}

CheckBox* CheckBox::create()
{
    CheckBox* widget = new (std::nothrow) CheckBox();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}
    
CheckBox* CheckBox::create(const std::string& backGround,
                           const std::string& backGroundSelected,
                           const std::string& cross,
                           const std::string& backGroundDisabled,
                           const std::string& frontCrossDisabled,
                           TextureResType texType)
{
    CheckBox *pWidget = new (std::nothrow) CheckBox;
    if (pWidget && pWidget->init(backGround,
                                 backGroundSelected,
                                 cross,
                                 backGroundDisabled,
                                 frontCrossDisabled,
                                 texType))
    {
        pWidget->autorelease();
        return pWidget;
    }
    OG_SAFE_DELETE(pWidget);
    return nullptr;
}
    
CheckBox* CheckBox::create(const std::string& backGround,
                           const std::string& cross,
                           TextureResType texType)
{
    CheckBox *pWidget = new (std::nothrow) CheckBox;
    if (pWidget && pWidget->init(backGround,
                                 "",
                                 cross,
                                 "",
                                 "",
                                 texType))
    {
        pWidget->autorelease();
        return pWidget;
    }
    OG_SAFE_DELETE(pWidget);
    return nullptr;
}
    
void CheckBox::onTouchEnded(Touch *touch, Event *unusedEvent)
{
    bool highlight = _highlight;

    AbstractCheckButton::onTouchEnded(touch, unusedEvent);

    if (highlight)
    {
        if (_isSelected)
        {
            setSelected(false);
            dispatchSelectChangedEvent(false);
        }
        else
        {
            setSelected(true);
            dispatchSelectChangedEvent(true);
        }
    }
}
    

void CheckBox::dispatchSelectChangedEvent(bool selected)
{
    EventType eventType = (selected ? EventType::SELECTED : EventType::UNSELECTED);
    
    this->retain();
    if (_checkBoxEventCallback)
    {
        _checkBoxEventCallback(this, eventType);
    }
    if (_ogEventCallback)
    {
        _ogEventCallback(this, static_cast<int>(eventType));
    }
    
    this->release();
    
}

void CheckBox::addEventListener(const ogCheckBoxCallback& callback)
{
    _checkBoxEventCallback = callback;
}
    
std::string CheckBox::getDescription() const
{
    return "CheckBox";
}

Widget* CheckBox::createCloneInstance()
{
    return CheckBox::create();
}

void CheckBox::copySpecialProperties(Widget *widget)
{
    CheckBox* checkBox = dynamic_cast<CheckBox*>(widget);
    if (checkBox)
    {
        AbstractCheckButton::copySpecialProperties(widget);
        _checkBoxEventListener = checkBox->_checkBoxEventListener;
        _checkBoxEventCallback = checkBox->_checkBoxEventCallback;
        _ogEventCallback = checkBox->_ogEventCallback;
    }
}

}

OG_END
