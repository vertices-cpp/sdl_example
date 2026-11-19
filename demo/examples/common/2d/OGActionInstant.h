

#ifndef _OG_ACTION_INSTANT_H_
#define _OG_ACTION_INSTANT_H_

#include <functional>
#include "2d/OGAction.h"

OG_BEGIN

/**
 * @addtogroup actions
 * @{
 */

/** @class ActionInstant
* @brief Instant actions are immediate actions. They don't have a duration like the IntervalAction actions.
**/
class   ActionInstant : public FiniteTimeAction
{
public:
    //
    // Overrides
    //
    virtual ActionInstant* clone() const override
    {
        OG_ASSERT(0);
        return nullptr;
    }
    
    virtual ActionInstant * reverse() const override
    {
        OG_ASSERT(0);
        return nullptr;
    }

    virtual void startWithTarget(Node *target) override;
    
    virtual bool isDone() const override;
    /**
     * @param dt In seconds.
     */
    virtual void step(float dt) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;

private:
    bool _done;
};

/** @class Show
* @brief Show the node.
**/
class   Show : public ActionInstant
{
public:
    /** Allocates and initializes the action.
     *
     * @return  An autoreleased Show object.
     */
    static Show * create();

    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual ActionInstant* reverse() const override;
    virtual Show* clone() const override;

OG_CONSTRUCTOR_ACCESS:
    Show(){}
    virtual ~Show(){}

private:
    OG_DISALLOW_COPY_AND_ASSIGN(Show);
};

/** @class Hide
* @brief Hide the node.
*/
class   Hide : public ActionInstant
{
public:
    /** Allocates and initializes the action.
     *
     * @return An autoreleased Hide object.
     */
    static Hide * create();

    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual ActionInstant* reverse() const override;
    virtual Hide* clone() const override;

OG_CONSTRUCTOR_ACCESS:
    Hide(){}
    virtual ~Hide(){}

private:
    OG_DISALLOW_COPY_AND_ASSIGN(Hide);
};

/** @class ToggleVisibility
* @brief Toggles the visibility of a node.
*/
class   ToggleVisibility : public ActionInstant
{
public:
    /** Allocates and initializes the action.
     *
     * @return An autoreleased ToggleVisibility object.
     */
    static ToggleVisibility * create();

    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual ToggleVisibility* reverse() const override;
    virtual ToggleVisibility* clone() const override;

OG_CONSTRUCTOR_ACCESS:
    ToggleVisibility(){}
    virtual ~ToggleVisibility(){}

private:
    OG_DISALLOW_COPY_AND_ASSIGN(ToggleVisibility);
};

/** @class RemoveSelf
* @brief Remove the node.
*/
class   RemoveSelf : public ActionInstant
{
public:
    /** Create the action.
     *
     * @param isNeedCleanUp Is need to clean up, the default value is true.
     * @return An autoreleased RemoveSelf object.
     */
    static RemoveSelf * create(bool isNeedCleanUp = true);

    //
    // Override
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual RemoveSelf* clone() const override;
    virtual RemoveSelf* reverse() const override;
    
OG_CONSTRUCTOR_ACCESS:
    RemoveSelf() : _isNeedCleanUp(true){}
    virtual ~RemoveSelf(){}

    /** init the action */
    bool init(bool isNeedCleanUp);

protected:
    bool _isNeedCleanUp;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(RemoveSelf);
};

/** @class FlipX
* @brief Flips the sprite horizontally.
* @since v0.99.0
*/
class   FlipX : public ActionInstant
{
public:
    /** Create the action.
     *
     * @param x Flips the sprite horizontally if true.
     * @return  An autoreleased FlipX object.
     */
    static FlipX * create(bool x);

    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual FlipX* reverse() const override;
    virtual FlipX* clone() const override;
    
OG_CONSTRUCTOR_ACCESS:
    FlipX() :_flipX(false) {}
    virtual ~FlipX() {}

    /** init the action */
    bool initWithFlipX(bool x);

protected:
    bool    _flipX;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(FlipX);
};

/** @class FlipY
* @brief Flips the sprite vertically.
* @since v0.99.0
*/
class   FlipY : public ActionInstant
{
public:
    /** Create the action.
     *
     * @param y Flips the sprite vertically if true.
     * @return An autoreleased FlipY object.
     */
    static FlipY * create(bool y);

    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual FlipY* reverse() const override;
    virtual FlipY* clone() const override;
    
OG_CONSTRUCTOR_ACCESS:
    FlipY() :_flipY(false) {}
    virtual ~FlipY() {}

    /** init the action */
    bool initWithFlipY(bool y);

protected:
    bool    _flipY;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(FlipY);
};

/** @class Place
* @brief Places the node in a certain position.
*/
class   Place : public ActionInstant
{
public:

    /** Creates a Place action with a position.
     *
     * @param pos  A certain position.
     * @return  An autoreleased Place object.
     */
    static Place * create(const Vec2& pos);

    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual Place* reverse() const override;
    virtual Place* clone() const override;
    
OG_CONSTRUCTOR_ACCESS:
    Place(){}
    virtual ~Place(){}

    /** Initializes a Place action with a position */
    bool initWithPosition(const Vec2& pos);

protected:
    Vec2 _position;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(Place);
};


/** @class CallFunc
* @brief Calls a 'callback'.
*/
class   CallFunc : public ActionInstant
{
public:
    /** Creates the action with the callback of type std::function<void()>.
     This is the preferred way to create the callback.
     * When this function bound in js or lua ,the input param will be changed.
     * In js: var create(var func, var this, var [data]) or var create(var func).
     * In lua:local create(local funcID).
     *
     * @param func  A callback function need to be executed.
     * @return  An autoreleased CallFunc object.
     */
    static CallFunc * create(const std::function<void()>& func);

public:
    /** Executes the callback.
     */
    virtual void execute();

    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual CallFunc* reverse() const override;
    virtual CallFunc* clone() const override;
    
OG_CONSTRUCTOR_ACCESS:
    CallFunc() = default;
    virtual ~CallFunc() = default;
    
    /** initializes the action with the std::function<void()>
     * @lua NA
     */
    bool initWithFunction(const std::function<void()>& func);

protected:
    
    /** function that will be called */
    std::function<void()> _function;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(CallFunc);
};

/** @class CallFuncN
* @brief Calls a 'callback' with the node as the first argument. N means Node.
* @js NA
*/
class   CallFuncN : public CallFunc
{
public:
    /** Creates the action with the callback of type std::function<void()>.
     This is the preferred way to create the callback.
     *
     * @param func  A callback function need to be executed.
     * @return  An autoreleased CallFuncN object.
     */
    static CallFuncN * create(const std::function<void(Node*)>& func);

    //
    // Overrides
    //
    virtual CallFuncN* clone() const override;
    virtual void execute() override;
    
OG_CONSTRUCTOR_ACCESS:
    CallFuncN():_functionN(nullptr){}
    virtual ~CallFuncN(){}

    /** initializes the action with the std::function<void(Node*)> */
    bool initWithFunction(const std::function<void(Node*)>& func);

protected:
    /** function that will be called with the "sender" as the 1st argument */
    std::function<void(Node*)> _functionN;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(CallFuncN);
};

// end of actions group
/// @}

OG_END

#endif //_OG_ACTION_INSTANT_H_
