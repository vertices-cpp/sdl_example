#ifndef _ACTION_MANAGER_H_
#define _ACTION_MANAGER_H_

#include "OGAction.h"
#include "OGVector.h"
#include "OGRef.h"


OG_BEGIN


class Action;

struct _hashElement;

class   ActionManager : public Ref
{
public:
	/**
	 * @js ctor
	 */
	ActionManager(); 
	virtual ~ActionManager(); 
	virtual void addAction(Action *action, Node *target, bool paused);

	/** Removes all actions from all the targets.
	 */
	virtual void removeAllActions();

 
	virtual void removeAllActionsFromTarget(Node *target);
 
	virtual void removeAction(Action *action);
 
	virtual void removeActionByTag(int tag, Node *target);
 
	virtual void removeAllActionsByTag(int tag, Node *target);
 
	virtual void removeActionsByFlags(unsigned int flags, Node *target);
 
	virtual Action* getActionByTag(int tag, const Node *target) const;
 
	virtual ssize_t getNumberOfRunningActionsInTarget(const Node *target) const;
 
	virtual ssize_t getNumberOfRunningActions() const;


	/** Returns the numbers of actions that are running in a
	 *  certain target with a specific tag.
	 * Like getNumberOfRunningActionsInTarget Composable actions
	 * are counted as 1 action. Example:
	 * - If you are running 1 Sequence of 7 actions, it will return 1.
	 * - If you are running 7 Sequences of 2 actions, it will return 7.
	 *
	 * @param target    A certain target.
	 * @param tag       Tag that will be searched.
	 * @return  The numbers of actions that are running in a certain target
	 *          with a specific tag.
	 * @see getNumberOfRunningActionsInTarget
	 * @js NA
	 */
	virtual size_t getNumberOfRunningActionsInTargetByTag(const Node *target, int tag);


	/** Pauses the target: all running actions and newly added actions will be paused.
	 *
	 * @param target    A certain target.
	 */
	virtual void pauseTarget(Node *target);

	/** Resumes the target. All queued actions will be resumed.
	 *
	 * @param target    A certain target.
	 */
	virtual void resumeTarget(Node *target);

	/** Pauses all running actions, returning a list of targets whose actions were paused.
	 *
	 * @return  A list of targets whose actions were paused.
	 */
	virtual Vector<Node*> pauseAllRunningActions();

	/** Resume a set of targets (convenience function to reverse a pauseAllRunningActions call).
	 *
	 * @param targetsToResume   A set of targets need to be resumed.
	 */
	virtual void resumeTargets(const Vector<Node*>& targetsToResume);

	/** Main loop of ActionManager.
	 * @param dt    In seconds.
	 */
	virtual void update(float dt);

protected:
	// declared in ActionManager.m

	void removeActionAtIndex(ssize_t index, struct _hashElement *element);
	void deleteHashElement(struct _hashElement *element);
	void actionAllocWithHashElement(struct _hashElement *element);

protected:
	struct _hashElement    *_targets;
	struct _hashElement    *_currentTarget;
	bool            _currentTargetSalvaged;
};



OG_END

#endif