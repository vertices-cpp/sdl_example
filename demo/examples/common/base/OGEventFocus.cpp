 
#include "base/OGEventFocus.h"

OG_BEGIN


EventFocus::EventFocus(ui::Widget *widgetLoseFocus, ui::Widget* widgetGetFocus)
:Event(Type::FOCUS),
_widgetGetFocus(widgetGetFocus),
_widgetLoseFocus(widgetLoseFocus)
{
    
}




OG_END
