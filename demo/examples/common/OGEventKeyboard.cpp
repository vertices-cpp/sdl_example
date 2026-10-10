 
#include "OGEventKeyboard.h"

OG_BEGIN

EventKeyboard::EventKeyboard(KeyCode keyCode, bool isPressed)
: Event(Type::KEYBOARD)
, _keyCode(keyCode)
, _isPressed(isPressed)
{}

OG_END
