 
#ifndef __OGEVENTLISTENER_H__
#define __OGEVENTLISTENER_H__

#include <functional>
#include <string>
#include <memory>

#include "OGPlatformMacros.h"
#include "OGRef.h"
 
OG_BEGIN

class Event;
class Node;
 
class   EventListener : public Ref
{
public:
    /** Type Event type.*/
    enum class Type
    {
        UNKNOWN,
        TOUCH_ONE_BY_ONE,
        TOUCH_ALL_AT_ONCE,
        KEYBOARD,
        MOUSE,
        ACCELERATION,
        FOCUS,
		GAME_CONTROLLER,
        CUSTOM
    };

    typedef std::string ListenerID;

public: 
    EventListener(); 
    virtual ~EventListener(); 

	bool init(Type t, const ListenerID& listenerID, const std::function<void(Event*)>& callback);
    virtual bool checkAvailable() = 0; 
    virtual EventListener* clone() = 0; 
    void setEnabled(bool enabled) { _isEnabled = enabled; } 
    bool isEnabled() const { return _isEnabled; }

protected: 
    void setPaused(bool paused) { _paused = paused; } 
    bool isPaused() const { return _paused; } 
    void setRegistered(bool registered) { _isRegistered = registered; } 
    bool isRegistered() const { return _isRegistered; } 
    Type getType() const { return _type; } 
    const ListenerID& getListenerID() const { return _listenerID; } 
    void setFixedPriority(int fixedPriority) { _fixedPriority = fixedPriority; }
	 
    int getFixedPriority() const { return _fixedPriority; } 
    void setAssociatedNode(Node* node) { _node = node; } 
    Node* getAssociatedNode() const { return _node; } 
    std::function<void(Event*)> _onEvent;   /// Event callback function

    Type _type;                             /// Event listener type
    ListenerID _listenerID;                 /// Event listener ID
    bool _isRegistered;                     /// Whether the listener has been added to dispatcher.

    int   _fixedPriority;   // The higher the number, the higher the priority, 0 is for scene graph base priority.
    Node* _node;            // scene graph based priority
    bool _paused;           // Whether the listener is paused
    bool _isEnabled;        // Whether the listener is enabled
    friend class EventDispatcher;
};

OG_END

// end of base group
/// @}

#endif // __OGEVENTLISTENER_H__
