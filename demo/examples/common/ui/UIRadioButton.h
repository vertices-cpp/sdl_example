

#ifndef __UIRADIOBUTTON_H__
#define __UIRADIOBUTTON_H__

#include "ui/UIAbstractCheckButton.h"
#include "ui/GUIExport.h"

/**
 * @addtogroup ui
 * @{
 */
OG_BEGIN

namespace ui {

class RadioButtonGroup;

/**
 *  RadioButton is a specific type of two-states button that is similar to CheckBox.
 *  Additionally, it can be used together with RadioButtonGroup to interact with other radio buttons.
 */
class RadioButton : public AbstractCheckButton
{

    DECLARE_CLASS_GUI_INFO
    
    friend class RadioButtonGroup;
    
public:
    /**
     * Radio button event types.
     */
    enum class EventType
    {
        SELECTED,
        UNSELECTED
    };
    
    /**
     * A callback which will be called after certain RadioButton event issue.
     * @see `RadioButton::EventType`
     */
    typedef std::function<void(RadioButton* radioButton, EventType)> ogRadioButtonCallback;
    
    /**
     * Default constructor.
     *
     * @lua new
     */
    RadioButton();
    
    /**
     * Default destructor.
     *
     * @lua NA
     */
    virtual ~RadioButton();
    
    /**
     * Create and return a empty RadioButton instance pointer.
     */
    static RadioButton* create();
    
    /**
     * Create a radio button with various images.
     *
     * @param backGround    backGround texture.
     * @param backGroundSelected    backGround selected state texture.
     * @param cross    cross texture.
     * @param backGroundDisabled    backGround disabled state texture.
     * @param frontCrossDisabled    cross dark state texture.
     * @param texType    @see `Widget::TextureResType`
     *
     * @return A RadioButton instance pointer.
     */
    static RadioButton* create(const std::string& backGround,
                            const std::string& backGroundSelected,
                            const std::string& cross,
                            const std::string& backGroundDisabled,
                            const std::string& frontCrossDisabled,
                            TextureResType texType = TextureResType::LOCAL);
    
    /**
     * Another factory method to create a RadioButton instance.
     * This method uses less resource to create a RadioButton.
     * @param backGround The background image name in `std::string`.
     * @param cross The cross image name in `std::string`.
     * @param texType  The texture's resource type in `Widget::TextureResType`.
     * @return A RadioButton instance pointer
     */
    static RadioButton* create(const std::string& backGround,
                            const std::string& cross,
                            TextureResType texType = TextureResType::LOCAL);
    
    /**
     * Add a callback function which would be called when radio button is selected or unselected.
     *@param callback A std::function with type @see `ogRadioButtonCallback`
     */
    void addEventListener(const ogRadioButtonCallback& callback);
    
    virtual std::string getDescription() const override;
    
protected:
    virtual void releaseUpEvent() override;

    virtual void dispatchSelectChangedEvent(bool selected) override;
    
    virtual Widget* createCloneInstance() override;
    virtual void copySpecialProperties(Widget* model) override;
    
    ogRadioButtonCallback _radioButtonEventCallback;
    RadioButtonGroup* _group;
    
};

/**
 *  RadioButtonGroup groups designated radio buttons to make them interact to each other.
 *  In one RadioButtonGroup, only one or no RadioButton can be checked.
 */
class RadioButtonGroup : public Widget
{
    friend class RadioButton;
public:
    /**
     * Radio button group event types.
     */
    enum class EventType
    {
        SELECT_CHANGED,
    };
    
    /**
     * A callback which will be called after RadioButtonGroup event issue.
     * @see `RadioButtonGroup::EventType`
     */
    typedef std::function<void(RadioButton* radioButton, int index, EventType)> ogRadioButtonGroupCallback;
    
    /**
     * Default constructor.
     *
     * @lua new
     */
    RadioButtonGroup();
    
    /**
     * Default destructor.
     *
     * @lua NA
     */
    virtual ~RadioButtonGroup();
    
    /**
     * Create and return a empty RadioButtonGroup instance pointer.
     */
    static RadioButtonGroup* create();
    
    /**
     * Add a callback function which would be called when radio button is selected or unselected.
     *@param callback A std::function with type @see `ogRadioButtonGroupCallback`
     */
    void addEventListener(const ogRadioButtonGroupCallback& callback);
    
    /**
     * Get the index of selected radio button.
     *
     * @return the selected button's index. Returns -1 if no button is selected.
     */
    virtual int getSelectedButtonIndex() const;
    
    /**
     * Select a radio button by index.
     *
     * @param index of the radio button
     */
    virtual void setSelectedButton(int index);
    
    /**
     * Select a radio button by instance.
     *
     * @param radio button instance
     */
    virtual void setSelectedButton(RadioButton* radioButton);
    
    /**
     * Select a radio button by index without event dispatch.
     *
     * @param index of the radio button
     */
    virtual void setSelectedButtonWithoutEvent(int index);
    
    /**
     * Select a radio button by instance without event dispatch.
     *
     * @param radio button instance
     */
    virtual void setSelectedButtonWithoutEvent(RadioButton* radioButton);
    
    /**
     * Add a radio button into this group.
     *
     * @param radio button instance
     */
    virtual void addRadioButton(RadioButton* radioButton);
    
    /**
     * Remove a radio button from this group.
     *
     * @param radio button instance
     */
    virtual void removeRadioButton(RadioButton* radioButton);
    
    /**
     * Remove all radio button from this group.
     */
    virtual void removeAllRadioButtons();
    
    /**
     * Get the number of radio buttons in this group.
     *
     * @return the number of radio buttons in this group
     */
    ssize_t getNumberOfRadioButtons() const;
    
    /**
     * Get a radio button in this group by index.
     *
     * @param index of the radio button
     * @return radio button instance. Returns nullptr if out of index.
     */
    RadioButton* getRadioButtonByIndex(int index) const;
    
    /**
     * Set a flag for allowing no-selection feature.
     * If it is allowed, no radio button can be selected.
     * If it is not allowed, one radio button must be selected all time except it is empty.
     * Default is not allowed.
     *
     * @param true means allowing no-selection, false means disallowing no-selection.
     */
    void setAllowedNoSelection(bool allowedNoSelection);
    
    /**
     * Query whether no-selection is allowed or not.
     *
     * @param true means no-selection is allowed, false means no-selection is not allowed.
     */
    bool isAllowedNoSelection() const;
    
    virtual std::string getDescription() const override;
    
protected:
    virtual Widget* createCloneInstance() override;
    virtual void copySpecialProperties(Widget* model) override;
    
    void onChangedRadioButtonSelect(RadioButton* radioButton);
    void deselect();
    
    Vector<RadioButton*> _radioButtons;
    ogRadioButtonGroupCallback _radioButtonGroupEventCallback;
    RadioButton* _selectedRadioButton;
    bool _allowedNoSelection;
    
};
    
}

OG_END
// end of ui group
/// @}

#endif /* defined(__UIRADIOBUTTON_H__) */
