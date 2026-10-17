 
#ifndef __orange_libs__OGEventFocus__
#define __orange_libs__OGEventFocus__

#include "base/OGEvent.h"
 
OG_BEGIN

namespace ui {
    class Widget;
}
 
class   EventFocus : public Event
{
public: 
    EventFocus(ui::Widget* widgetLoseFocus, ui::Widget* widgetGetFocus);
    
private:
    ui::Widget *_widgetGetFocus;
    ui::Widget *_widgetLoseFocus;
    
    friend class EventListenerFocus;
};


OG_END

// end of base group
/// @}

#endif /* defined(__orange_libs__OGEventFocus__) */
