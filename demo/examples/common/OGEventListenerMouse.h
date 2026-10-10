 
#ifndef __orange_libs__OGMouseEventListener__
#define __orange_libs__OGMouseEventListener__

#include "OGEventListener.h"
#include "OGEventMouse.h"

/**
 * @addtogroup base
 * @{
 */

OG_BEGIN

class Event;

/** @class EventListenerMouse
 * @brief Mouse event listener.
 * @js cc._EventListenerMouse
 */
class   EventListenerMouse : public EventListener
{
public:
    static const std::string LISTENER_ID;
    
    /** Create a mouse event listener.
     *
     * @return An autoreleased EventListenerMouse object.
     */
    static EventListenerMouse* create();

    /// Overrides
    virtual EventListenerMouse* clone() override;
    virtual bool checkAvailable() override;

    std::function<void(EventMouse* event)> onMouseDown;
    std::function<void(EventMouse* event)> onMouseUp;
    std::function<void(EventMouse* event)> onMouseMove;
    std::function<void(EventMouse* event)> onMouseScroll;

public:
    EventListenerMouse();
    bool init();
};

OG_END

// end of base group
/// @}

#endif /* defined(__orange_libs__OGMouseEventListener__) */
