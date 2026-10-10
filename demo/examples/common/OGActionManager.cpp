#include "OGActionManager.h"
#include "OGNode.h"
#include "OGAction.h"
#include "OGScheduler.h"
#include "ogMacros.h"
#include "ogCArray.h"
#include "uthash.h"
 
OG_BEGIN

typedef struct _hashElement
{
	struct _ogArray     *actions;
	Node                *target;
	int                 actionIndex;
	Action              *currentAction;
	bool                currentActionSalvaged;
	bool                paused;
	UT_hash_handle      hh;
} tHashElement;

ActionManager::ActionManager()
	: _targets(nullptr),
	_currentTarget(nullptr),
	_currentTargetSalvaged(false)
{

}

ActionManager::~ActionManager()
{
	OGLOGINFO("deallocing ActionManager: %p", this);

	removeAllActions();
}

// private

void ActionManager::deleteHashElement(tHashElement *element)
{
	ogArrayFree(element->actions);
	HASH_DEL(_targets, element);
	element->target->release();
	free(element);
}

void ActionManager::actionAllocWithHashElement(tHashElement *element)
{
	// 4 actions per Node by default
	if (element->actions == nullptr)
	{
		element->actions = ogArrayNew(4);
	}
	else
		if (element->actions->num == element->actions->max)
		{
			ogArrayDoubleCapacity(element->actions);
		}

}

void ActionManager::removeActionAtIndex(ssize_t index, tHashElement *element)
{
	Action *action = static_cast<Action*>(element->actions->arr[index]);

	if (action == element->currentAction && (!element->currentActionSalvaged))
	{
		element->currentAction->retain();
		element->currentActionSalvaged = true;
	}

	ogArrayRemoveObjectAtIndex(element->actions, index, true);

	// update actionIndex in case we are in tick. looping over the actions
	if (element->actionIndex >= index)
	{
		element->actionIndex--;
	}

	if (element->actions->num == 0)
	{
		if (_currentTarget == element)
		{
			_currentTargetSalvaged = true;
		}
		else
		{
			deleteHashElement(element);
		}
	}
}

// pause / resume

void ActionManager::pauseTarget(Node *target)
{
	tHashElement *element = nullptr;
	HASH_FIND_PTR(_targets, &target, element);
	if (element)
	{
		element->paused = true;
	}
}

void ActionManager::resumeTarget(Node *target)
{
	tHashElement *element = nullptr;
	HASH_FIND_PTR(_targets, &target, element);
	if (element)
	{
		element->paused = false;
	}
}

Vector<Node*> ActionManager::pauseAllRunningActions()
{
	Vector<Node*> idsWithActions;

	for (tHashElement *element = _targets; element != nullptr; element = (tHashElement *)element->hh.next)
	{
		if (!element->paused)
		{
			element->paused = true;
			idsWithActions.pushBack(element->target);
		}
	}

	return idsWithActions;
}

void ActionManager::resumeTargets(const Vector<Node*>& targetsToResume)
{
	for (const auto &node : targetsToResume)
	{
		this->resumeTarget(node);
	}
}

// run

void ActionManager::addAction(Action *action, Node *target, bool paused)
{
	OGASSERT(action != nullptr, "action can't be nullptr!");
	OGASSERT(target != nullptr, "target can't be nullptr!");
	if (action == nullptr || target == nullptr)
		return;

	tHashElement *element = nullptr;
	// we should convert it to Ref*, because we save it as Ref*
	Ref *tmp = target;
	HASH_FIND_PTR(_targets, &tmp, element);
	if (!element)
	{
		element = (tHashElement*)calloc(sizeof(*element), 1);
		element->paused = paused;
		target->retain();
		element->target = target;
		HASH_ADD_PTR(_targets, target, element);
	}

	actionAllocWithHashElement(element);

	OGASSERT(!ogArrayContainsObject(element->actions, action), "action already be added!");
	ogArrayAppendObject(element->actions, action);

	action->startWithTarget(target);
}

// remove

void ActionManager::removeAllActions()
{
	for (tHashElement *element = _targets; element != nullptr; )
	{
		auto target = element->target;
		element = (tHashElement*)element->hh.next;
		removeAllActionsFromTarget(target);
	}
}

void ActionManager::removeAllActionsFromTarget(Node *target)
{
	// explicit null handling
	if (target == nullptr)
	{
		return;
	}

	tHashElement *element = nullptr;
	HASH_FIND_PTR(_targets, &target, element);
	if (element)
	{
		if (ogArrayContainsObject(element->actions, element->currentAction) && (!element->currentActionSalvaged))
		{
			element->currentAction->retain();
			element->currentActionSalvaged = true;
		}

		ogArrayRemoveAllObjects(element->actions);
		if (_currentTarget == element)
		{
			_currentTargetSalvaged = true;
		}
		else
		{
			deleteHashElement(element);
		}
	}
}

void ActionManager::removeAction(Action *action)
{
	// explicit null handling
	if (action == nullptr)
	{
		return;
	}

	tHashElement *element = nullptr;
	Ref *target = action->getOriginalTarget();
	HASH_FIND_PTR(_targets, &target, element);
	if (element)
	{
		auto i = ogArrayGetIndexOfObject(element->actions, action);
		if (i != -1)
		{
			removeActionAtIndex(i, element);
		}
	}
}

void ActionManager::removeActionByTag(int tag, Node *target)
{
	OGASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");
	OGASSERT(target != nullptr, "target can't be nullptr!");
	if (target == nullptr)
	{
		return;
	}

	tHashElement *element = nullptr;
	HASH_FIND_PTR(_targets, &target, element);

	if (element)
	{
		auto limit = element->actions->num;
		for (int i = 0; i < limit; ++i)
		{
			Action *action = static_cast<Action*>(element->actions->arr[i]);

			if (action->getTag() == (int)tag && action->getOriginalTarget() == target)
			{
				removeActionAtIndex(i, element);
				break;
			}
		}
	}
}

void ActionManager::removeAllActionsByTag(int tag, Node *target)
{
	OGASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");
	OGASSERT(target != nullptr, "target can't be nullptr!");
	if (target == nullptr)
	{
		return;
	}

	tHashElement *element = nullptr;
	HASH_FIND_PTR(_targets, &target, element);

	if (element)
	{
		auto limit = element->actions->num;
		for (int i = 0; i < limit;)
		{
			Action *action = static_cast<Action*>(element->actions->arr[i]);

			if (action->getTag() == (int)tag && action->getOriginalTarget() == target)
			{
				removeActionAtIndex(i, element);
				--limit;
			}
			else
			{
				++i;
			}
		}
	}
}

void ActionManager::removeActionsByFlags(unsigned int flags, Node *target)
{
	if (flags == 0)
	{
		return;
	}
	OGASSERT(target != nullptr, "target can't be nullptr!");
	if (target == nullptr)
	{
		return;
	}

	tHashElement *element = nullptr;
	HASH_FIND_PTR(_targets, &target, element);

	if (element)
	{
		auto limit = element->actions->num;
		for (int i = 0; i < limit;)
		{
			Action *action = static_cast<Action*>(element->actions->arr[i]);

			if ((action->getFlags() & flags) != 0 && action->getOriginalTarget() == target)
			{
				removeActionAtIndex(i, element);
				--limit;
			}
			else
			{
				++i;
			}
		}
	}
}

// get

// FIXME: Passing "const O *" instead of "const O&" because HASH_FIND_IT requires the address of a pointer
// and, it is not possible to get the address of a reference
Action* ActionManager::getActionByTag(int tag, const Node *target) const
{
	OGASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");

	tHashElement *element = nullptr;
	HASH_FIND_PTR(_targets, &target, element);

	if (element)
	{
		if (element->actions != nullptr)
		{
			auto limit = element->actions->num;
			for (int i = 0; i < limit; ++i)
			{
				Action *action = static_cast<Action*>(element->actions->arr[i]);

				if (action->getTag() == (int)tag)
				{
					return action;
				}
			}
		}
	}

	return nullptr;
}

// FIXME: Passing "const O *" instead of "const O&" because HASH_FIND_IT requires the address of a pointer
// and, it is not possible to get the address of a reference
ssize_t ActionManager::getNumberOfRunningActionsInTarget(const Node *target) const
{
	tHashElement *element = nullptr;
	HASH_FIND_PTR(_targets, &target, element);
	if (element)
	{
		return element->actions ? element->actions->num : 0;
	}

	return 0;
}

// FIXME: Passing "const O *" instead of "const O&" because HASH_FIND_IT requires the address of a pointer
// and, it is not possible to get the address of a reference
size_t ActionManager::getNumberOfRunningActionsInTargetByTag(const Node *target,
	int tag)
{
	OGASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");

	tHashElement *element = nullptr;
	HASH_FIND_PTR(_targets, &target, element);

	if (!element || !element->actions)
		return 0;

	int count = 0;
	auto limit = element->actions->num;
	for (int i = 0; i < limit; ++i)
	{
		auto action = static_cast<Action*>(element->actions->arr[i]);
		if (action->getTag() == tag)
			++count;
	}

	return count;
}

ssize_t ActionManager::getNumberOfRunningActions() const
{
	ssize_t count = 0;
	struct _hashElement* element = nullptr;
	struct _hashElement* tmp = nullptr;
	HASH_ITER(hh, _targets, element, tmp)
	{
		count += (element->actions ? element->actions->num : 0);
	}
	return count;
}

// main loop
void ActionManager::update(float dt)
{
	for (tHashElement *elt = _targets; elt != nullptr; )
	{
		_currentTarget = elt;
		_currentTargetSalvaged = false;

		if (!_currentTarget->paused)
		{
			// The 'actions' MutableArray may change while inside this loop.
			for (_currentTarget->actionIndex = 0; _currentTarget->actionIndex < _currentTarget->actions->num;
				_currentTarget->actionIndex++)
			{
				_currentTarget->currentAction = static_cast<Action*>(_currentTarget->actions->arr[_currentTarget->actionIndex]);
				if (_currentTarget->currentAction == nullptr)
				{
					continue;
				}

				_currentTarget->currentActionSalvaged = false;

				_currentTarget->currentAction->step(dt);

				if (_currentTarget->currentActionSalvaged)
				{
					// The currentAction told the node to remove it. To prevent the action from
					// accidentally deallocating itself before finishing its step, we retained
					// it. Now that step is done, it's safe to release it.
					_currentTarget->currentAction->release();
				}
				else
					if (_currentTarget->currentAction->isDone())
					{
						_currentTarget->currentAction->stop();

						Action *action = _currentTarget->currentAction;
						// Make currentAction nil to prevent removeAction from salvaging it.
						_currentTarget->currentAction = nullptr;
						removeAction(action);
					}

				_currentTarget->currentAction = nullptr;
			}
		}

		// elt, at this moment, is still valid
		// so it is safe to ask this here (issue #490)
		elt = (tHashElement*)(elt->hh.next);

		// only delete currentTarget if no actions were scheduled during the cycle (issue #481)
		if (_currentTargetSalvaged && _currentTarget->actions->num == 0)
		{
			deleteHashElement(_currentTarget);
		}
		//if some node reference 'target', it's reference count >= 2 (issues #14050)
		else if (_currentTarget->target->getReferenceCount() == 1)
		{
			deleteHashElement(_currentTarget);
		}
	}

	// issue #635
	_currentTarget = nullptr;
}





OG_END