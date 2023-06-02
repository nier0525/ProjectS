#include "pch.h"
#include "IOCPSession.h"

IOCPSession::IOCPSession() : receiveBuffer(MAX_PACKET_SIZE, 10), sock(SocketUtil::CreateSocket())
{

}

IOCPSession::~IOCPSession()
{
	SocketUtil::CloseSocket(sock);
	isConnected.store(false);

	registeredSend.store(false);
	sendQueue.Clear();
	receiveBuffer.Clear();
}

HANDLE IOCPSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(sock);
}

void IOCPSession::OnDispatch(IOCPEvent* iocpEvent, int32 bytes)
{
	if (nullptr == iocpEvent)
		return LOG(L"Invalid Event");

	switch (iocpEvent->type)
	{	
	case IOCPEventType::CONNECT: return ProcessConnect();
	case IOCPEventType::DISCONNECT: return ProcessDisconnect();
	case IOCPEventType::SEND: return ProcessSend(bytes);
	case IOCPEventType::RECEIVE: return ProcessReceive(bytes);
	}
	LOG(L"Unknown Event Type %u", iocpEvent->type);
}

bool IOCPSession::DoConnect(SocketAddress address)
{
	if (true == IsConnected())
		return false;

	SetAddress(address);
	return RegisterConnect();
}

void IOCPSession::DoDisconnect()
{
	if (true == isConnected.exchange(false))
		RegisterDisconnect();
}

void IOCPSession::DoDisconnect(const wchar* reason, ...)
{
	wchar log[0x200]{};

	va_list args;
	va_start(args, reason);
	vswprintf_s(log, 0x200, reason, args);
	va_end(args);

	LOG(L"Force Disconnect [%s]", log);
	DoDisconnect();
}

void IOCPSession::DoSend(SendBufferRef sendBuffer)
{
	if (false == IsConnected())
		return;

	sendQueue.Push(sendBuffer);
	if (false == registeredSend.exchange(true))
		RegisterSend();
}

bool IOCPSession::IsPending(int32 errorCode)
{
	return WSA_IO_PENDING == errorCode;
}

void IOCPSession::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNABORTED:
	case WSAECONNRESET:
	case WSAENOTCONN:
		return DoDisconnect(L"IOCP Handle Error %u", errorCode);
	}
	LOG(L"IOCP Handle Error %u", errorCode);
}

bool IOCPSession::RegisterConnect()
{
	if (true == IsConnected())
		return false;
	if (false == SocketUtil::SetReuseAddress(sock, true))
		return false;
	if (false == SocketUtil::BindAny(sock))
		return false;

	connectEvent.Initialize();
	connectEvent.owner = shared_from_this();

	DWORD bytes{ 0 };
	auto& address = this->address.GetAddress();
	
	if (TRUE == SocketUtil::ConnectEx(sock, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr), nullptr, 0, &bytes, &connectEvent))
		return true;

	auto errorCode = GetLastError();
	if (true == IsPending(errorCode))
		return true;

	connectEvent.owner = nullptr;
	HandleError(errorCode);

	return false;
}

bool IOCPSession::RegisterDisconnect()
{
	disconnectEvent.Initialize();
	disconnectEvent.owner = shared_from_this();

	if (TRUE == SocketUtil::DisconnectEx(sock, &disconnectEvent, TF_REUSE_SOCKET, 0))
		return true;

	auto errorCode = GetLastError();
	if (true == IsPending(errorCode))
		return true;

	ProcessDisconnect();
	HandleError(errorCode);

	return false;
}

bool IOCPSession::RegisterSend()
{
	if (false == IsConnected())
		return false;

	sendEvent.Initialize();
	sendEvent.owner = shared_from_this();

	list<SendBufferRef> sendBuffers;
	sendQueue.PopAll(sendBuffers);

	vector<WSABUF> buffers;
	buffers.reserve(sendBuffers.size());

	for (const auto & sendBuffer : sendBuffers)
	{
		if (nullptr == sendBuffer)
			continue;

		WSABUF buffer;
		buffer.buf = reinterpret_cast<int8*>(sendBuffer->GetBuffer());
		buffer.len = sendBuffer->GetOffset();

		buffers.push_back(buffer);
	}

	if (true == buffers.empty())
		return true;

	DWORD bytes{ 0 };
	if (SOCKET_ERROR != WSASend(sock, buffers.data(), static_cast<DWORD>(buffers.size()), &bytes, 0, &sendEvent, nullptr))
		return true;

	auto errorCode = GetLastError();
	if (true == IsPending(errorCode))
		return true;

	sendEvent.owner = nullptr;
	HandleError(errorCode);

	if (true == sendQueue.Empty())
		registeredSend.store(false);
	else
		RegisterSend();

	return false;
}

bool IOCPSession::RegisterReceive()
{
	if (false == IsConnected())
		return false;

	receiveEvent.Initialize();
	receiveEvent.owner = shared_from_this();

	WSABUF buffer;
	buffer.buf = reinterpret_cast<int8*>(receiveBuffer.GetWrite());
	buffer.len = receiveBuffer.GetFreeSize();

	DWORD bytes{ 0 };
	DWORD flag{ 0 };

	if (SOCKET_ERROR != WSARecv(sock, &buffer, 1, &bytes, &flag, &receiveEvent, nullptr))
		return true;

	auto errorCode = GetLastError();
	if (true == IsPending(errorCode))
		return true;

	receiveEvent.owner = nullptr;
	HandleError(errorCode);

	return false;
}

void IOCPSession::ProcessAccept()
{
	isConnected.store(true);
	GET_SINGLE(IOCPSessionManager)->Insert(GET_SHARED(IOCPSession));

	OnAccepted();
	RegisterReceive();
}

void IOCPSession::ProcessConnect()
{
	isConnected.store(true);
	GET_SINGLE(IOCPSessionManager)->Insert(GET_SHARED(IOCPSession));
	connectEvent.owner = nullptr;

	OnConnected();
	RegisterReceive();
}

void IOCPSession::ProcessDisconnect()
{
	isConnected.store(false);
	GET_SINGLE(IOCPSessionManager)->Remove(GET_SHARED(IOCPSession));

	OnDisconnected();
	disconnectEvent.owner = nullptr;
}

void IOCPSession::ProcessSend(int32 bytes)
{
	sendEvent.owner = nullptr;

	if (0 == bytes)
		return DoDisconnect();

	OnSend(bytes);

	if (true == sendQueue.Empty())
		registeredSend.store(false);
	else
		RegisterSend();
}

void IOCPSession::ProcessReceive(int32 bytes)
{
	receiveEvent.owner = nullptr;

	if (0 == bytes)
		return DoDisconnect();

	if (false == receiveBuffer.Write(bytes))
		return DoDisconnect(L"Receive Failed");

	auto dataSize = receiveBuffer.GetSize();
	auto length = OnReceive(receiveBuffer.GetRead(), dataSize);

	if (0 >= dataSize)
		return DoDisconnect();
	if (dataSize < length || false == receiveBuffer.Read(length))
		return DoDisconnect(L"Receive Failed");

	receiveBuffer.Clear();
	RegisterReceive();
}

IOCPSessionManager::IOCPSessionManager()
{

}

IOCPSessionManager::~IOCPSessionManager()
{
	Clear();
}

void IOCPSessionManager::Clear()
{
	WRITE_LOCK(latch);
	for (auto & session : sessions)
		session->DoDisconnect();
}

bool IOCPSessionManager::Insert(IOCPSessionRef session)
{
	WRITE_LOCK(latch);
	if (sessions.find(session) != sessions.end())
		return false;

	sessions.insert(session);
	return true;
}

void IOCPSessionManager::Remove(IOCPSessionRef session)
{
	WRITE_LOCK(latch);
	if (sessions.find(session) != sessions.end())
		sessions.erase(session);
}
