#include "pch.h"
#include <ThreadManager.h>
#include <Dumper.h>

class MyClass
{	
public:
	void SetValue(int32 value) { this->value = value; }
	int32 GetValue() const { return value; }

private:
	int32 value{ 0 };

	USE_ALLOCATE(MyClass)
};

SpinLock latch;
int value{ 0 };

void DisplayLog(const wchar* log) { wcout << log; }

void Func()
{
	WRITE_LOCK(latch);
	for (int i = 0; i < 100000; ++i)
		++value;
}

void Func2()
{
	READ_LOCK(latch);
	this_thread::sleep_for(100ms);
}


int main()
{
	MemoryLeakDetector::Launch();
	Dumper::Launch();

	GET_SINGLE(Logger)->Initialize("logs", DisplayLog, false);
	LOG(L"Hello %u", 5);
	LOG(L"C++");
}