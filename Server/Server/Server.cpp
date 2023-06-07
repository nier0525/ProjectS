#include "pch.h"
#include <ThreadManager.h>
#include <Dumper.h>
#include <IOCPModule.h>
#include "User.h"

void StartServer(IOCPServerRef server)
{
	while (true)
	{
		GET_SINGLE(JobManager)->UpdateTick(60);
		server->OnDispatch(10);
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

	auto server = IOCPServer::MakeShared(SocketAddress(L"127.0.0.1", 8050), User::MakeShared<>);
	server->Open();
	server->Listen(10);

	for (int i = 0; i < 4; ++i)
		GET_SINGLE(ThreadManager)->Launch([&server]() { StartServer(server); });

	LOG(L"Open Server");
	GET_SINGLE(ThreadManager)->Joins();
}