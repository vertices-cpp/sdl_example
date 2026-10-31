 

#include "2d/OGActionInstant.h"
#include "2d/OGNode.h"
#include "2d/OGSprite.h"

OG_BEGIN
//
// InstantAction
//
void ActionInstant::startWithTarget(Node *target)
{
    FiniteTimeAction::startWithTarget(target);
    _done = false;
}

bool ActionInstant::isDone() const
{
    return _done;
}

void ActionInstant::step(float /*dt*/)
{
    float updateDt = 1;
#if OG_ENABLE_SCRIPT_BINDING
//     if (_scriptType == kScriptTypeJavascript)
//     {
//         if (ScriptEngineManager::sendActionEventToJS(this, kActionUpdate, (void *)&updateDt))
//             return;
//     }
#endif
    update(updateDt);
}

void ActionInstant::update(float /*time*/)
{
    _done = true;
}

//
// Show
//

Show* Show::create() 
{
    Show* ret = new (std::nothrow) Show();

    if (ret)
    {
        ret->autorelease();
    }

    return ret;
}

void Show::update(float time)
{
    ActionInstant::update(time);
    _target->setVisible(true);
}

ActionInstant* Show::reverse() const
{
    return Hide::create();
}

Show* Show::clone() const
{
    // no copy constructor
    return Show::create();
}

//
// Hide
//
Hide * Hide::create() 
{
    Hide *ret = new (std::nothrow) Hide();

    if (ret)
    {
        ret->autorelease();
    }

    return ret;
}

void Hide::update(float time)
{
    ActionInstant::update(time);
    _target->setVisible(false);
}

ActionInstant *Hide::reverse() const
{
    return Show::create();
}

Hide* Hide::clone() const
{
    // no copy constructor
    return Hide::create();
}

//
// ToggleVisibility
//
ToggleVisibility * ToggleVisibility::create()
{
    ToggleVisibility *ret = new (std::nothrow) ToggleVisibility();

    if (ret)
    {
        ret->autorelease();
    }

    return ret;
}

void ToggleVisibility::update(float time)
{
    ActionInstant::update(time);
    _target->setVisible(!_target->isVisible());
}

ToggleVisibility * ToggleVisibility::reverse() const
{
    return ToggleVisibility::create();
}

ToggleVisibility * ToggleVisibility::clone() const
{
    // no copy constructor
    return ToggleVisibility::create();
}

//
// Remove Self
//
RemoveSelf * RemoveSelf::create(bool isNeedCleanUp /*= true*/) 
{
    RemoveSelf *ret = new (std::nothrow) RemoveSelf();

    if (ret && ret->init(isNeedCleanUp))
    {
        ret->autorelease();
    }

    return ret;
}

bool RemoveSelf::init(bool isNeedCleanUp)
{
    _isNeedCleanUp = isNeedCleanUp;
    return true;
}

void RemoveSelf::update(float time)
{
    ActionInstant::update(time);
    _target->removeFromParentAndCleanup(_isNeedCleanUp);
}

RemoveSelf *RemoveSelf::reverse() const
{
    return RemoveSelf::create(_isNeedCleanUp);
}

RemoveSelf * RemoveSelf::clone() const
{
    // no copy constructor
    return RemoveSelf::create(_isNeedCleanUp);
}

//
// FlipX
//

FlipX *FlipX::create(bool x)
{
    FlipX *ret = new (std::nothrow) FlipX();

    if (ret && ret->initWithFlipX(x))
    {
        ret->autorelease();
        return ret;
    }

    OG_SAFE_DELETE(ret);
    return nullptr;
}

bool FlipX::initWithFlipX(bool x)
{
    _flipX = x;
    return true;
}

void FlipX::update(float time)
{
    ActionInstant::update(time);
    static_cast<Sprite*>(_target)->setFlippedX(_flipX);
}

FlipX* FlipX::reverse() const
{
    return FlipX::create(!_flipX);
}

FlipX * FlipX::clone() const
{
    // no copy constructor
    return FlipX::create(_flipX);
}
//
// FlipY
//

FlipY * FlipY::create(bool y)
{
    FlipY *ret = new (std::nothrow) FlipY();

    if (ret && ret->initWithFlipY(y))
    {
        ret->autorelease();
        return ret;
    }

    OG_SAFE_DELETE(ret);
    return nullptr;
}

bool FlipY::initWithFlipY(bool y)
{
    _flipY = y;
    return true;
}

void FlipY::update(float time)
{
    ActionInstant::update(time);
    static_cast<Sprite*>(_target)->setFlippedY(_flipY);
}

FlipY* FlipY::reverse() const
{
    return FlipY::create(!_flipY);
}

FlipY * FlipY::clone() const
{
    // no copy constructor
    return FlipY::create(_flipY);
}

//
// Place
//

Place* Place::create(const Vec2& pos)
{
    Place *ret = new (std::nothrow) Place();

    if (ret && ret->initWithPosition(pos))
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool Place::initWithPosition(const Vec2& pos)
{
    _position = pos;
    return true;
}

Place * Place::clone() const
{
    // no copy constructor
    return Place::create(_position);
}

Place * Place::reverse() const
{
    // no reverse, just clone
    return this->clone();
}

void Place::update(float time)
{
    ActionInstant::update(time);
    _target->setPosition(_position);
}

//
// CallFunc
//

CallFunc * CallFunc::create(const std::function<void()> &func)
{
    CallFunc *ret = new (std::nothrow) CallFunc();

    if (ret && ret->initWithFunction(func) )
    {
        ret->autorelease();
        return ret;
    }

    OG_SAFE_DELETE(ret);
    return nullptr;
}

bool CallFunc::initWithFunction(const std::function<void()> &func)
{
    _function = func;
    return true;
}

CallFunc * CallFunc::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) CallFunc();
    if( _function )
        a->initWithFunction(_function);

    a->autorelease();
    return a;
}

CallFunc * CallFunc::reverse() const
{
    // no reverse here, just return a clone
    return this->clone();
}

void CallFunc::update(float time)
{
    ActionInstant::update(time);
    this->execute();
}

void CallFunc::execute()
{
    if( _function )
    {
        _function();
    }
}

//
// CallFuncN
//

CallFuncN * CallFuncN::create(const std::function<void(Node*)> &func)
{
    auto ret = new (std::nothrow) CallFuncN();

    if (ret && ret->initWithFunction(func) )
    {
        ret->autorelease();
        return ret;
    }

    OG_SAFE_DELETE(ret);
    return nullptr;
}

void CallFuncN::execute()
{
    if (_functionN)
    {
        _functionN(_target);
    }
}

bool CallFuncN::initWithFunction(const std::function<void (Node *)> &func)
{
    _functionN = func;
    return true;
}

CallFuncN * CallFuncN::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) CallFuncN();
    if( _functionN)
        a->initWithFunction(_functionN);

    a->autorelease();
    return a;
}

OG_END
