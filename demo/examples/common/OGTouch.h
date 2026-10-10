

#ifndef _OG_TOUCH_H_
#define _OG_TOUCH_H_

//#include "OGHeader.h"
#include "OGRef.h"
#include "OGGeometry.h"



OG_BEGIN


/**
 * @addtogroup base
 * @{
 */

/** @class Touch
 * @brief Encapsulates the Touch information, such as touch point, id and so on,
 and provides the methods that commonly used.
 */
class  Touch : public Ref
{
public:
    /** 
     * Dispatch mode, how the touches are dispatched.
     * @js NA
     */
    enum class DispatchMode {
        ALL_AT_ONCE, /** All at once. */
        ONE_BY_ONE,  /** One by one. */
    };

    /** Constructor.
     * @js ctor
     */
    Touch() 
        : _id(0),
        _startPointCaptured(false),
        _curForce(0.f),
        _maxForce(0.f)
    {}

    /** Returns the current touch location in OpenGL coordinates.
     *
     * @return The current touch location in OpenGL coordinates.
     */
    Vec2 getLocation() const;
    /** Returns the previous touch location in OpenGL coordinates.
     *
     * @return The previous touch location in OpenGL coordinates.
     */
    Vec2 getPreviousLocation() const;
    /** Returns the start touch location in OpenGL coordinates.
     *
     * @return The start touch location in OpenGL coordinates.
     */
    Vec2 getStartLocation() const;
    /** Returns the delta of 2 current touches locations in screen coordinates.
     *
     * @return The delta of 2 current touches locations in screen coordinates.
     */
    Vec2 getDelta() const;
    /** Returns the current touch location in screen coordinates.
     *
     * @return The current touch location in screen coordinates.
     */
    Vec2 getLocationInView() const;
    /** Returns the previous touch location in screen coordinates. 
     *
     * @return The previous touch location in screen coordinates.
     */
    Vec2 getPreviousLocationInView() const;
    /** Returns the start touch location in screen coordinates.
     *
     * @return The start touch location in screen coordinates.
     */
    Vec2 getStartLocationInView() const;
    
    /** Set the touch information. It always used to monitor touch event.
     *
     * @param id A given id
     * @param x A given x coordinate.
     * @param y A given y coordinate.
     */
    void setTouchInfo(int id, float x, float y)
    {
        _id = id;
        _prevPoint = _point;
        _point.x   = x;
        _point.y   = y;
        _curForce = 0.0f;
        _maxForce = 0.0f;
        if (!_startPointCaptured)
        {
            _startPoint = _point;
            _startPointCaptured = true;
            _prevPoint = _point;
        }
    }

    /** Set the touch information. It always used to monitor touch event.
     *
     * @param id A given id
     * @param x A given x coordinate.
     * @param y A given y coordinate.
     * @param force Current force for 3d touch.
     * @param maxForce maximum possible force for 3d touch.
     */
    void setTouchInfo(int id, float x, float y, float force, float maxForce)
    {
        _id = id;
        _prevPoint = _point;
        _point.x   = x;
        _point.y   = y;
        _curForce = force;
        _maxForce = maxForce;
        if (!_startPointCaptured)
        {
            _startPoint = _point;
            _startPointCaptured = true;
            _prevPoint = _point;
        }
    }
    /** Get touch id.
     * @js getId
     * @lua getId
     *
     * @return The id of touch.
     */
    int getID() const
    {
        return _id;
    }
    /** Returns the current touch force for 3d touch.
     *
     * @return The current touch force for 3d touch.
     */
    float getCurrentForce() const;
    /** Returns the maximum touch force for 3d touch.
     *
     * @return The maximum touch force for 3d touch.
     */
    float getMaxForce() const;

private:
    int _id;
    bool _startPointCaptured;
    Vec2 _startPoint;
    Vec2 _point;
    Vec2 _prevPoint;
    float _curForce;
    float _maxForce;
};

// end of base group
/// @}



OG_END

#endif  // __PLATFORM_TOUCH_H__
