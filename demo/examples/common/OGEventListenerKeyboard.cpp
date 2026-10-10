

#include "OGEventListenerKeyboard.h"
#include "OGMacros.h"

OG_BEGIN

const std::string EventListenerKeyboard::LISTENER_ID = "__og_keyboard";

bool EventListenerKeyboard::checkAvailable()
{
    if (onKeyPressed == nullptr && onKeyReleased == nullptr)
    {
        OGASSERT(false, "Invalid EventListenerKeyboard!");
        return false;
    }
    
    return true;
}

EventListenerKeyboard* EventListenerKeyboard::create()
{
    auto ret = new (std::nothrow) EventListenerKeyboard();
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        OG_SAFE_DELETE(ret);
    }
    return ret;
}

EventListenerKeyboard* EventListenerKeyboard::clone()
{
    auto ret = new (std::nothrow) EventListenerKeyboard();
    if (ret && ret->init())
    {
        ret->autorelease();
        ret->onKeyPressed = onKeyPressed;
        ret->onKeyReleased = onKeyReleased;
    }
    else
    {
		OG_SAFE_DELETE(ret);
    }
    return ret;
}

EventListenerKeyboard::EventListenerKeyboard()
: onKeyPressed(nullptr)
, onKeyReleased(nullptr)
{
}

bool EventListenerKeyboard::init()
{
    auto listener = [this](Event* event){
        auto keyboardEvent = static_cast<EventKeyboard*>(event);
        if (keyboardEvent->_isPressed)
        {
            if (onKeyPressed != nullptr)
                onKeyPressed(keyboardEvent->_keyCode, event);
        }
        else
        {
            if (onKeyReleased != nullptr)
                onKeyReleased(keyboardEvent->_keyCode, event);
        }
    };
    
    if (EventListener::init(Type::KEYBOARD, LISTENER_ID, listener))
    {
        return true;
    }
    
    return false;
}

OG_END
