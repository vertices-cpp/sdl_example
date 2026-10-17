

#include "ui/UIRadioButton.h"

OG_BEGIN

namespace ui {
    
IMPLEMENT_CLASS_GUI_INFO(RadioButton)

RadioButton::RadioButton() :
_radioButtonEventCallback(nullptr),
_group(nullptr)
{
}

RadioButton::~RadioButton()
{
    _radioButtonEventCallback = nullptr;
    _group = nullptr;
}

RadioButton* RadioButton::create()
{
    RadioButton* widget = new (std::nothrow) RadioButton();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}

RadioButton* RadioButton::create(const std::string& backGround,
                           const std::string& backGroundSelected,
                           const std::string& cross,
                           const std::string& backGroundDisabled,
                           const std::string& frontCrossDisabled,
                           TextureResType texType)
{
    RadioButton *pWidget = new (std::nothrow) RadioButton;
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

RadioButton* RadioButton::create(const std::string& backGround,
                           const std::string& cross,
                           TextureResType texType)
{
    RadioButton *pWidget = new (std::nothrow) RadioButton;
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

void RadioButton::dispatchSelectChangedEvent(bool selected)
{
    EventType eventType = (selected ? EventType::SELECTED : EventType::UNSELECTED);
    this->retain();
    if (_radioButtonEventCallback)
    {
        _radioButtonEventCallback(this, eventType);
    }
    if (_ogEventCallback)
    {
        _ogEventCallback(this, static_cast<int>(eventType));
    }
    
    if(selected && _group != nullptr)
    {
        _group->onChangedRadioButtonSelect(this);
    }
    this->release();
}

void RadioButton::addEventListener(const ogRadioButtonCallback& callback)
{
    _radioButtonEventCallback = callback;
}

void RadioButton::releaseUpEvent()
{
    Widget::releaseUpEvent();
    
    if (!_isSelected)
    {
        setSelected(true);
        dispatchSelectChangedEvent(true);
    }
}

std::string RadioButton::getDescription() const
{
    return "RadioButton";
}

Widget* RadioButton::createCloneInstance()
{
    return RadioButton::create();
}

void RadioButton::copySpecialProperties(Widget *widget)
{
    RadioButton* radioButton = dynamic_cast<RadioButton*>(widget);
    if (radioButton)
    {
        AbstractCheckButton::copySpecialProperties(widget);
        _radioButtonEventCallback = radioButton->_radioButtonEventCallback;
        _ogEventCallback = radioButton->_ogEventCallback;
        _group = radioButton->_group;
    }
}

RadioButtonGroup::RadioButtonGroup() :
_radioButtonGroupEventCallback(nullptr),
_selectedRadioButton(nullptr),
_allowedNoSelection(false)
{
}

RadioButtonGroup::~RadioButtonGroup()
{
    _radioButtonGroupEventCallback = nullptr;
    _selectedRadioButton = nullptr;
    _radioButtons.clear();
}

RadioButtonGroup* RadioButtonGroup::create()
{
    RadioButtonGroup* widget = new (std::nothrow) RadioButtonGroup();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}

void RadioButtonGroup::addEventListener(const ogRadioButtonGroupCallback& callback)
{
    _radioButtonGroupEventCallback = callback;
}

void RadioButtonGroup::addRadioButton(RadioButton* radioButton)
{
    if(radioButton != nullptr)
    {
        OGASSERT(!radioButton->_group, "It already belongs to a group!");
        radioButton->_group = this;
        _radioButtons.pushBack(radioButton);
        
        if(!_allowedNoSelection && _selectedRadioButton == nullptr)
        {
            setSelectedButtonWithoutEvent(radioButton);
        }
    }
}

void RadioButtonGroup::removeRadioButton(RadioButton* radioButton)
{
    ssize_t index = _radioButtons.getIndex(radioButton);
    if( index == OG_INVALID_INDEX )
    {
        OGLOGERROR("The radio button does not belong to this group!");
        return;
    }
    
    if(radioButton != nullptr)
    {
        radioButton->_group = nullptr;
        if(radioButton == _selectedRadioButton)
        {
            deselect();
        }
        _radioButtons.erase(index);
        
        if(!_allowedNoSelection && _selectedRadioButton == nullptr && !_radioButtons.empty())
        {
            setSelectedButtonWithoutEvent(0);
        }
    }
}

void RadioButtonGroup::removeAllRadioButtons()
{
    while(!_radioButtons.empty())
    {
        removeRadioButton(_radioButtons.at(0));
    }
}

ssize_t RadioButtonGroup::getNumberOfRadioButtons() const
{
    return _radioButtons.size();
}

RadioButton* RadioButtonGroup::getRadioButtonByIndex(int index) const
{
    if(index >= _radioButtons.size())
    {
        OGLOGERROR("Out of array index! length=%d, requestedIndex=%d", (int)_radioButtons.size(), index);
        return nullptr;
    }
    return _radioButtons.at(index);
}

void RadioButtonGroup::deselect()
{
    if(_selectedRadioButton != nullptr)
    {
        _selectedRadioButton->setSelected(false);
        _selectedRadioButton->dispatchSelectChangedEvent(false);
    }
    _selectedRadioButton = nullptr;
}

int RadioButtonGroup::getSelectedButtonIndex() const
{
    return (int) _radioButtons.getIndex(_selectedRadioButton);
}

void RadioButtonGroup::setSelectedButton(int index)
{
    OGASSERT(index < _radioButtons.size(), "Out of array index!");
    setSelectedButton(_radioButtons.at(index));
}

void RadioButtonGroup::setSelectedButton(RadioButton* radioButton)
{
    setSelectedButtonWithoutEvent(radioButton);
    onChangedRadioButtonSelect(_selectedRadioButton);
}

void RadioButtonGroup::setSelectedButtonWithoutEvent(int index)
{
    setSelectedButtonWithoutEvent(_radioButtons.at(index));
}

void RadioButtonGroup::setSelectedButtonWithoutEvent(RadioButton* radioButton)
{
    if(!_allowedNoSelection && radioButton == nullptr)
    {
        return;
    }
    if(_selectedRadioButton == radioButton)
    {
        return;
    }
    if(radioButton != nullptr && !_radioButtons.contains(radioButton))
    {
        OGLOGERROR("The radio button does not belong to this group!");
        return;
    }
    
    deselect();
    _selectedRadioButton = radioButton;
    if(_selectedRadioButton != nullptr)
    {
        _selectedRadioButton->setSelected(true);
    }
}

std::string RadioButtonGroup::getDescription() const
{
    return "RadioButtonGroup";
}

void RadioButtonGroup::setAllowedNoSelection(bool allowedNoSelection)
{
    _allowedNoSelection = allowedNoSelection;
    if(!_allowedNoSelection && _selectedRadioButton == nullptr)
    {
        if (_radioButtons.size() > 0)
        {
            setSelectedButton(_radioButtons.at(0));
        }
    }
}

bool RadioButtonGroup::isAllowedNoSelection() const
{
    return _allowedNoSelection;
}

Widget* RadioButtonGroup::createCloneInstance()
{
    return RadioButtonGroup::create();
}

void RadioButtonGroup::copySpecialProperties(Widget *widget)
{
    RadioButtonGroup* radioButtonGroup = dynamic_cast<RadioButtonGroup*>(widget);
    if (radioButtonGroup)
    {
        _radioButtonGroupEventCallback = radioButtonGroup->_radioButtonGroupEventCallback;
        _ogEventCallback = radioButtonGroup->_ogEventCallback;
        _selectedRadioButton = radioButtonGroup->_selectedRadioButton;
        _allowedNoSelection = radioButtonGroup->_allowedNoSelection;
        
        _radioButtons.clear();
        for(const auto& radioButton : radioButtonGroup->_radioButtons)
        {
            _radioButtons.pushBack(radioButton);
        }
    }
}

void RadioButtonGroup::onChangedRadioButtonSelect(RadioButton* radioButton)
{
    if(_selectedRadioButton != radioButton)
    {
        deselect();
        _selectedRadioButton = radioButton;
    }
    
    this->retain();
    if (_radioButtonGroupEventCallback)
    {
        int index = (int) _radioButtons.getIndex(radioButton);
        _radioButtonGroupEventCallback(_selectedRadioButton, index, EventType::SELECT_CHANGED);
    }
    if (_ogEventCallback)
    {
        _ogEventCallback(this, static_cast<int>(EventType::SELECT_CHANGED));
    }
    this->release();
}
    
}

OG_END
