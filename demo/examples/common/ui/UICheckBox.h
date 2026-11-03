

#ifndef _UI_CHECK_BOX_H_
#define _UI_CHECK_BOX_H_

#include "ui/UIAbstractCheckButton.h"
#include "ui/GUIExport.h"

/**
 * @addtogroup ui
 * @{
 */
OG_BEGIN

namespace ui {

/**
 *  Checkbox is a specific type of two-states button that can be either checked or unchecked.
 */
class CheckBox : public AbstractCheckButton
{
    
    DECLARE_CLASS_GUI_INFO
    
public:
    /**
     * CheckBox event type, currently only "selected" and "unselected" event are cared.
     */
    enum class EventType
    {
        SELECTED,
        UNSELECTED
    };
    
    /**
     * A callback which will be called after certain CheckBox event issue.
     * @see `CheckBox::EventType`
     */
    typedef std::function<void(Ref*,CheckBox::EventType)> ogCheckBoxCallback;
    
    /**
     * Default constructor.
     * 
     * @lua new
     */
    CheckBox();

    /**
     * Default destructor.
     * 
     * @lua NA
     */
    virtual ~CheckBox();

    /**
     * Create and return a empty CheckBox instance pointer.
     */
    static CheckBox* create();
    
    static CheckBox* create(const std::string& backGround,
                            const std::string& backGroundSelected,
                            const std::string& cross,
                            const std::string& backGroundDisabled,
                            const std::string& frontCrossDisabled,
                            TextureResType texType = TextureResType::LOCAL);

    static CheckBox* create(const std::string& backGround,
                            const std::string& cross,
                            TextureResType texType = TextureResType::LOCAL);

    /**
     *Add a callback function which would be called when checkbox is selected or unselected.
     *@param callback A std::function with type @see `ogCheckBoxCallback`
     */
    void addEventListener(const ogCheckBoxCallback& callback);


    //override functions
    virtual std::string getDescription() const override;
    
    virtual void onTouchEnded(Touch *touch, Event *unusedEvent) override;
protected:
    
    virtual void dispatchSelectChangedEvent(bool selected) override;
    
    virtual Widget* createCloneInstance() override;
    virtual void copySpecialProperties(Widget* model) override;
    
protected:
    //if you use the old event callback, it will retain the _checkBoxEventListener
    Ref*       _checkBoxEventListener;
    
    ogCheckBoxCallback _checkBoxEventCallback;

};

}

OG_END
// end of ui group
/// @}

#endif /* defined(_UI_CHECK_BOX_H_) */
