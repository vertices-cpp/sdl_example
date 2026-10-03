 

#ifndef __OG_IME_DELEGATE_H__
#define __OG_IME_DELEGATE_H__

#include <string>
#include "OGGeometry.h"
#include "OGEventDispatcher.h"

/**
 * @addtogroup base
 * @{
 */
OG_BEGIN

/**
 * A static global empty std::string install.
 */
	extern const std::string   STD_STRING_EMPTY;


/**
 * Keyboard notification event type.
 */
typedef struct
{
    Rect  begin;              // the soft keyboard rectangle when animation begins
    Rect  end;                // the soft keyboard rectangle when animation ends
    float     duration;           // the soft keyboard animation duration
} IMEKeyboardNotificationInfo;

/**
 *@brief    Input method editor delegate.
 */
 

class   IMEDelegate
{
public: 
    virtual ~IMEDelegate(); 
    virtual bool attachWithIME(); 
    virtual bool detachWithIME();

protected:
    friend class IMEDispatcher; 
    virtual bool canAttachWithIME() { return false; } 
    virtual void didAttachWithIME() {} 
    virtual bool canDetachWithIME() { return false; } 
    virtual void didDetachWithIME() {} 
    virtual void insertText(const char* /*text*/, size_t /*len*/) {} 
    virtual void deleteBackward() {} 
    virtual void controlKey(EventKeyboard::KeyCode /*keyCode*/) {} 
 
    virtual const std::string& getContentText() { return STD_STRING_EMPTY; }
    virtual void keyboardWillShow(IMEKeyboardNotificationInfo& /*info*/)   {} 
    virtual void keyboardDidShow(IMEKeyboardNotificationInfo& /*info*/)    {} 
    virtual void keyboardWillHide(IMEKeyboardNotificationInfo& /*info*/)   {} 
    virtual void keyboardDidHide(IMEKeyboardNotificationInfo& /*info*/)    {}

protected:
    /**
     * @js NA
     * @lua NA
     */
    IMEDelegate();
};


OG_END
// end of base group
/// @}

#endif    // __CC_IME_DELEGATE_H__
