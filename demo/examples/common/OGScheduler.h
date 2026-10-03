#ifndef _OG_SCHEDULER_H_
#define _OG_SCHEDULER_H_

#include <vector>
#include <algorithm>
#include "OGRef.h"

OG_BEGIN

class Node;

// 一个 schedule 条目
struct SchedulerEntry
{
	Node* target = nullptr;
	float interval = 0.0f;   // 0 = 每帧
	float elapsed = 0.0f;
	bool  paused = false;
	bool  markedForRemoval = false;
};

class Scheduler
{
public:
	Instance(Scheduler);

	// 添加：每帧调用 target->update(dt)
	void scheduleUpdate(Node* target, float interval = 0.0f);

	// 移除
	void unscheduleUpdate(Node* target);

	// 每帧调用
	void update(float dt);

	// 清空
	void clear();

private:
	std::vector<SchedulerEntry> _entries;
};

OG_END

#endif