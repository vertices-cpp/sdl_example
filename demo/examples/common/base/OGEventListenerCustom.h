 

#ifndef _OG_EVENT_LISTENER_CUSTOM_H_
#define _OG_EVENT_LISTENER_CUSTOM_H_

#include "base/OGEventListener.h"

/**
 * @addtogroup base
 * @{
 */

OG_BEGIN

class EventCustom; 
class   EventListenerCustom : public EventListener
{
public:
    /** Creates an event listener with type and callback.
     * @param eventName The type of the event.
     * @param callback The callback function when the specified event was emitted.
     * @return An autoreleased EventListenerCustom object.
     */
    static EventListenerCustom* create(const std::string& eventName, const std::function<void(EventCustom*)>& callback);
    
    /// Overrides
    virtual bool checkAvailable() override;
    virtual EventListenerCustom* clone() override;
    
public:
    /** Constructor */
    EventListenerCustom();
    
    /** Initializes event with type and callback function */
    bool init(const ListenerID& listenerId, const std::function<void(EventCustom*)>& callback);
    
protected:
    std::function<void(EventCustom*)> _onCustomEvent;
    
    friend class LuaEventListenerCustom;
};

OG_END

// end of base group
/// @}

#endif /* defined(_OG_EVENT_LISTENER_CUSTOM_H_) */
