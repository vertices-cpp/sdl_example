#include "OGScheduler.h"
#include "OGNode.h"

OG_BEGIN

void Scheduler::scheduleUpdate(Node* target, float interval)
{
	if (!target) return;

	// 已存在就不重复加
	for (auto& e : _entries)
	{
		if (e.target == target)
		{
			e.interval = interval;
			e.elapsed = 0.0f;
			e.paused = false;
			e.markedForRemoval = false;
			return;
		}
	}

	SchedulerEntry e;
	e.target = target;
	e.interval = interval;
	_entries.push_back(e);
}

void Scheduler::unscheduleUpdate(Node* target)
{
	for (auto& e : _entries)
	{
		if (e.target == target)
		{
			e.markedForRemoval = true;
			break;
		}
	}
}

void Scheduler::update(float dt)
{
	// 先处理：如果 target 被删了，也移除（可选，看 Node 生命周期管理）
	for (auto& e : _entries)
	{
		if (e.markedForRemoval || e.paused) continue;

		if (e.interval <= 0.0f)
		{
			// 每帧
			e.target->update(dt);
		}
		else
		{
			e.elapsed += dt;
			if (e.elapsed >= e.interval)
			{
				e.elapsed -= e.interval;
				e.target->update(e.interval);
			}
		}
	}

	// 移除标记的
	_entries.erase(
		std::remove_if(_entries.begin(), _entries.end(),
			[](const SchedulerEntry& e) { return e.markedForRemoval; }),
		_entries.end());
}

void Scheduler::clear()
{
	_entries.clear();
}

OG_END