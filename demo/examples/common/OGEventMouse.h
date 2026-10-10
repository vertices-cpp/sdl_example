
#ifndef __orange_libs__OGMouseEvent__
#define __orange_libs__OGMouseEvent__

#include "OGEvent.h"
#include "OGGeometry.h"
 
OG_BEGIN 

class   EventMouse : public Event
{
public: 
    enum class MouseEventType
    {
        MOUSE_NONE,
        MOUSE_DOWN,
        MOUSE_UP,
        MOUSE_MOVE,
        MOUSE_SCROLL,
    };

    enum class MouseButton
    {
      BUTTON_UNSET   = -1,
      BUTTON_LEFT    =  0,
      BUTTON_RIGHT   =  1,
      BUTTON_MIDDLE  =  2,
      BUTTON_4       =  3,
      BUTTON_5       =  4,
      BUTTON_6       =  5,
      BUTTON_7       =  6,
      BUTTON_8       =  7
    };
	 
    EventMouse(MouseEventType mouseEventCode); 
    void setScrollData(float scrollX, float scrollY) { _scrollX = scrollX; _scrollY = scrollY; } 
    float getScrollX() const { return _scrollX; } 
    float getScrollY() const { return _scrollY; } 
    void setCursorPosition(float x, float y) { 
        _x = x;
        _y = y;
        _prevPoint = _point;
        _point.x = x;
        _point.y = y;
        if (!_startPointCaptured)
        {
            _startPoint = _point;
            _startPointCaptured = true;
        }
    } 
    void setMouseButton(MouseButton button) { _mouseButton = button; } 
    MouseButton getMouseButton() const { return _mouseButton; } 
    float getCursorX() const { return _x; } 
    float getCursorY() const { return _y; } 
    Vec2 getLocation() const; 
    Vec2 getPreviousLocation() const; 
    Vec2 getStartLocation() const; 
    Vec2 getDelta() const; 
    Vec2 getLocationInView() const; 
    Vec2 getPreviousLocationInView() const; 
    Vec2 getStartLocationInView() const;


private:
    MouseEventType _mouseEventType;
    MouseButton _mouseButton;
    float _x;
    float _y;
    float _scrollX;
    float _scrollY;

    bool _startPointCaptured;
    Vec2 _startPoint;
    Vec2 _point;
    Vec2 _prevPoint;

    friend class EventListenerMouse;
};

OG_END

// end of base group
/// @}

#endif /* defined(__orange_libs__OGMouseEvent__) */
