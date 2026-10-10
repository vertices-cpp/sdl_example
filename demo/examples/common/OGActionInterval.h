 

#ifndef _OG_ACTION_INTERVAL_H_
#define _OG_ACTION_INTERVAL_H_

#include <vector>

#include "OGAction.h"
#include "OGAnimation.h"
#include "OGProtocols.h"
#include "OGVector.h"

OG_BEGIN

class Node;
class SpriteFrame;
class EventCustom;

/**
 * @addtogroup actions
 * @{
 */

/** @class ActionInterval
@brief An interval action is an action that takes place within a certain period of time.
It has an start time, and a finish time. The finish time is the parameter
duration plus the start time.

These ActionInterval actions have some interesting properties, like:
- They can run normally (default)
- They can run reversed with the reverse method
- They can run with the time altered with the Accelerate, AccelDeccel and Speed actions.

For example, you can simulate a Ping Pong effect running the action normally and
then running it again in Reverse mode.

Example:

@code
auto action = MoveBy::create(1.0f, Vec2::ONE);
auto pingPongAction = Sequence::create(action, action->reverse(), nullptr);
@endcode
*/
class   ActionInterval : public FiniteTimeAction
{
public: 
    float getElapsed() { return _elapsed; } 
    void setAmplitudeRate(float amp); 
    float getAmplitudeRate();

    //
    // Overrides
    //
    virtual bool isDone() const override;
    /**
     * @param dt in seconds
     */
    virtual void step(float dt) override;
    virtual void startWithTarget(Node *target) override;
    virtual ActionInterval* reverse() const override
    {
        OG_ASSERT(0);
        return nullptr;
    }

    virtual ActionInterval *clone() const override
    {
        OG_ASSERT(0);
        return nullptr;
    }

public:
    /** initializes the action */
    bool initWithDuration(float d);

protected:
    float _elapsed;
    bool _firstTick;
    bool _done;
    
protected:
   // bool sendUpdateEventToScript(float dt, Action *actionObject);
};

/** @class ScaleTo
 @brief Scales a Node object to a zoom factor by modifying it's scale attribute.
 @warning This action doesn't support "reverse".
 @warning The physics body contained in Node doesn't support this action.
 */
class   ScaleTo : public ActionInterval
{
public: 
    static ScaleTo* create(float duration, float s); 
    static ScaleTo* create(float duration, float sx, float sy); 
    static ScaleTo* create(float duration, float sx, float sy, float sz);

    //
    // Overrides
    //
    virtual ScaleTo* clone() const override;
    virtual ScaleTo* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
public:
    ScaleTo() {}
    virtual ~ScaleTo() {}

    /** 
     * initializes the action with the same scale factor for X and Y
     * @param duration in seconds
     */
    bool initWithDuration(float duration, float s);
    /** 
     * initializes the action with and X factor and a Y factor 
     * @param duration in seconds
     */
    bool initWithDuration(float duration, float sx, float sy);
    /** 
     * initializes the action with X Y Z factor 
     * @param duration in seconds
     */
    bool initWithDuration(float duration, float sx, float sy, float sz);

protected:
    float _scaleX;
    float _scaleY;
  //  float _scaleZ;
    float _startScaleX;
    float _startScaleY;
  //  float _startScaleZ;
    float _endScaleX;
    float _endScaleY;
  //  float _endScaleZ;
    float _deltaX;
    float _deltaY;
  //  float _deltaZ;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(ScaleTo);
};

/** @class ScaleBy
 * @brief Scales a Node object a zoom factor by modifying it's scale attribute.
 @warning The physics body contained in Node doesn't support this action.
*/
class   ScaleBy : public ScaleTo
{
public: 
    static ScaleBy* create(float duration, float s); 
    static ScaleBy* create(float duration, float sx, float sy); 
    static ScaleBy* create(float duration, float sx, float sy, float sz);

    //
    // Overrides
    //
    virtual void startWithTarget(Node *target) override;
    virtual ScaleBy* clone() const override;
    virtual ScaleBy* reverse() const override;

public:
    ScaleBy() {}
    virtual ~ScaleBy() {}

private:
    OG_DISALLOW_COPY_AND_ASSIGN(ScaleBy);
};
 

OG_END

#endif //__ACTION_OGINTERVAL_ACTION_H__
