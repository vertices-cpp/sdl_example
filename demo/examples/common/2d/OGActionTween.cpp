 

#include "2d/OGActionTween.h"

OG_BEGIN

ActionTween* ActionTween::create(float duration, const std::string& key, float from, float to)
{
    ActionTween* ret = new (std::nothrow) ActionTween();
    if (ret && ret->initWithDuration(duration, key, from, to))
    {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}

bool ActionTween::initWithDuration(float duration, const std::string& key, float from, float to)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _key    = key;
        _to       = to;
        _from     = from;
        return true;
    }

    return false;
}

ActionTween *ActionTween::clone() const
{
    return ActionTween::create(_duration, _key, _from, _to);
}

void ActionTween::startWithTarget(Node *target)
{
    OGASSERT(dynamic_cast<ActionTweenDelegate*>(target), "target must implement ActionTweenDelegate");
    ActionInterval::startWithTarget(target);
    _delta = _to - _from;
}

void ActionTween::update(float dt)
{
    dynamic_cast<ActionTweenDelegate*>(_target)->updateTweenAction(_to  - _delta * (1 - dt), _key);
}

ActionTween* ActionTween::reverse() const
{
    return ActionTween::create(_duration, _key, _to, _from);
}


OG_END
