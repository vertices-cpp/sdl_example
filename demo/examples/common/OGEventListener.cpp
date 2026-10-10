 
#include "OGEventListener.h"
//#include "OGConsole.h"

OG_BEGIN

EventListener::EventListener()
{}
    
EventListener::~EventListener() 
{
	OGLOGINFO("In the destructor of EventListener. %p", this);
}

bool EventListener::init(Type t, const ListenerID& listenerID, const std::function<void(Event*)>& callback)
{
    _onEvent = callback;
    _type = t;
    _listenerID = listenerID;
    _isRegistered = false;
    _paused = false;
    _isEnabled = true;
    
    return true;
}

bool EventListener::checkAvailable()
{ 
	return (_onEvent != nullptr);
}

OG_END
