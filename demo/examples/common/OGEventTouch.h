 
#ifndef __orange_libs__TouchEvent__
#define __orange_libs__TouchEvent__

#include "OGEvent.h"
#include <vector>
 
OG_BEGIN

class Touch;

#define TOUCH_PERF_DEBUG 1
 
class   EventTouch : public Event
{
public:
    static const int MAX_TOUCHES = 15; 
    enum class EventCode
    {
        BEGAN,
        MOVED,
        ENDED,
        CANCELLED
    }; 
    EventTouch(); 
    EventCode getEventCode() const { return _eventCode; } 
    const std::vector<Touch*>& getTouches() const { return _touches; }

#if TOUCH_PERF_DEBUG 
    void setEventCode(EventCode eventCode) { _eventCode = eventCode; }; 
    void setTouches(const std::vector<Touch*>& touches) { _touches = touches; };
#endif
    
private:
    EventCode _eventCode;
    std::vector<Touch*> _touches;

    friend class SDLView;
};


OG_END
 

#endif /* defined(__orange_libs__TouchEvent__) */
