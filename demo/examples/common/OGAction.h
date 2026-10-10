 
#ifndef _OG_ACTION_H_
#define _OG_ACTION_H_

#include "OGRef.h"
#include "OGGeometry.h"
//#include "OGScriptSupport.h"

OG_BEGIN

class Node;

enum {
    kActionUpdate
};

/**
 * @addtogroup actions
 * @{
 */

/** 
 * @brief Base class for Action objects.
 */
class   Action : public Ref, public Clonable
{
public:
    /** Default tag used for all the actions. */
    static const int INVALID_TAG = -1;
    /**
     * @js NA
     * @lua NA
     */
    virtual std::string description() const;

    /** Returns a clone of action.
     *
     * @return A clone action.
     */
    virtual Action* clone() const
    {
        OG_ASSERT(0);
        return nullptr;
    }

    /** Returns a new action that performs the exact reverse of the action. 
     *
     * @return A new action that performs the exact reverse of the action.
     * @js NA
     */
    virtual Action* reverse() const
    {
        OG_ASSERT(0);
        return nullptr;
    }

    /** Return true if the action has finished. 
     * 
     * @return Is true if the action has finished.
     */
    virtual bool isDone() const;

    /** Called before the action start. It will also set the target. 
     *
     * @param target A certain target.
     */
    virtual void startWithTarget(Node *target);

    /** 
     * Called after the action has finished. It will set the 'target' to nil.
     * IMPORTANT: You should never call "Action::stop()" manually. Instead, use: "target->stopAction(action);".
     */
    virtual void stop();

    /** Called every frame with it's delta time, dt in seconds. DON'T override unless you know what you are doing. 
     *
     * @param dt In seconds.
     */
    virtual void step(float dt);

    /** 
     * Called once per frame. time a value between 0 and 1.

     * For example:
     * - 0 Means that the action just started.
     * - 0.5 Means that the action is in the middle.
     * - 1 Means that the action is over.
     *
     * @param time A value between 0 and 1.
     */
    virtual void update(float time);
    /** Return certain target.
     *
     * @return A certain target.
     */
    Node* getTarget() const { return _target; }
    /** The action will modify the target properties. 
     *
     * @param target A certain target.
     */
    void setTarget(Node *target) { _target = target; }
    /** Return a original Target. 
     *
     * @return A original Target.
     */
    Node* getOriginalTarget() const { return _originalTarget; }
    /** 
     * Set the original target, since target can be nil.
     * Is the target that were used to run the action. Unless you are doing something complex, like ActionManager, you should NOT call this method.
     * The target is 'assigned', it is not 'retained'.
     * @since v0.8.2
     *
     * @param originalTarget Is 'assigned', it is not 'retained'.
     */
    void setOriginalTarget(Node *originalTarget) { _originalTarget = originalTarget; }
    /** Returns a tag that is used to identify the action easily. 
     *
     * @return A tag.
     */
    int getTag() const { return _tag; }
    /** Changes the tag that is used to identify the action easily. 
     *
     * @param tag Used to identify the action easily.
     */
    void setTag(int tag) { _tag = tag; }
    /** Returns a flag field that is used to group the actions easily.
     *
     * @return A tag.
     */
    unsigned int getFlags() const { return _flags; }
    /** Changes the flag field that is used to group the actions easily.
     *
     * @param flags Used to group the actions easily.
     */
    void setFlags(unsigned int flags) { _flags = flags; }

public:
    Action();
    virtual ~Action();

protected:
    Node    *_originalTarget;
    /** 
     * The "target".
     * The target will be set with the 'startWithTarget' method.
     * When the 'stop' method is called, target will be set to nil.
     * The target is 'assigned', it is not 'retained'.
     */
    Node    *_target;
    /** The action tag. An identifier of the action. */
    int     _tag;
    /** The action flag field. To categorize action into certain groups.*/
    unsigned int _flags;

 
private:
    OG_DISALLOW_COPY_AND_ASSIGN(Action);
};

/** @class FiniteTimeAction
 * @brief
 * Base class actions that do have a finite time duration.
 * Possible actions:
 * - An action with a duration of 0 seconds.
 * - An action with a duration of 35.5 seconds.
 * Infinite time actions are valid.
 */
class   FiniteTimeAction : public Action
{
public:
    /** Get duration in seconds of the action. 
     *
     * @return The duration in seconds of the action.
     */
    float getDuration() const { return _duration; }
    /** Set duration in seconds of the action. 
     *
     * @param duration In seconds of the action.
     */
    void setDuration(float duration) { _duration = duration; }

    //
    // Overrides
    //
    virtual FiniteTimeAction* reverse() const override
    {
        OG_ASSERT(0);
        return nullptr;
    }
    virtual FiniteTimeAction* clone() const override
    {
        OG_ASSERT(0);
        return nullptr;
    }

public:
    FiniteTimeAction()
    : _duration(0)
    {}
    virtual ~FiniteTimeAction(){}

protected:
    //! Duration in seconds.
    float _duration;
private:
    OG_DISALLOW_COPY_AND_ASSIGN(FiniteTimeAction);
};
 

OG_END

#endif // __ACTIONS_OGACTION_H__
