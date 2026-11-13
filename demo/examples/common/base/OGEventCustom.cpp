 
#include "base/OGEventCustom.h"
#include "base/OGEvent.h"

OG_BEGIN

EventCustom::EventCustom(const std::string& eventName)
: Event(Type::CUSTOM)
, _userData(nullptr)
, _eventName(eventName)
{
}

OG_END
