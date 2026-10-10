

#include "OGActionInterval.h"

#include <stdarg.h>

//#include "OGSprite.h"
#include "OGNode.h"
//#include "OGSpriteFrame.h"
//#include "OGActionInstant.h"
#include "OGDirector.h"
//#include "OGEventCustom.h"
#include "OGEventDispatcher.h"
//#include "OGStdC.h"
//#include "OGScriptSupport.h"

OG_BEGIN

bool ActionInterval::initWithDuration(float d)
{
	_duration = d;

	_elapsed = 0;
	_firstTick = true;
	_done = false;

	return true;
}

bool ActionInterval::isDone() const
{
	return _done;
}

void ActionInterval::step(float dt)
{
	if (_firstTick)
	{
		_firstTick = false;
		_elapsed = 0;
	}
	else
	{
		_elapsed += dt;
	}


	float updateDt = std::max(0.0f,                                  // needed for rewind. elapsed could be negative
		std::min(1.0f, _elapsed / _duration)
	);

	// if (sendUpdateEventToScript(updateDt, this)) return;

	this->update(updateDt);

	_done = _elapsed >= _duration;
}

void ActionInterval::setAmplitudeRate(float /*amp*/)
{
	// Abstract class needs implementation
	OGASSERT(0, "Subclass should implement this method!");
}

float ActionInterval::getAmplitudeRate()
{
	// Abstract class needs implementation
	OGASSERT(0, "Subclass should implement this method!");

	return 0;
}

void ActionInterval::startWithTarget(Node *target)
{
	FiniteTimeAction::startWithTarget(target);
	_elapsed = 0.0f;
	_firstTick = true;
	_done = false;
}

//
// ScaleTo
//
ScaleTo* ScaleTo::create(float duration, float s)
{
    ScaleTo *scaleTo = new (std::nothrow) ScaleTo();
    if (scaleTo && scaleTo->initWithDuration(duration, s))
    {
        scaleTo->autorelease();
        return  scaleTo;
    }
    
    delete scaleTo;
    return nullptr;
}

ScaleTo* ScaleTo::create(float duration, float sx, float sy)
{
    ScaleTo *scaleTo = new (std::nothrow) ScaleTo();
    if (scaleTo && scaleTo->initWithDuration(duration, sx, sy))
    {
        scaleTo->autorelease();
        return scaleTo;
    }
    
    delete scaleTo;
    return nullptr;
}

ScaleTo* ScaleTo::create(float duration, float sx, float sy, float sz)
{
    ScaleTo *scaleTo = new (std::nothrow) ScaleTo();
    if (scaleTo && scaleTo->initWithDuration(duration, sx, sy, sz))
    {
        scaleTo->autorelease();
        return scaleTo;
    }
    
    delete scaleTo;
    return nullptr;
}

bool ScaleTo::initWithDuration(float duration, float s)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _endScaleX = s;
        _endScaleY = s;
        

        return true;
    }

    return false;
}

bool ScaleTo::initWithDuration(float duration, float sx, float sy)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _endScaleX = sx;
        _endScaleY = sy;
       

        return true;
    }

    return false;
}

bool ScaleTo::initWithDuration(float duration, float sx, float sy, float sz)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _endScaleX = sx;
        _endScaleY = sy;
       

        return true;
    }

    return false;
}

ScaleTo* ScaleTo::clone() const
{
    // no copy constructor
    return ScaleTo::create(_duration, _endScaleX, _endScaleY);
}

ScaleTo* ScaleTo::reverse() const
{
    OGASSERT(false, "reverse() not supported in ScaleTo");
    return nullptr;
}

void ScaleTo::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _startScaleX = target->getScaleX();
    _startScaleY = target->getScaleY();
    
    _deltaX = _endScaleX - _startScaleX;
    _deltaY = _endScaleY - _startScaleY;
 
}

void ScaleTo::update(float time)
{
    if (_target)
    {
        _target->setScaleX(_startScaleX + _deltaX * time);
        _target->setScaleY(_startScaleY + _deltaY * time);
       
    }
}

//
// ScaleBy
//

ScaleBy* ScaleBy::create(float duration, float s)
{
    ScaleBy *scaleBy = new (std::nothrow) ScaleBy();
    if (scaleBy && scaleBy->initWithDuration(duration, s))
    {
        scaleBy->autorelease();
        return scaleBy;
    }
    
    delete scaleBy;
    return nullptr;
}

ScaleBy* ScaleBy::create(float duration, float sx, float sy)
{
    ScaleBy *scaleBy = new (std::nothrow) ScaleBy();
    if (scaleBy && scaleBy->initWithDuration(duration, sx, sy, 1.f))
    {
        scaleBy->autorelease();
        return scaleBy;
    }
    
    delete scaleBy;
    return nullptr;
}

ScaleBy* ScaleBy::create(float duration, float sx, float sy, float sz)
{
    ScaleBy *scaleBy = new (std::nothrow) ScaleBy();
    if (scaleBy && scaleBy->initWithDuration(duration, sx, sy, sz))
    {
        scaleBy->autorelease();
        return scaleBy;
    }
    
    delete scaleBy;
    return nullptr;
}

ScaleBy* ScaleBy::clone() const
{
    // no copy constructor
    return ScaleBy::create(_duration, _endScaleX, _endScaleY);
}

void ScaleBy::startWithTarget(Node *target)
{
    ScaleTo::startWithTarget(target);
    _deltaX = _startScaleX * _endScaleX - _startScaleX;
    _deltaY = _startScaleY * _endScaleY - _startScaleY;
    
}

ScaleBy* ScaleBy::reverse() const
{
    return ScaleBy::create(_duration, 1 / _endScaleX, 1 / _endScaleY );
}
 

OG_END
