 
#ifndef _OG_EVENT_LISTENER_FOCUS_H_
#define _OG_EVENT_LISTENER_FOCUS_H_

#include "base/OGEventListener.h"

/**
 * @addtogroup base
 * @{
 */

OG_BEGIN

namespace ui {
    class Widget;
}
 
class   EventListenerFocus : public EventListener
{
public:
    static const std::string LISTENER_ID;
    
    /** Create a focus event listener.
     *
     * @return An autoreleased EventListenerFocus object.
     */
    static EventListenerFocus* create();
    
    /** Destructor.
     * @js NA
     */
    virtual ~EventListenerFocus();
    
    /// Overrides
    virtual EventListenerFocus* clone() override;
    virtual bool checkAvailable() override;
    //
    
public:
    std::function<void(ui::Widget*, ui::Widget*)> onFocusChanged;
    
public:
    EventListenerFocus();
    bool init();
    
    friend class EventDispatcher;
};

OG_END

// end of base group
/// @}

#endif /* defined(_OG_EVENT_LISTENER_FOCUS_H_) */
