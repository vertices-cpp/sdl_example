 
#include "OGEventDispatcher.h"

OG_BEGIN

static EventListener::ListenerID __getListenerID(Event* event)
{
	EventListener::ListenerID ret;
	switch (event->getType())
	{
// 	case Event::Type::ACCELERATION:
// 		ret = EventListenerAcceleration::LISTENER_ID;
// 		break;
	case Event::Type::CUSTOM:
	{
		auto customEvent = static_cast<EventCustom*>(event);
		ret = customEvent->getEventName();
	}
	break;
	case Event::Type::KEYBOARD:
		ret = EventListenerKeyboard::LISTENER_ID;
		break;
// 	case Event::Type::MOUSE:
// 		ret = EventListenerMouse::LISTENER_ID;
// 		break;
// 	case Event::Type::FOCUS:
// 		ret = EventListenerFocus::LISTENER_ID;
// 		break;
// 	case Event::Type::TOUCH:
// 		// Touch listener is very special, it contains two kinds of listeners, EventListenerTouchOneByOne and EventListenerTouchAllAtOnce.
// 		// return UNKNOWN instead.
// 		OGASSERT(false, "Don't call this method if the event is for touch.");
// 		break;
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
// 	case Event::Type::GAME_CONTROLLER:
// 		ret = EventListenerController::LISTENER_ID;
// 		break;
// #endif
	default:
		OGASSERT(false, "Invalid type!");
		break;
	}

	return ret;
}


void EventDispatcher::addEvent(EventListener * listener)
{
	_listenerMap.push_back(listener);
}

void EventDispatcher::addEventListenerWithFixedPriority(EventListener* listener, int fixedPriority)
{
	addEvent(listener);
}
void EventDispatcher::removeEventListener(EventListener * listener)
{
	if (listener == nullptr)
		return;

	// just return if listener is in _toRemovedListeners to avoid remove listeners more than once
	auto it = std::find(_listenerMap.begin(), _listenerMap.end(), listener) ;
	if (it != _listenerMap.end())
		return;
	 _listenerMap.erase(it);
}

EventCustom::EventCustom(const std::string & eventName)
	: Event(Type::CUSTOM)
	, _userData(nullptr)
	, _eventName(eventName)
{
}

const std::string EventListenerTouchAllAtOnce::LISTENER_ID = "__og_touch_all_at_once";

OG_END
 

