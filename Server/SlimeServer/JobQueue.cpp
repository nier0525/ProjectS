#include "pch.h"
#include "JobQueue.h"

thread_local uint64 LMaxFlushJobTick{ 0 };
thread_local bool LFlushedJobQueue{ false };

JobQueue::JobQueue()
{

}

JobQueue::~JobQueue()
{
	Clear();
}

void JobQueue::Clear()
{
	jobs.Clear();
	jobCount.store(0);
}

void JobQueue::PushJob(Action&& job)
{
	PushJob(Job::MakeShared(move(job)));
}

void JobQueue::ReserveJob(uint64 interval, Action&& job)
{
	GET_SINGLE(JobTimerManager)->Push(interval, shared_from_this(), Job::MakeShared( move(job)));
}

void JobQueue::PushJob(JobRef job, bool invoke)
{
	auto count = jobCount.fetch_add(1);
	jobs.Push(job);

	if (0 != count)
		return;

	if (false == LFlushedJobQueue && true == invoke)
		Flush();
	else
		GET_SINGLE(GlobalJobQueue)->Push(shared_from_this());
}

void JobQueue::Flush()
{
	LFlushedJobQueue = true;	
	while (true)
	{
		if (GetTickCount64() > LMaxFlushJobTick)
		{
			GET_SINGLE(GlobalJobQueue)->Push(shared_from_this());
			break;
		}

		list<JobRef> jobList;
		jobs.PopAll(jobList);

		auto completedCount = static_cast<uint32>(jobList.size());

		for (auto & job : jobList)
			job->Invoke();

		if (completedCount == jobCount.fetch_sub(completedCount))
			break;
	}
	LFlushedJobQueue = false;
}

GlobalJobQueue::GlobalJobQueue()
{

}

GlobalJobQueue::~GlobalJobQueue()
{

}

void GlobalJobQueue::Clear()
{
	jobQueues.Clear();
}

void GlobalJobQueue::Push(JobQueueRef jobQueue)
{
	jobQueues.Push(jobQueue);
}

void GlobalJobQueue::Flush()
{
	if (true == flushed.exchange(true))
		return;

	while (true)
	{
		if (GetTickCount64() > LMaxFlushJobTick)
			break;

		auto jobQueue = jobQueues.Pop();
		if (nullptr == jobQueue)
			break;

		jobQueue->Flush();
	}
	flushed.store(false);
}
