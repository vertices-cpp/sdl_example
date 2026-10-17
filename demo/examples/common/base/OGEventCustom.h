
#ifndef __orange_libs__OGCustomEvent__
#define __orange_libs__OGCustomEvent__

#include <string>
#include "base/OGEvent.h"

/**
 * @addtogroup base
 * @{
 */

OG_BEGIN
 
class   EventCustom : public Event
{
public: 
    EventCustom(const std::string& eventName); 
    void setUserData(void* data) { _userData = data; } 
    void* getUserData() const { return _userData; } 
    const std::string& getEventName() const { return _eventName; }
protected:
    void* _userData;       ///< User data
    std::string _eventName;
};

OG_END
 

#endif /* defined(__orange_libs__OGCustomEvent__) */
