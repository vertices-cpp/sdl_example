 

#ifndef _OG_ACTION_INTERVAL_H_
#define _OG_ACTION_INTERVAL_H_

#include <vector>

#include "base/ogMacros.h"
#include "2d/OGAction.h"
#include "2d/OGAnimation.h"
#include "base/OGProtocols.h"
#include "base/OGVector.h"

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

OG_CONSTRUCTOR_ACCESS:
    /** initializes the action */
    bool initWithDuration(float d);

protected:
    float _elapsed;
    bool _firstTick;
    bool _done;
    
protected:
   // bool sendUpdateEventToScript(float dt, Action *actionObject);
};

/** @class Sequence
 * @brief Runs actions sequentially, one after another.
 */
class   Sequence : public ActionInterval
{
public: 
    static Sequence* create(FiniteTimeAction *action1, ...) OG_REQUIRES_NULL_TERMINATION; 
    static Sequence* create(const Vector<FiniteTimeAction*>& arrayOfActions); 
    static Sequence* createWithVariableList(FiniteTimeAction *action1, va_list args); 
    static Sequence* createWithTwoActions(FiniteTimeAction *actionOne, FiniteTimeAction *actionTwo); 
    virtual Sequence* clone() const override;
    virtual Sequence* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    virtual void stop() override;
    virtual bool isDone() const override;
    /**
     * @param t In seconds.
     */
    virtual void update(float t) override;
    
OG_CONSTRUCTOR_ACCESS:
    Sequence();
    virtual ~Sequence();

    /** initializes the action */
    bool initWithTwoActions(FiniteTimeAction *pActionOne, FiniteTimeAction *pActionTwo);
    bool init(const Vector<FiniteTimeAction*>& arrayOfActions);

protected:
    FiniteTimeAction *_actions[2];
    float _split;
    int _last;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(Sequence);
};

/** @class Repeat
 * @brief Repeats an action a number of times.
 * To repeat an action forever use the RepeatForever action.
 */
class   Repeat : public ActionInterval
{
public: 
    static Repeat* create(FiniteTimeAction *action, unsigned int times); 
    void setInnerAction(FiniteTimeAction *action)
    {
        if (_innerAction != action)
        {
            OG_SAFE_RETAIN(action);
            OG_SAFE_RELEASE(_innerAction);
            _innerAction = action;
        }
    } 
    FiniteTimeAction* getInnerAction()
    {
        return _innerAction;
    }

    //
    // Overrides
    //
    virtual Repeat* clone() const override;
    virtual Repeat* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    virtual void stop() override;
    /**
     * @param dt In seconds.
     */
    virtual void update(float dt) override;
    virtual bool isDone() const override;
    
OG_CONSTRUCTOR_ACCESS:
    Repeat() {}
    virtual ~Repeat();

    /** initializes a Repeat action. Times is an unsigned integer between 1 and pow(2,30) */
    bool initWithAction(FiniteTimeAction *pAction, unsigned int times);

protected:
    unsigned int _times;
    unsigned int _total;
    float _nextDt;
    bool _actionInstant;
    /** Inner action */
    FiniteTimeAction *_innerAction;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(Repeat);
};

/** @class RepeatForever
 * @brief Repeats an action for ever.
 To repeat the an action for a limited number of times use the Repeat action.
 * @warning This action can't be Sequenceable because it is not an IntervalAction.
 */
class   RepeatForever : public ActionInterval
{
public: 
    static RepeatForever* create(ActionInterval *action); 
    void setInnerAction(ActionInterval *action)
    {
        if (_innerAction != action)
        {
            OG_SAFE_RELEASE(_innerAction);
            _innerAction = action;
            OG_SAFE_RETAIN(_innerAction);
        }
    }
	 
    ActionInterval* getInnerAction()
    {
        return _innerAction;
    }

    //
    // Overrides
    //
    virtual RepeatForever* clone() const override;
    virtual RepeatForever* reverse() const override;
    virtual void startWithTarget(Node* target) override;
    /**
     * @param dt In seconds.
     */
    virtual void step(float dt) override;
    virtual bool isDone() const override;
    
OG_CONSTRUCTOR_ACCESS:
    RepeatForever()
    : _innerAction(nullptr)
    {}
    virtual ~RepeatForever();

    /** initializes the action */
    bool initWithAction(ActionInterval *action);

protected:
    /** Inner action */
    ActionInterval *_innerAction;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(RepeatForever);
};

/** @class Spawn
 * @brief Spawn a new action immediately
 */
class   Spawn : public ActionInterval
{
public: 
    static Spawn* create(FiniteTimeAction *action1, ...) OG_REQUIRES_NULL_TERMINATION; 
    static Spawn* createWithVariableList(FiniteTimeAction *action1, va_list args); 
    static Spawn* create(const Vector<FiniteTimeAction*>& arrayOfActions); 
    static Spawn* createWithTwoActions(FiniteTimeAction *action1, FiniteTimeAction *action2);

    //
    // Overrides
    //
    virtual Spawn* clone() const override;
    virtual Spawn* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    virtual void stop() override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    Spawn();
    virtual ~Spawn();

    /** initializes the Spawn action with the 2 actions to spawn */
    bool initWithTwoActions(FiniteTimeAction *action1, FiniteTimeAction *action2);
    bool init(const Vector<FiniteTimeAction*>& arrayOfActions);

protected:
    FiniteTimeAction *_one;
    FiniteTimeAction *_two;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(Spawn);
};

/** @class RotateTo
 * @brief Rotates a Node object to a certain angle by modifying it's rotation attribute.
 The direction will be decided by the shortest angle.
*/ 
class   RotateTo : public ActionInterval
{
public: 
    static RotateTo* create(float duration, float dstAngleX, float dstAngleY); 
    static RotateTo* create(float duration, float dstAngle); 
    static RotateTo* create(float duration, const Vec2& dstAngle3D);

    //
    // Overrides
    //
    virtual RotateTo* clone() const override;
    virtual RotateTo* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    RotateTo();
    virtual ~RotateTo() {}
	 
    bool initWithDuration(float duration, float dstAngleX, float dstAngleY); 
    bool initWithDuration(float duration, const Vec2& dstAngle3D);

    /** 
     * calculates the start and diff angles
     * @param dstAngle in degreesCW
     */
    void calculateAngles(float &startAngle, float &diffAngle, float dstAngle);
    
protected:
    bool _is3D;
	Vec2 _dstAngle;
	Vec2 _startAngle;
	Vec2 _diffAngle;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(RotateTo);
};

/** @class RotateBy
 * @brief Rotates a Node object clockwise a number of degrees by modifying it's rotation attribute.
*/
class   RotateBy : public ActionInterval
{
public: 
    static RotateBy* create(float duration, float deltaAngle); 
    static RotateBy* create(float duration, float deltaAngleZ_X, float deltaAngleZ_Y); 
    static RotateBy* create(float duration, const Vec2& deltaAngle3D);

    //
    // Override
    //
    virtual RotateBy* clone() const override;
    virtual RotateBy* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    RotateBy();
    virtual ~RotateBy() {}

    /** initializes the action */
    bool initWithDuration(float duration, float deltaAngle); 
    bool initWithDuration(float duration, float deltaAngleZ_X, float deltaAngleZ_Y);
    bool initWithDuration(float duration, const Vec2& deltaAngle3D);
    
protected:
    bool _is3D;
	Vec2 _deltaAngle;
	Vec2 _startAngle;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(RotateBy);
};

/** @class MoveBy
 * @brief Moves a Node object x,y pixels by modifying it's position attribute.
 x and y are relative to the position of the object.
 Several MoveBy actions can be concurrently called, and the resulting
 movement will be the sum of individual movements.
 @since v2.1beta2-custom
 */
class   MoveBy : public ActionInterval
{
public: 
    static MoveBy* create(float duration, const Vec2& deltaPosition);  

    //
    // Overrides
    //
    virtual MoveBy* clone() const override;
    virtual MoveBy* reverse() const  override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time in seconds
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    MoveBy():_is3D(false) {}
    virtual ~MoveBy() {}

    /** initializes the action */
    bool initWithDuration(float duration, const Vec2& deltaPosition); 

protected:
    bool _is3D;
    Vec2 _positionDelta;
    Vec2 _startPosition;
    Vec2 _previousPosition;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(MoveBy);
};

/** @class MoveTo
 * @brief Moves a Node object to the position x,y. x and y are absolute coordinates by modifying it's position attribute.
 Several MoveTo actions can be concurrently called, and the resulting
 movement will be the sum of individual movements.
 @since v2.1beta2-custom
 */
class   MoveTo : public MoveBy
{
public: 
    static MoveTo* create(float duration, const Vec2& position); 

    //
    // Overrides
    //
    virtual MoveTo* clone() const override;
    virtual MoveTo* reverse() const  override;
    virtual void startWithTarget(Node *target) override;
    
OG_CONSTRUCTOR_ACCESS:
    MoveTo() {}
    virtual ~MoveTo() {}  
    bool initWithDuration(float duration, const Vec2& position);

protected:
    Vec2 _endPosition;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(MoveTo);
};

/** @class SkewTo
 * @brief Skews a Node object to given angles by modifying it's skewX and skewY attributes
@since v1.0
*/
class   SkewTo : public ActionInterval
{
public: 
    static SkewTo* create(float t, float sx, float sy);

    //
    // Overrides
    //
    virtual SkewTo* clone() const override;
    virtual SkewTo* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    SkewTo();
    virtual ~SkewTo() {}
    /**
     * @param t In seconds.
     */
    bool initWithDuration(float t, float sx, float sy);

protected:
    float _skewX;
    float _skewY;
    float _startSkewX;
    float _startSkewY;
    float _endSkewX;
    float _endSkewY;
    float _deltaX;
    float _deltaY;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(SkewTo);
};

/** @class SkewBy
* @brief Skews a Node object by skewX and skewY degrees.
@since v1.0
*/
class   SkewBy : public SkewTo
{
public: 
    static SkewBy* create(float t, float deltaSkewX, float deltaSkewY);

    //
    // Overrides
    //
    virtual void startWithTarget(Node *target) override;
    virtual SkewBy* clone() const  override;
    virtual SkewBy* reverse() const override;
    
OG_CONSTRUCTOR_ACCESS:
    SkewBy() {}
    virtual ~SkewBy() {}
    /**
     * @param t In seconds.
     */
    bool initWithDuration(float t, float sx, float sy);

private:
    OG_DISALLOW_COPY_AND_ASSIGN(SkewBy);
};

/** @class ResizeTo
* @brief Resize a Node object to the final size by modifying it's Size attribute.
*/
class    ResizeTo : public ActionInterval 
{
public: 
    static ResizeTo* create(float duration, const orange::Size& final_size);

    //
    // Overrides
    //
    virtual ResizeTo* clone() const override;
    void startWithTarget(orange::Node* target) override;
    void update(float time) override;

OG_CONSTRUCTOR_ACCESS:
    ResizeTo() {}
    virtual ~ResizeTo() {} 
    bool initWithDuration(float duration, const orange::Size& final_size);

protected:
    orange::Size _initialSize;
    orange::Size _finalSize;
    orange::Size _sizeDelta;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(ResizeTo);
};


/** @class ResizeBy
* @brief Resize a Node object by a Size. Works on all nodes where setContentSize is effective. But it's mostly useful for nodes where 9-slice is enabled
*/
class   ResizeBy : public ActionInterval 
{
public: 
    static ResizeBy* create(float duration, const orange::Size& deltaSize);
    
    //
    // Overrides
    //
    virtual ResizeBy* clone() const override;
    virtual ResizeBy* reverse() const  override;
    virtual void startWithTarget(Node *target) override;
    /**
    * @param time in seconds
    */
    virtual void update(float time) override;

OG_CONSTRUCTOR_ACCESS:
    ResizeBy() {}
    virtual ~ResizeBy() {}
    
    /** initializes the action */
    bool initWithDuration(float duration, const orange::Size& deltaSize);

protected:
    orange::Size _sizeDelta;
    orange::Size _startSize;
    orange::Size _previousSize;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(ResizeBy);
};


/** @class JumpBy
 * @brief Moves a Node object simulating a parabolic jump movement by modifying it's position attribute.
*/
class   JumpBy : public ActionInterval
{
public: 
    static JumpBy* create(float duration, const Vec2& position, float height, int jumps);

    //
    // Overrides
    //
    virtual JumpBy* clone() const override;
    virtual JumpBy* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    JumpBy() {}
    virtual ~JumpBy() {}

    /** 
     * initializes the action
     * @param duration in seconds
     */
    bool initWithDuration(float duration, const Vec2& position, float height, int jumps);

protected:
    Vec2           _startPosition;
    Vec2           _delta;
    float           _height;
    int             _jumps;
    Vec2           _previousPos;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(JumpBy);
};

/** @class JumpTo
 * @brief Moves a Node object to a parabolic position simulating a jump movement by modifying it's position attribute.
*/ 
class   JumpTo : public JumpBy
{
public: 
    static JumpTo* create(float duration, const Vec2& position, float height, int jumps);

    //
    // Override
    //
    virtual void startWithTarget(Node *target) override;
    virtual JumpTo* clone() const override;
    virtual JumpTo* reverse() const override;

OG_CONSTRUCTOR_ACCESS:
    JumpTo() {}
    virtual ~JumpTo() {}

    /** 
     * initializes the action
     * @param duration In seconds.
     */
    bool initWithDuration(float duration, const Vec2& position, float height, int jumps);

protected:
    Vec2 _endPosition;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(JumpTo);
};

/** @struct Bezier configuration structure
 */
typedef struct _ogBezierConfig {
    //! end position of the bezier
    Vec2 endPosition;
    //! Bezier control point 1
    Vec2 controlPoint_1;
    //! Bezier control point 2
    Vec2 controlPoint_2;
} ogBezierConfig;

/** @class BezierBy
 * @brief An action that moves the target with a cubic Bezier curve by a certain distance.
 */
class   BezierBy : public ActionInterval
{
public: 
    static BezierBy* create(float t, const ogBezierConfig& c);

    //
    // Overrides
    //
    virtual BezierBy* clone() const override;
    virtual BezierBy* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    BezierBy() {}
    virtual ~BezierBy() {}

    /** 
     * initializes the action with a duration and a bezier configuration
     * @param t in seconds
     */
    bool initWithDuration(float t, const ogBezierConfig& c);

protected:
    ogBezierConfig _config;
    Vec2 _startPosition;
    Vec2 _previousPosition;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(BezierBy);
};

/** @class BezierTo
 * @brief An action that moves the target with a cubic Bezier curve to a destination point.
 @since v0.8.2
 */
class   BezierTo : public BezierBy
{
public: 
    static BezierTo* create(float t, const ogBezierConfig& c);

    //
    // Overrides
    //
    virtual void startWithTarget(Node *target) override;
    virtual BezierTo* clone() const override;
    virtual BezierTo* reverse() const override;
    
OG_CONSTRUCTOR_ACCESS:
    BezierTo() {}
    virtual ~BezierTo() {}
    /**
     * @param t In seconds.
     */
    bool initWithDuration(float t, const ogBezierConfig &c);

protected:
    ogBezierConfig _toConfig;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(BezierTo);
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
    
OG_CONSTRUCTOR_ACCESS:
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

OG_CONSTRUCTOR_ACCESS:
    ScaleBy() {}
    virtual ~ScaleBy() {}

private:
    OG_DISALLOW_COPY_AND_ASSIGN(ScaleBy);
};

/** @class Blink
 * @brief Blinks a Node object by modifying it's visible attribute.
*/
class   Blink : public ActionInterval
{
public: 
    static Blink* create(float duration, int blinks);

    //
    // Overrides
    //
    virtual Blink* clone() const override;
    virtual Blink* reverse() const override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual void startWithTarget(Node *target) override;
    virtual void stop() override;
    
OG_CONSTRUCTOR_ACCESS:
    Blink() {}
    virtual ~Blink() {}

    /** 
     * initializes the action 
     * @param duration in seconds
     */
    bool initWithDuration(float duration, int blinks);
    
protected:
    int _times;
    bool _originalState;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(Blink);
};


/** @class FadeTo
 * @brief Fades an object that implements the RGBAProtocol protocol. It modifies the opacity from the current value to a custom one.
 @warning This action doesn't support "reverse"
 */
class   FadeTo : public ActionInterval
{
public: 
    static FadeTo* create(float duration, uint8_t opacity);

    //
    // Overrides
    //
    virtual FadeTo* clone() const override;
    virtual FadeTo* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    FadeTo() {}
    virtual ~FadeTo() {} 
    bool initWithDuration(float duration, uint8_t opacity);

protected:
    uint8_t _toOpacity;
    uint8_t _fromOpacity;
    friend class FadeOut;
    friend class FadeIn;
private:
    OG_DISALLOW_COPY_AND_ASSIGN(FadeTo);
};

/** @class FadeIn
 * @brief Fades In an object that implements the RGBAProtocol protocol. It modifies the opacity from 0 to 255.
 The "reverse" of this action is FadeOut
 */
class   FadeIn : public FadeTo
{
public: 
    static FadeIn* create(float d);

    //
    // Overrides
    //
    virtual void startWithTarget(Node *target) override;
    virtual FadeIn* clone() const override;
    virtual FadeTo* reverse() const override;

    /**
     * @js NA
     */
    void setReverseAction(FadeTo* ac);

OG_CONSTRUCTOR_ACCESS:
    FadeIn():_reverseAction(nullptr) {}
    virtual ~FadeIn() {}

private:
    OG_DISALLOW_COPY_AND_ASSIGN(FadeIn);
    FadeTo* _reverseAction;
};

/** @class FadeOut
 * @brief Fades Out an object that implements the RGBAProtocol protocol. It modifies the opacity from 255 to 0.
 The "reverse" of this action is FadeIn
*/
class   FadeOut : public FadeTo
{
public: 
    static FadeOut* create(float d);

    //
    // Overrides
    //
    virtual void startWithTarget(Node *target) override;
    virtual FadeOut* clone() const  override;
    virtual FadeTo* reverse() const override;

    /**
     * @js NA
     */
    void setReverseAction(FadeTo* ac);

OG_CONSTRUCTOR_ACCESS:
    FadeOut():_reverseAction(nullptr) {}
    virtual ~FadeOut() {}
private:
    OG_DISALLOW_COPY_AND_ASSIGN(FadeOut);
    FadeTo* _reverseAction;
};

/** @class TintTo
 * @brief Tints a Node that implements the NodeRGB protocol from current tint to a custom one.
 @warning This action doesn't support "reverse"
 @since v0.7.2
*/
class   TintTo : public ActionInterval
{
public: 
    static TintTo* create(float duration, uint8_t red, uint8_t green, uint8_t blue); 
    static TintTo* create(float duration, const Color3B& color);

    //
    // Overrides
    //
    virtual TintTo* clone() const override;
    virtual TintTo* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    TintTo() {}
    virtual ~TintTo() {}

    /** initializes the action with duration and color */
    bool initWithDuration(float duration, uint8_t red, uint8_t green, uint8_t blue);

protected:
    Color3B _to;
    Color3B _from;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(TintTo);
};

/** @class TintBy
 @brief Tints a Node that implements the NodeRGB protocol from current tint to a custom one.
 @since v0.7.2
 */
class   TintBy : public ActionInterval
{
public: 
    static TintBy* create(float duration, int16_t deltaRed, int16_t deltaGreen, int16_t deltaBlue);

    //
    // Overrides
    //
    virtual TintBy* clone() const override;
    virtual TintBy* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    TintBy() {}
    virtual ~TintBy() {}

    /** initializes the action with duration and color */
    bool initWithDuration(float duration, int16_t deltaRed, int16_t deltaGreen, int16_t deltaBlue);

protected:
    int16_t _deltaR;
    int16_t _deltaG;
    int16_t _deltaB;

    int16_t _fromR;
    int16_t _fromG;
    int16_t _fromB;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(TintBy);
};

/** @class DelayTime
 * @brief Delays the action a certain amount of seconds.
*/
class   DelayTime : public ActionInterval
{
public: 
    static DelayTime* create(float d);

    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    virtual DelayTime* reverse() const override;
    virtual DelayTime* clone() const override;

OG_CONSTRUCTOR_ACCESS:
    DelayTime() {}
    virtual ~DelayTime() {}

private:
    OG_DISALLOW_COPY_AND_ASSIGN(DelayTime);
};

/** @class ReverseTime
 * @brief Executes an action in reverse order, from time=duration to time=0
 
 @warning Use this action carefully. This action is not
 sequenceable. Use it as the default "reversed" method
 of your own actions, but using it outside the "reversed"
 scope is not recommended.
*/
class   ReverseTime : public ActionInterval
{
public: 
    static ReverseTime* create(FiniteTimeAction *action);

    //
    // Overrides
    //
    virtual ReverseTime* reverse() const override;
    virtual ReverseTime* clone() const override;
    virtual void startWithTarget(Node *target) override;
    virtual void stop() override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    ReverseTime();
    virtual ~ReverseTime();

    /** initializes the action */
    bool initWithAction(FiniteTimeAction *action);

protected:
    FiniteTimeAction *_other;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(ReverseTime);
};

class Texture2D;
/** @class Animate
 * @brief Animates a sprite given the name of an Animation.
 */
class   Animate : public ActionInterval
{
public: 
    static Animate* create(Animation *animation); 
    void setAnimation( Animation* animation ); 
    Animation* getAnimation() { return _animation; }
    const Animation* getAnimation() const { return _animation; } 
    int getCurrentFrameIndex() { return _currFrameIndex; }
    //
    // Overrides
    //
    virtual Animate* clone() const override;
    virtual Animate* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    virtual void stop() override;
    /**
     * @param t In seconds.
     */
    virtual void update(float t) override;
    
OG_CONSTRUCTOR_ACCESS:
    Animate();
    virtual ~Animate();

    /** initializes the action with an Animation and will restore the original frame when the animation is over */
    bool initWithAnimation(Animation *animation);

protected:
    std::vector<float>* _splitTimes = new std::vector<float>;
    int             _nextFrame = 0;
    SpriteFrame*    _origFrame = nullptr;
    int _currFrameIndex = 0;
    unsigned int    _executedLoops = 0;
    Animation*      _animation = nullptr;

    EventCustom*    _frameDisplayedEvent = nullptr;
    AnimationFrame::DisplayedEventInfo _frameDisplayedEventInfo;
private:
    OG_DISALLOW_COPY_AND_ASSIGN(Animate);
};

/** @class TargetedAction
 * @brief Overrides the target of an action so that it always runs on the target
 * specified at action creation rather than the one specified by runAction.
 */
class   TargetedAction : public ActionInterval
{
public: 
    static TargetedAction* create(Node* target, FiniteTimeAction* action); 
    void setForcedTarget(Node* forcedTarget); 
    Node* getForcedTarget() { return _forcedTarget; }
    const Node* getForcedTarget() const { return _forcedTarget; }

    //
    // Overrides
    //
    virtual TargetedAction* clone() const override;
    virtual TargetedAction* reverse() const  override;
    virtual void startWithTarget(Node *target) override;
    virtual void stop() override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
OG_CONSTRUCTOR_ACCESS:
    TargetedAction();
    virtual ~TargetedAction();

    /** Init an action with the specified action and forced target */
    bool initWithTarget(Node* target, FiniteTimeAction* action);

protected:
    FiniteTimeAction* _action;
    Node* _forcedTarget;

private:
    OG_DISALLOW_COPY_AND_ASSIGN(TargetedAction);
};

/**
 * @class ActionFloat
 * @brief Action used to animate any value in range [from,to] over specified time interval
 */
class   ActionFloat : public ActionInterval
{
public: 
    typedef std::function<void(float value)> ActionFloatCallback; 
    static ActionFloat* create(float duration, float from, float to, ActionFloatCallback callback);

    /**
     * Overridden ActionInterval methods
     */
    void startWithTarget(Node* target) override;
    void update(float delta) override;
    ActionFloat* reverse() const override;
    ActionFloat* clone() const override;

OG_CONSTRUCTOR_ACCESS:
    ActionFloat() {};
    virtual ~ActionFloat() {};

    bool initWithDuration(float duration, float from, float to, ActionFloatCallback callback);

protected:
    /* From value */
    float _from;
    /* To value */
    float _to;
    /* delta time */
    float _delta;

    /* Callback to report back results */
    ActionFloatCallback _callback;
private:
    OG_DISALLOW_COPY_AND_ASSIGN(ActionFloat);
};

// end of actions group
/// @}

OG_END

#endif //__ACTION_OGINTERVAL_ACTION_H__
