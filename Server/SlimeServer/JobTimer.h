#pragma once

struct JobTimer
{
public:
	JobTimer(JobQueueRef owner, JobRef job);

public:
	JobQueueRef owner;
	JobRef job;

	USE_ALLOCATE(JobTimer)
};

class JobTimerManager
{
	DECLARE_SINGLE(JobTimerManager)

	struct Item
	{
		bool operator>(const Item& other) const { return tick > other.tick; }

		JobTimer* data{ nullptr };
		uint64 tick;
	};
	using JobTimers = priority_queue<Item, vector<Item>, greater<Item>>;
private:
	JobTimerManager();
	~JobTimerManager();

public:
	void Clear();
	void Push(uint64 interval, JobQueueRef owner, JobRef job);
	void Flush();

private:
	SpinLock latch;
	JobTimers jobTimers;
	atomic_bool flushed{ false };
};

