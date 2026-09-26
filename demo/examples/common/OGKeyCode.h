#ifndef _KEY_CODE_H
#define _KEY_CODE_H
#include <SDL.h>
#include "OGHeader.h"
#include "OGPlatformMacros.h" 
#include "OGEventDispatcher.h"

OG_BEGIN


struct keyCodeItem
{
	int SDLKeyCode;
	EventKeyboard::KeyCode keyCode;
};
// struct MouseKeyCodeItem
// {
// 	int SDLKeyCode;
// 	EventMouse::MouseEventType mouseKeyCode;
// };
// 
// struct MouseButtonCodeItem
// {
// 	int SDLKeyCode;
// 	EventMouse::MouseButton mouseButtonCode;
// };

extern std::unordered_map<int, EventKeyboard::KeyCode> g_keyCodeMap;
extern keyCodeItem g_keyCodeStructArray[117]; 
extern bool AllKeyMap[512];
extern const char* KeyStr[167];
extern const char* GetKeyName(EventKeyboard::KeyCode KeyCode);

OG_END


#endif 