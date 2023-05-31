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

void DisplayLog(const wchar* log)
{
	wcout << log; 
}

void StartLogger()
{
	while (true)
	{
		GET_SINGLE(Logger)->Flush();
		this_thread::sleep_for(100ms);
	}
}

DECLARE_SHARED(MyClass);
class MyClass : public JobQueue
{	
public:
	void IncrementValue() { ++value; }
	void SetValue(int32 value) { this->value = value; }
	int32 GetValue() const { return value; }

	void PrintValue() { LOG(L"%u", value); }

private:
	int32 value{ 0 };

	USE_ALLOCATE(MyClass)
};

SpinLock latch;
int value{ 0 };

MyClassRef c{ MyClass::MakeShared() };

void Func(int i)
{
	LMaxFlushJobTick = GetTickCount64() + 60;

	for (int i = 0; i < 10000; ++i)	
		c->PushJob(&MyClass::IncrementValue);		
	c->ReserveJob(1000 * i, &MyClass::PrintValue);
}

int main()
{
	MemoryLeakDetector::Launch();
	Dumper::Launch();

	GET_SINGLE(Logger)->Initialize("logs", DisplayLog, false);
	GET_SINGLE(ThreadManager)->Launch(StartLogger);

	for (int i = 0; i < 4; ++i)
		GET_SINGLE(ThreadManager)->Launch(StartServer);

	GET_SINGLE(ThreadManager)->Joins();
}