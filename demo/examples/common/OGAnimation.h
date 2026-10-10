
#ifndef __OG_ANIMATION_H__
#define __OG_ANIMATION_H__

//#include "OGPlatformConfig.h"
#include "OGRef.h"
#include "OGValue.h"
#include "OGVector.h"
//#include "OGSpriteFrame.h"

#include <string>

OG_BEGIN

class Texture2D;
class SpriteFrame;

/**
 * @addtogroup _2d
 * @{
 */

/** @class AnimationFrame
 *
 * A frame of the animation. It contains information like:
 * - sprite frame name.
 * - # of delay units.
 * - offset
 
 @since v2.0
 */
class AnimationFrame : public Ref, public Clonable
{
public:
    /** @struct DisplayedEventInfo
     * When the animation display,Dispatches the event of UserData.
     */
    struct DisplayedEventInfo
    {
        Node* target;
        const ValueMap* userInfo;
    };
    
    /**
     * Creates the animation frame with a spriteframe, number of delay units and a notification user info.
     *
     * @param spriteFrame The animation frame with a spriteframe.
     * @param delayUnits Number of delay units.
     * @param userInfo A notification user info.
     * @since 3.0
     */
    static AnimationFrame* create(SpriteFrame* spriteFrame, float delayUnits, const ValueMap& userInfo);
    /** Return a SpriteFrameName to be used.
     *
     * @return a SpriteFrameName to be used.
     */
    SpriteFrame* getSpriteFrame() const { return _spriteFrame; };
    /** Set the SpriteFrame.
     *
     * @param frame A SpriteFrame will be used.
     */
//     void setSpriteFrame(SpriteFrame* frame)
//     {
//         OG_SAFE_RETAIN(frame);
//         OG_SAFE_RELEASE(_spriteFrame);
//         _spriteFrame = frame;
//     }

    /** Gets the units of time the frame takes.
     *
     * @return The units of time the frame takes.
     */
    float getDelayUnits() const { return _delayUnits; };
    
    /** Sets the units of time the frame takes.
     *
     * @param delayUnits The units of time the frame takes.
     */
    void setDelayUnits(float delayUnits) { _delayUnits = delayUnits; };
    
    /** @brief Gets user information
     * A AnimationFrameDisplayedNotification notification will be broadcast when the frame is displayed with this dictionary as UserInfo. 
     * If UserInfo is nil, then no notification will be broadcast.
     *
     * @return A dictionary as UserInfo
     */
    const ValueMap& getUserInfo() const { return _userInfo; };
    ValueMap& getUserInfo() { return _userInfo; };
    
    /** Sets user information.
     * @param userInfo A dictionary as UserInfo.
     */
    void setUserInfo(const ValueMap& userInfo)
    {
        _userInfo = userInfo;
    }
    
    // Overrides
    virtual AnimationFrame *clone() const override;
    
public:
    /**
     * @js ctor
     */
    AnimationFrame();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~AnimationFrame();
    
    /** initializes the animation frame with a spriteframe, number of delay units and a notification user info */
    bool initWithSpriteFrame(SpriteFrame* spriteFrame, float delayUnits, const ValueMap& userInfo);

protected:
    
    /** SpriteFrameName to be used */
    SpriteFrame* _spriteFrame;

    /**  how many units of time the frame takes */
    float _delayUnits;

    /**  A AnimationFrameDisplayedNotification notification will be broadcast when the frame is displayed with this dictionary as UserInfo. If UserInfo is nil, then no notification will be broadcast. */
    ValueMap _userInfo;
    
private:
    OG_DISALLOW_COPY_AND_ASSIGN(AnimationFrame);
};




/** @class Animation
 * A Animation object is used to perform animations on the Sprite objects.
 * The Animation object contains AnimationFrame objects, and a possible delay between the frames.
 * You can animate a Animation object by using the Animate action. Example:
 * @code
 * sprite->runAction(Animate::create(animation));
 * @endcode
*/
class Animation : public Ref, public Clonable
{
public:
    /** Creates an animation.
     * @since v0.99.5
     */
    static Animation* create(); 
    static Animation* createWithSpriteFrames(const Vector<SpriteFrame*>& arrayOfSpriteFrameNames, float delay = 0.0f, unsigned int loops = 1);
	 
    static Animation* create(const Vector<AnimationFrame*>& arrayOfAnimationFrameNames, float delayPerUnit, unsigned int loops = 1);
 
    void addSpriteFrame(SpriteFrame *frame); 
//     void addSpriteFrameWithFile(const std::string& filename); 
//     void addSpriteFrameWithTexture(Texture2D* pobTexture, const Rect& rect); 
    float getTotalDelayUnits() const { return _totalDelayUnits; }; 
    void setDelayPerUnit(float delayPerUnit) { _delayPerUnit = delayPerUnit; }; 
    float getDelayPerUnit() const { return _delayPerUnit; }; 
    float getDuration() const; 
    const Vector<AnimationFrame*>& getFrames() const { return _frames; }; 
    void setFrames(const Vector<AnimationFrame*>& frames)
    {
        _frames = frames;
    } 
    bool getRestoreOriginalFrame() const { return _restoreOriginalFrame; }; 
    void setRestoreOriginalFrame(bool restoreOriginalFrame) { _restoreOriginalFrame = restoreOriginalFrame; }; 
    unsigned int getLoops() const { return _loops; }; 
    void setLoops(unsigned int loops) { _loops = loops; };
    
    // overrides
    virtual Animation *clone() const override;
    
public:
    Animation();
    virtual ~Animation();
    
    /** Initializes a Animation. */
    bool init();
    
    /** Initializes a Animation with frames and a delay between frames.
     * @since v0.99.5
     */
    bool initWithSpriteFrames(const Vector<SpriteFrame*>& arrayOfSpriteFrameNames, float delay = 0.0f, unsigned int loops = 1);
    
    /** Initializes a Animation with AnimationFrame.
     * @since v2.0
     */
    bool initWithAnimationFrames(const Vector<AnimationFrame*>& arrayOfAnimationFrameNames, float delayPerUnit, unsigned int loops);

protected:
    /** total Delay units of the Animation. */
    float _totalDelayUnits;

    /** Delay in seconds of the "delay unit". */
    float _delayPerUnit;

    /** duration in seconds of the whole animation. It is the result of totalDelayUnits * delayPerUnit. */
    float _duration;

    /** array of AnimationFrames. */
    Vector<AnimationFrame*> _frames;

    /** whether or not it shall restore the original frame when the animation finishes. */
    bool _restoreOriginalFrame;

    /** how many times the animation is going to loop. 0 means animation is not animated. 1, animation is executed one time, ... */
    unsigned int _loops;
    
private:
    OG_DISALLOW_COPY_AND_ASSIGN(Animation);
};

// end of sprite_nodes group
/// @}

OG_END

#endif // __OG_ANIMATION_H__
