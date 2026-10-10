 
#include "OGAction.h"
#include "OGNode.h"
#include "OGDirector.h"
//#include "ccUTF8.h"

OG_BEGIN
//
// Action Base Class
//

Action::Action()
:_originalTarget(nullptr)
,_target(nullptr)
,_tag(Action::INVALID_TAG)
,_flags(0)
{
}

Action::~Action()
{
    OGLOGINFO("deallocing Action: %p - tag: %i", this, _tag);
}

std::string Action::description() const
{
    return StringUtils::format("<Action | Tag = %d", _tag);
}

void Action::startWithTarget(Node *aTarget)
{
    _originalTarget = _target = aTarget;
}

void Action::stop()
{
    _target = nullptr;
}

bool Action::isDone() const
{
    return true;
}

void Action::step(float /*dt*/)
{
    OGLOG("[Action step]. override me");
}

void Action::update(float /*time*/)
{
    OGLOG("[Action update]. override me");
}



OG_END


