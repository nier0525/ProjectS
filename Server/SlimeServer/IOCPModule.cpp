#include "pch.h"
#include "IOCPModule.h"

IOCPModule::IOCPModule(IOCPType type, SocketAddress address, SessionFactory sessionFactory) : type(type), address(address), sessionFactory(sessionFactory)
{
	handler = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	if (INVALID_HANDLE_VALUE == handler)
		CRASH("Create IOCP Handler Failed");
}

IOCPModule::~IOCPModule()
{
	CloseHandle(handler);
}

IOCPSessionRef IOCPModule::NewSession()
{
	auto session = sessionFactory();
	if (nullptr == session)
		return nullptr;
	if (false == RegisterIOCP(session))
		return nullptr;
	return session;
}

void IOCPModule::Close()
{
	isClosed.store(true);
}

bool IOCPModule::RegisterIOCP(IOCPContextRef context)
{
	return CreateIoCompletionPort(context->GetHandle(), handler, NULL, NULL);
}

bool IOCPModule::OnDispatch(uint32 timeoutMS)
{
	IOCPEvent* iocpEvent{ nullptr };
	DWORD bytes{ 0 };
	ULONG_PTR key{ 0 };

	if (TRUE == GetQueuedCompletionStatus(handler, &bytes, &key, reinterpret_cast<OVERLAPPED**>(&iocpEvent), timeoutMS))
	{
		iocpEvent->owner->OnDispatch(iocpEvent, bytes);
		return true;
	}

	auto errorCode = GetLastError();
	switch (errorCode)
	{
	case WAIT_TIMEOUT:
		return true;

	default:
		iocpEvent->owner->OnDispatch(iocpEvent, bytes);
		break;
	}

	switch (errorCode)
	{
	case ERROR_NETNAME_DELETED:
	case ERROR_IO_PENDING:
		break;

	default:
		LOG(L"IOCP Dispatch Failed.. Error Code [%u]", errorCode);
		return false;
	}
	return true;
}

IOCPServer::IOCPServer(SocketAddress address, SessionFactory sessionFactory) : IOCPModule(IOCPType::SERVER, address, sessionFactory)
{

}

IOCPServer::~IOCPServer()
{

}

void IOCPServer::Open()
{

}

void IOCPServer::Listen(int32 listenCount)
{

}

IOCPClient::IOCPClient(SocketAddress address, SessionFactory sessionFactory) : IOCPModule(IOCPType::CLIENT, address, sessionFactory)
{

}

IOCPClient::~IOCPClient()
{

}

bool IOCPClient::Connect(uint32 connectCount)
{
	if (true == IsClosed())
		return false;

	for (uint32 i = 0; i < connectCount; ++i)
	{
		auto session = NewSession();
		if (nullptr == session)
			return false;

		if (false == session->DoConnect(address))
			return false;
	}
	return true;
}
