 
#ifndef __orange_libs__OGKeyboardEventListener__
#define __orange_libs__OGKeyboardEventListener__

#include "OGEventListener.h"
#include "OGEventKeyboard.h"

/**
 * @addtogroup base
 * @{
 */

OG_BEGIN

class Event;
 
class   EventListenerKeyboard : public EventListener
{
public:
    static const std::string LISTENER_ID; 
    static EventListenerKeyboard* create();
    
    /// Overrides
    virtual EventListenerKeyboard* clone() override;
    virtual bool checkAvailable() override;
    
    std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
    std::function<void(EventKeyboard::KeyCode, Event*)> onKeyReleased;
public:
    EventListenerKeyboard();
    bool init();
};

OG_END

// end of base group
/// @}

#endif /* defined(__orange_libs__OGKeyboardEventListener__) */
