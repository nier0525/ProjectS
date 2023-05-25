#pragma once

extern thread_local int32 LThreadID;

class ThreadManager
{
	DECLARE_SINGLE(ThreadManager)
private:
	ThreadManager();
	~ThreadManager();

public:
	static void Initialize();
	static void Destroy();

	void Launch(function<void(void)> callback);
	void Joins();

private:
	mutex latch;
	list<thread> threads;
};
