 
#include "base/OGEventListenerCustom.h"
#include "base/OGEventCustom.h"

OG_BEGIN

EventListenerCustom::EventListenerCustom()
: _onCustomEvent(nullptr)
{
}

EventListenerCustom* EventListenerCustom::create(const std::string& eventName, const std::function<void(EventCustom*)>& callback)
{
    EventListenerCustom* ret = new (std::nothrow) EventListenerCustom();
    if (ret && ret->init(eventName, callback))
    {
        ret->autorelease();
    }
    else
    {
       OG_SAFE_DELETE(ret);
    }
    return ret;
}

bool EventListenerCustom::init(const ListenerID& listenerId, const std::function<void(EventCustom*)>& callback)
{
    bool ret = false;
    
    _onCustomEvent = callback;
    
    auto listener = [this](Event* event){
        if (_onCustomEvent != nullptr)
        {
            _onCustomEvent(static_cast<EventCustom*>(event));
        }
    };
    
    if (EventListener::init(EventListener::Type::CUSTOM, listenerId, listener))
    {
        ret = true;
    }
    return ret;
}

EventListenerCustom* EventListenerCustom::clone()
{
    EventListenerCustom* ret = new (std::nothrow) EventListenerCustom();
    if (ret && ret->init(_listenerID, _onCustomEvent))
    {
        ret->autorelease();
    }
    else
    {
		OG_SAFE_DELETE(ret);
    }
    return ret;
}

bool EventListenerCustom::checkAvailable()
{
    bool ret = false;
    if (EventListener::checkAvailable() && _onCustomEvent != nullptr)
    {
        ret = true;
    }
    return ret;
}

OG_END
