 
#ifndef _OG_EVENT_FOCUS_H_
#define _OG_EVENT_FOCUS_H_

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

#endif /* defined(_OG_EVENT_FOCUS_H_) */
