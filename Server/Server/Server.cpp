#include "pch.h"
#include <ThreadManager.h>
#include <Dumper.h>
#include <JobQueue.h>
#include <SendBuffer.h>

void StartServer()
{
	while (true)
	{
		LMaxFlushJobTick = GetTickCount64() + 60;
		GET_SINGLE(GlobalJobQueue)->Flush();
		GET_SINGLE(JobTimerManager)->Flush();

		this_thread::yield();
	}
}

void StartLogger()
{
	while (true)
	{
		GET_SINGLE(Logger)->Flush();
		this_thread::sleep_for(100ms);
	}
}

int main()
{
	MemoryLeakDetector::Launch();
	Dumper::Launch();

	GET_SINGLE(Logger)->Initialize("logs", [](const wchar* log) { wcout << log; }, false);
	GET_SINGLE(ThreadManager)->Launch(StartLogger);

	for (int i = 0; i < 4; ++i)
		GET_SINGLE(ThreadManager)->Launch(StartServer);

	LOG(L"Open Server");
	GET_SINGLE(ThreadManager)->Joins();
}