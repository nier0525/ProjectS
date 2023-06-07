#include "pch.h"
#include "IOCPListener.h"

IOCPListener::IOCPListener()
{
}

IOCPListener::~IOCPListener()
{
	Close();

	for (auto& acceptEvent : acceptEvents)
		SlimeAllocator::Free(acceptEvent);
	acceptEvents.clear();
}

HANDLE IOCPListener::GetHandle()
{
	return reinterpret_cast<HANDLE>(sock);
}

void IOCPListener::OnDispatch(IOCPEvent* iocpEvent, int32 bytes)
{
	if (IOCPEventType::ACCEPT != iocpEvent->type)
		CRASH("Invalid Event Type");

	ProcessAccept(static_cast<AcceptEvent*>(iocpEvent));
}

bool IOCPListener::Open(IOCPServerRef server)
{
	if (nullptr == server)
		return false;

	this->server = server;
	sock = SocketUtil::CreateSocket();

	if (INVALID_SOCKET == sock)
		return false;
	if (false == SocketUtil::SetReuseAddress(sock, true))
		return false;
	if (false == SocketUtil::Bind(sock, *server->GetAddress()))
		return false;
	if (false == SocketUtil::Listen(sock))
		return false;
	if (false == server->RegisterIOCP(shared_from_this()))
		return false;

	return true;
}

void IOCPListener::Close()
{
	SocketUtil::CloseSocket(sock);
}

void IOCPListener::DoAccept(int32 count)
{
	for (int32 i = 0; i < count; ++i)
		CreateAcceptEvent();
}

void IOCPListener::CreateAcceptEvent()
{
	auto acceptEvent = SlimeAllocator::Malloc<AcceptEvent>();
	acceptEvent->owner = shared_from_this();

	acceptEvents.push_back(acceptEvent);
	RegisterAccept(acceptEvent);
}

void IOCPListener::RegisterAccept(AcceptEvent* acceptEvent)
{
	auto session = server->NewSession();
	if (nullptr == session)
		CRASH("Create Session Failed");

	acceptEvent->Initialize();
	acceptEvent->session = session;

	DWORD bytes{ 0 };
	if (TRUE == SocketUtil::AcceptEx(sock, session->sock, session->receiveBuffer.GetWrite(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, static_cast<OVERLAPPED*>(acceptEvent)))
		return;

	const auto errorCode = GetLastError();
	if (WSA_IO_PENDING == errorCode)
		return;

	LOG(L"Accept Failed. Error Code [%u]", errorCode);
	RegisterAccept(acceptEvent);
}

void IOCPListener::ProcessAccept(AcceptEvent* acceptEvent)
{
	if (true == server->IsClosed())
		return;

	auto session = acceptEvent->session;
	if (nullptr == session)
		return RegisterAccept(acceptEvent);

	if (false == SocketUtil::SetUpdateAcceptSocket(session->sock, sock))
		return RegisterAccept(acceptEvent);

	SOCKADDR_IN address;
	auto length = static_cast<int32>(sizeof(address));

	if (SOCKET_ERROR == getpeername(session->sock, reinterpret_cast<sockaddr*>(&address), &length))
		return RegisterAccept(acceptEvent);

	session->SetAddress(address);
	session->ProcessAccept();

	acceptEvent->session = nullptr;
	RegisterAccept(acceptEvent);
}
