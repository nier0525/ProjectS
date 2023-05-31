#pragma once
#include "Job.h"
#include "LockQueue.h"

extern thread_local uint64 LMaxFlushJobTick;
extern thread_local bool LFlushedJobQueue;

DECLARE_SHARED(JobQueue);
class JobQueue : public enable_shared_from_this<JobQueue>
{
	friend class GlobalJobQueue;
	friend class JobTimerManager;

	using Jobs = LockQueue<JobRef>;
public:
	JobQueue();
	~JobQueue();

public:
	void Clear();
	
	void PushJob(Action&& job);
	template <typename CallBack, typename... Param>
	void PushJob(CallBack callback(Param...), Param... param);
	template <typename Owner, typename CallBack, typename... Param>
	void PushJob(CallBack(Owner::* callback)(Param...), Param... param);
	template <typename Owner, typename CallBack, typename... Param>
	void PushJob(shared_ptr<Owner> owner, CallBack(Owner::* callback)(Param...), Param... param);
	template <typename Owner, typename CallBack, typename... Param>
	void PushJob(Owner* owner, CallBack(Owner::* callback)(Param...), Param... param);

	void ReserveJob(uint64 interval, Action&& job);
	template <typename CallBack, typename... Param>
	void ReserveJob(uint64 interval, CallBack callback(Param...), Param... param);
	template <typename Owner, typename CallBack, typename... Param>
	void ReserveJob(uint64 interval, CallBack(Owner::* callback)(Param...), Param... param);
	template <typename Owner, typename CallBack, typename... Param>
	void ReserveJob(uint64 interval, shared_ptr<Owner> owner, CallBack(Owner::* callback)(Param...), Param... param);
	template <typename Owner, typename CallBack, typename... Param>
	void ReserveJob(uint64 interval, Owner* owner, CallBack(Owner::* callback)(Param...), Param... param);

private:
	void PushJob(JobRef job, bool invoke = true);
	void Flush();

private:
	Jobs jobs;
	atomic_int jobCount{ 0 };
};

template<typename CallBack, typename ...Param>
inline void JobQueue::PushJob(CallBack callback(Param...), Param ...param)
{
	PushJob(Job::MakeShared(callback, forward<Param>(param)...));
}

template<typename Owner, typename CallBack, typename ...Param>
inline void JobQueue::PushJob(CallBack(Owner::* callback)(Param...), Param ...param)
{	
	PushJob(Job::MakeShared(GET_SHARED(Owner), callback, forward<Param>(param)...));
}

template<typename Owner, typename CallBack, typename ...Param>
inline void JobQueue::PushJob(shared_ptr<Owner> owner, CallBack(Owner::* callback)(Param...), Param ...param)
{
	PushJob(Job::MakeShared(owner, callback, forward<Param>(param)...));
}

template<typename Owner, typename CallBack, typename ...Param>
inline void JobQueue::PushJob(Owner* owner, CallBack(Owner::* callback)(Param...), Param ...param)
{
	PushJob(Job::MakeShared(owner, callback, forward<Param>(param)...));
}

template<typename CallBack, typename ...Param>
inline void JobQueue::ReserveJob(uint64 interval, CallBack callback(Param...), Param ...param)
{
	GET_SINGLE(JobTimerManager)->Push(interval, shared_from_this(), Job::MakeShared(callback, forward<Param>(param)...));
}

template<typename Owner, typename CallBack, typename ...Param>
inline void JobQueue::ReserveJob(uint64 interval, CallBack(Owner::* callback)(Param...), Param ...param)
{
	GET_SINGLE(JobTimerManager)->Push(interval, shared_from_this(), Job::MakeShared(GET_SHARED(Owner), callback, forward<Param>(param)...));
}

template<typename Owner, typename CallBack, typename ...Param>
inline void JobQueue::ReserveJob(uint64 interval, shared_ptr<Owner> owner, CallBack(Owner::* callback)(Param...), Param ...param)
{
	GET_SINGLE(JobTimerManager)->Push(interval, shared_from_this(), Job::MakeShared(owner, callback, forward<Param>(param)...));
}

template<typename Owner, typename CallBack, typename ...Param>
inline void JobQueue::ReserveJob(uint64 interval, Owner* owner, CallBack(Owner::* callback)(Param...), Param ...param)
{
	GET_SINGLE(JobTimerManager)->Push(interval, shared_from_this(), Job::MakeShared(owner, callback, forward<Param>(param)...));
}

class GlobalJobQueue
{
	DECLARE_SINGLE(GlobalJobQueue)
	using JobQueues = LockQueue<JobQueueRef>;
private:
	GlobalJobQueue();
	~GlobalJobQueue();

public:
	void Clear();
	void Push(JobQueueRef jobQueue);
	void Flush();

private:
	JobQueues jobQueues;
	atomic_bool flushed{ false };
};
