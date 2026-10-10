 
#ifndef __orange_libs__OGTouchEventListener__
#define __orange_libs__OGTouchEventListener__

#include "OGEventListener.h"
#include <vector>

/**
 * @addtogroup base
 * @{
 */

OG_BEGIN

class Touch;

/** @class EventListenerTouchOneByOne
 * @brief Single touch event listener.
 * @js cc._EventListenerTouchOneByOne
 */
class   EventListenerTouchOneByOne : public EventListener
{
public:
    static const std::string LISTENER_ID;
    
    /** Create a one by one touch event listener.
     */
    static EventListenerTouchOneByOne* create();
    
    /**
     * Destructor.
     * @js NA
     */
    virtual ~EventListenerTouchOneByOne();
    
    /** Whether or not to swall touches.
     *
     * @param needSwallow True if needs to swall touches.
     */
    void setSwallowTouches(bool needSwallow);
    /** Is swall touches or not.
     *
     * @return True if needs to swall touches.
     */
    bool isSwallowTouches();
    
    /// Overrides
    virtual EventListenerTouchOneByOne* clone() override;
    virtual bool checkAvailable() override;
    //

public:

    typedef std::function<bool(Touch*, Event*)> ogTouchBeganCallback;
    typedef std::function<void(Touch*, Event*)> ogTouchCallback;

	ogTouchBeganCallback onTouchBegan;
	ogTouchCallback onTouchMoved;
	ogTouchCallback onTouchEnded;
	ogTouchCallback onTouchCancelled;
    
public:
    EventListenerTouchOneByOne();
    bool init();
    
private:
    std::vector<Touch*> _claimedTouches;
    bool _needSwallow;
    
    friend class EventDispatcher;
};

/** @class EventListenerTouchAllAtOnce
 * @brief Multiple touches event listener.
 */
class   EventListenerTouchAllAtOnce : public EventListener
{
public:
    static const std::string LISTENER_ID;
    
    /** Create a all at once event listener.
     *
     * @return An autoreleased EventListenerTouchAllAtOnce object.
     */
    static EventListenerTouchAllAtOnce* create();
    /** Destructor.
     * @js NA
     */
    virtual ~EventListenerTouchAllAtOnce();
    
    /// Overrides
    virtual EventListenerTouchAllAtOnce* clone() override;
    virtual bool checkAvailable() override;
    //
public:

    typedef std::function<void(const std::vector<Touch*>&, Event*)>  ogTouchesCallback;

    ogTouchesCallback onTouchesBegan;
	ogTouchesCallback onTouchesMoved;
	ogTouchesCallback onTouchesEnded;
	ogTouchesCallback onTouchesCancelled;
    
public:
    EventListenerTouchAllAtOnce();
    bool init();
private:
    
    friend class EventDispatcher;
};

OG_END

// end of base group
/// @}

#endif /* defined(__orange_libs__OGTouchEventListener__) */
