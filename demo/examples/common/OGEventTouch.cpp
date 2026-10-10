
#include "OGEventTouch.h"
#include "OGTouch.h"

OG_BEGIN

EventTouch::EventTouch()
: Event(Type::TOUCH)
{
    _touches.reserve(MAX_TOUCHES);
}

OG_END
