
#ifndef __OG_IME_DISPATCHER_H__
#define __OG_IME_DISPATCHER_H__

#include "OGIMEDelegate.h"

/**
 * @addtogroup base
 * @{
 */
OG_BEGIN


/**
@brief    Input Method Edit Message Dispatcher.
*/
class   IMEDispatcher
{
public: 
    ~IMEDispatcher(); 
    static IMEDispatcher* sharedDispatcher(); 
    void dispatchInsertText(const char * text, size_t len); 
    void dispatchDeleteBackward(); 
    void dispatchControlKey(EventKeyboard::KeyCode keyCode); 
    const std::string& getContentText(); 
    void dispatchKeyboardWillShow(IMEKeyboardNotificationInfo& info); 
    void dispatchKeyboardDidShow(IMEKeyboardNotificationInfo& info); 
    void dispatchKeyboardWillHide(IMEKeyboardNotificationInfo& info); 
    void dispatchKeyboardDidHide(IMEKeyboardNotificationInfo& info);

protected:
    friend class IMEDelegate; 
    void addDelegate(IMEDelegate * delegate); 
    bool attachDelegateWithIME(IMEDelegate * delegate); 
    bool detachDelegateWithIME(IMEDelegate * delegate); 
    void removeDelegate(IMEDelegate * delegate);

private:
    IMEDispatcher();
    
    class Impl;
    Impl * _impl;
};


OG_END
// end of base group
/// @}

#endif    // __CC_IME_DISPATCHER_H__
