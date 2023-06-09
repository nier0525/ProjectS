#include "pch.h"
#include <ThreadManager.h>
#include <Dumper.h>
#include <IOCPModule.h>
#include "Dummy.h"

void StartClient(IOCPClientRef client)
{
	while (true)
	{
		GET_SINGLE(JobManager)->UpdateTick(60);
		client->OnDispatch(10);
		GET_SINGLE(JobManager)->Flush();
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

	SocketUtil::Initialize();

	auto client = IOCPClient::MakeShared(SocketAddress(L"127.0.0.1", 8050), Dummy::MakeShared<>);
	for (int i = 0; i < 6; ++i)
		GET_SINGLE(ThreadManager)->Launch([&client]() { StartClient(client); });

	client->Connect();

	LOG(L"Start Dummy Client");
	GET_SINGLE(ThreadManager)->Joins();
}