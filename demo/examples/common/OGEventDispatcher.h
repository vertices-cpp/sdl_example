#ifndef _EVENT_DISPATCHER_H_
#define _EVENT_DISPATCHER_H_

#include "OGRef.h"

OG_BEGIN

class   Event : public Ref
{
public:
	enum Type
	{
		UNKNOWN,
		KEYBOARD,
		MOUSE,
		FOCUS,
		CUSTOM,
		TOUCH
	};

	Event(Type type) :_type(type)
	{
	}

	virtual ~Event()
	{
	}
	Type getType() const {
		return _type;
	}
	Type _type;
};

class   EventKeyboard : public Event
{
public:
 
	enum class KeyCode
	{
		KEY_NONE,
		KEY_PAUSE,
		KEY_SCROLL_LOCK,
		KEY_PRINT,
		KEY_SYSREQ,
		KEY_BREAK,
		KEY_ESCAPE,
		KEY_BACK = KEY_ESCAPE,
		KEY_BACKSPACE,
		KEY_TAB,
		KEY_BACK_TAB,
		KEY_RETURN,
		KEY_CAPS_LOCK,
		KEY_SHIFT,
		KEY_LEFT_SHIFT = KEY_SHIFT,
		KEY_RIGHT_SHIFT,
		KEY_CTRL,
		KEY_LEFT_CTRL = KEY_CTRL,
		KEY_RIGHT_CTRL,
		KEY_ALT,
		KEY_LEFT_ALT = KEY_ALT,
		KEY_RIGHT_ALT,
		KEY_MENU,
		KEY_HYPER,
		KEY_INSERT,
		KEY_HOME,
		KEY_PG_UP,
		KEY_DELETE,
		KEY_END,
		KEY_PG_DOWN,
		KEY_LEFT_ARROW,
		KEY_RIGHT_ARROW,
		KEY_UP_ARROW,
		KEY_DOWN_ARROW,
		KEY_NUM_LOCK,
		KEY_KP_PLUS,
		KEY_KP_MINUS,
		KEY_KP_MULTIPLY,
		KEY_KP_DIVIDE,
		KEY_KP_ENTER,
		KEY_KP_HOME,
		KEY_KP_UP,
		KEY_KP_PG_UP,
		KEY_KP_LEFT,
		KEY_KP_FIVE,
		KEY_KP_RIGHT,
		KEY_KP_END,
		KEY_KP_DOWN,
		KEY_KP_PG_DOWN,
		KEY_KP_INSERT,
		KEY_KP_DELETE,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_SPACE,
		KEY_EXCLAM,
		KEY_QUOTE,
		KEY_NUMBER,
		KEY_DOLLAR,
		KEY_PERCENT,
		KEY_CIRCUMFLEX,
		KEY_AMPERSAND,
		KEY_APOSTROPHE,
		KEY_LEFT_PARENTHESIS,
		KEY_RIGHT_PARENTHESIS,
		KEY_ASTERISK,
		KEY_PLUS,
		KEY_COMMA,
		KEY_MINUS,
		KEY_PERIOD,
		KEY_SLASH,
		KEY_0,
		KEY_1,
		KEY_2,
		KEY_3,
		KEY_4,
		KEY_5,
		KEY_6,
		KEY_7,
		KEY_8,
		KEY_9,
		KEY_COLON,
		KEY_SEMICOLON,
		KEY_LESS_THAN,
		KEY_EQUAL,
		KEY_GREATER_THAN,
		KEY_QUESTION,
		KEY_AT,
		KEY_CAPITAL_A,
		KEY_CAPITAL_B,
		KEY_CAPITAL_C,
		KEY_CAPITAL_D,
		KEY_CAPITAL_E,
		KEY_CAPITAL_F,
		KEY_CAPITAL_G,
		KEY_CAPITAL_H,
		KEY_CAPITAL_I,
		KEY_CAPITAL_J,
		KEY_CAPITAL_K,
		KEY_CAPITAL_L,
		KEY_CAPITAL_M,
		KEY_CAPITAL_N,
		KEY_CAPITAL_O,
		KEY_CAPITAL_P,
		KEY_CAPITAL_Q,
		KEY_CAPITAL_R,
		KEY_CAPITAL_S,
		KEY_CAPITAL_T,
		KEY_CAPITAL_U,
		KEY_CAPITAL_V,
		KEY_CAPITAL_W,
		KEY_CAPITAL_X,
		KEY_CAPITAL_Y,
		KEY_CAPITAL_Z,
		KEY_LEFT_BRACKET,
		KEY_BACK_SLASH,
		KEY_RIGHT_BRACKET,
		KEY_UNDERSCORE,
		KEY_GRAVE,
		KEY_A,
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z,
		KEY_LEFT_BRACE,
		KEY_BAR,
		KEY_RIGHT_BRACE,
		KEY_TILDE,
		KEY_EURO,
		KEY_POUND,
		KEY_YEN,
		KEY_MIDDLE_DOT,
		KEY_SEARCH,
		KEY_DPAD_LEFT,
		KEY_DPAD_RIGHT,
		KEY_DPAD_UP,
		KEY_DPAD_DOWN,
		KEY_DPAD_CENTER,
		KEY_ENTER,
		KEY_PLAY,
		KEY_KP_PERIOD	//添加小键盘的英文句话
	};

	 EventKeyboard(KeyCode keyCode, bool isPressed)
		: Event(Type::KEYBOARD)
		, _keyCode(keyCode)
		, _isPressed(isPressed)
	{
	}

	EventKeyboard() = default;
private:
	KeyCode _keyCode;
	bool _isPressed;
	friend class EventListenerKeyboard;
};
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

class EventListener :public Ref
{
public:
	/** Type Event type.*/
	enum class Type
	{
		UNKNOWN,
		KEYBOARD,
		MOUSE,
		TOUCH_ONE_BY_ONE,
		TOUCH_ALL_AT_ONCE,
		CUSTOM
	};
	typedef std::string ListenerID;
	
	bool init(Type t, const ListenerID& listenerID, const std::function<void(Event*)>& callback)
	{
		_onEvent = callback;
		_type = t;
		_listenerID = listenerID;
		_isRegistered = false;
		_paused = false;
		_isEnabled = true;

		return true;
	}
	Type getType() const { return _type; }
	std::function<void(Event*)> _onEvent;   /// Event callback function
protected:
	Type _type;                             /// Event listener type
	ListenerID _listenerID;                 /// Event listener ID
	bool _isRegistered;                     /// Whether the listener has been added to dispatcher.
	 
	bool _paused;           // Whether the listener is paused
	bool _isEnabled;        // Whether the listener is enabled
	friend class EventDispatcher;
};
class  EventListenerKeyboard : public EventListener
{
public:
	static const std::string LISTENER_ID;

	CREATE_FUNC(EventListenerKeyboard);

	bool init() {
		auto listener = [this](Event* event) {
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
	}
 

	//包含两个事件回调的调用
	std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
	std::function<void(EventKeyboard::KeyCode, Event*)> onKeyReleased;

	EventListenerKeyboard()
		: onKeyPressed(nullptr)
		, onKeyReleased(nullptr)
	{
	}

};
class   EventListenerCustom : public EventListener
{
public:
	/** Creates an event listener with type and callback.
	 * @param eventName The type of the event.
	 * @param callback The callback function when the specified event was emitted.
	 * @return An autoreleased EventListenerCustom object.
	 */
	static EventListenerCustom*  create(const std::string& eventName, const std::function<void(EventCustom*)>& callback)
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

	 
	/** Constructor */
	 EventListenerCustom()
		: _onCustomEvent(nullptr)
	{
	}

	/** Initializes event with type and callback function */
	 bool  init(const ListenerID& listenerId, const std::function<void(EventCustom*)>& callback)
	 {
		 bool ret = false;

		 _onCustomEvent = callback;

		 auto listener = [this](Event* event) {
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

protected:
	std::function<void(EventCustom*)> _onCustomEvent;

	friend class LuaEventListenerCustom;
};
class   EventDispatcher : public Ref {
public:
	Instance(EventDispatcher);
	void addEvent(EventListener * event)
	{
		_event = event;
	}
	void  dispatchEvent(Event* event)
	{
		if (event->getType() == Event::Type::KEYBOARD && _event) {
			_event->_onEvent(event);
		}
	}
	void  dispatchCustomEvent(const std::string &eventName, void *optionalUserData)
	{
		EventCustom ev(eventName);
		ev.setUserData(optionalUserData);
		dispatchEvent(&ev);
	}
	EventListener *_event = nullptr;
};

OG_END

#endif