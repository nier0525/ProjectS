#include "pch.h"
#include "JobTimer.h"

JobTimer::JobTimer(JobQueueRef owner, JobRef job) : owner(owner), job(job)
{

}

JobTimerManager::JobTimerManager()
{

}

JobTimerManager::~JobTimerManager()
{
	Clear();
}

void JobTimerManager::Clear()
{
	WRITE_LOCK(latch);
	while (false == jobTimers.empty())
	{
		auto item = jobTimers.top();
		jobTimers.pop();

		JobTimer::Free(item.data);
	}
}

void JobTimerManager::Push(uint64 interval, JobQueueRef owner, JobRef job)
{
	WRITE_LOCK(latch);
	jobTimers.push({ JobTimer::Malloc(owner, job), GetTickCount64() + interval });
}

void JobTimerManager::Flush()
{
	if (true == flushed.exchange(true))
		return;

	auto tick = GetTickCount64();
	list<Item> items;
	{
		WRITE_LOCK(latch);
		while (false == jobTimers.empty())
		{
			auto& item = jobTimers.top();
			if (tick < item.tick)
				break;

			items.push_back(item);
			jobTimers.pop();
		}
	}

	for (auto& item : items)
	{
		if (auto owner = item.data->owner)
			owner->PushJob(item.data->job, false);

		JobTimer::Free(item.data);
	}
	flushed.store(false);
}
