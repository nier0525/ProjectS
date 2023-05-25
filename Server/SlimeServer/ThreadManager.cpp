#include "pch.h"
#include "ThreadManager.h"

thread_local int32 LThreadID{ 0 };

ThreadManager::ThreadManager()
{

}

ThreadManager::~ThreadManager()
{
	Joins();
}

void ThreadManager::Initialize()
{
	static atomic_int threadID{ 1 };
	LThreadID = threadID.fetch_add(1);
}

void ThreadManager::Destroy()
{
	LThreadID = 0;
}

void ThreadManager::Launch(function<void(void)> callback)
{
	lock_guard lock(latch);
	threads.push_back(thread([callback]()
	{
		Initialize();
		callback();
		Destroy();
	}));	
}

void ThreadManager::Joins()
{
	lock_guard lock(latch);
	for (auto& item : threads)
	{
		if (true == item.joinable())
			item.join();
	}
	threads.clear();
}
