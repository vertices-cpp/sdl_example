

#ifndef _OG_EVENT_H_
#define _OG_EVENT_H_

#include "base/OGRef.h"
#include "platform/OGPlatformMacros.h"

/**
 * @addtogroup base
 * @{
 */

OG_BEGIN

class Node;
 
class  Event : public Ref
{
public:
    /** Type Event type.*/
    enum class Type
    {
        TOUCH,
        KEYBOARD,
        ACCELERATION,
        MOUSE,
        FOCUS,
        GAME_CONTROLLER,
        CUSTOM
    };
    
public: 
    Event(Type type);
public: 
    virtual ~Event(); 
    Type getType() const { return _type; } 
    void stopPropagation() { _isStopped = true; } 
    bool isStopped() const { return _isStopped; } 
    Node* getCurrentTarget() { return _currentTarget; }
    
protected:
    /** Sets current target */
    void setCurrentTarget(Node* target) { _currentTarget = target; }
    
	Type _type;     ///< Event type
    
    bool _isStopped;       ///< whether the event has been stopped.
    Node* _currentTarget;  ///< Current target
    
    friend class EventDispatcher;
};

OG_END

// end of base group
/// @}

#endif // _OG_EVENT_H_
