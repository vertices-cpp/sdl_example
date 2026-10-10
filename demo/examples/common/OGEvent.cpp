#include "OGEvent.h"

OG_BEGIN

Event::Event(Type type)
: _type(type)
, _isStopped(false)
, _currentTarget(nullptr)
{
}

Event::~Event()
{
}


OG_END
