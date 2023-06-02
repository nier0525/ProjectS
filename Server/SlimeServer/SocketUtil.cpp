#include "pch.h"
#include "SocketUtil.h"

LPFN_ACCEPTEX SocketUtil::AcceptEx{ nullptr };
LPFN_CONNECTEX SocketUtil::ConnectEx{ nullptr };
LPFN_DISCONNECTEX SocketUtil::DisconnectEx{ nullptr };

void SocketUtil::Initialize()
{
	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
		CRASH("WSA Startup Failed");

	auto dummySocket = CreateSocket();
	if (false == BindWindowFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<void**>(&AcceptEx)))
		CRASH("Bind AcceptEX Failed");
	if (false == BindWindowFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<void**>(&ConnectEx)))
		CRASH("Bind ConnectEX Failed");
	if (false == BindWindowFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<void**>(&DisconnectEx)))
		CRASH("Bind DIsconnectDX Failed");

	CloseSocket(dummySocket);
}

void SocketUtil::Clear()
{
	WSACleanup();
}

SOCKET SocketUtil::CreateSocket()
{
	return WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

void SocketUtil::CloseSocket(SOCKET& sock)
{
	if (INVALID_SOCKET != sock)
		closesocket(sock);
	sock = INVALID_SOCKET;
}

bool SocketUtil::Bind(SOCKET sock, const SocketAddress& address)
{
	return SOCKET_ERROR != ::bind(sock, reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr));
}

bool SocketUtil::BindAny(SOCKET sock, uint16 portNumber)
{
	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_port = htons(portNumber);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	return Bind(sock, address);
}

bool SocketUtil::Listen(SOCKET sock, int32 listenCount)
{
	return SOCKET_ERROR != listen(sock, listenCount);
}

bool SocketUtil::SetLinger(SOCKET sock, uint16 onoff, uint16 linger)
{
	LINGER lingerOption;
	lingerOption.l_onoff = onoff;
	lingerOption.l_linger = linger;

	return SetSocketOption(sock, SOL_SOCKET, SO_LINGER, lingerOption);
}

bool SocketUtil::SetReuseAddress(SOCKET sock, bool flag)
{
	return SetSocketOption(sock, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtil::SetReceiveBufferSize(SOCKET sock, int32 size)
{
	return SetSocketOption(sock, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtil::SetSendBufferSize(SOCKET sock, int32 size)
{
	return SetSocketOption(sock, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtil::SetNoDelay(SOCKET sock, bool flag)
{
	return SetSocketOption(sock, SOL_SOCKET, TCP_NODELAY, flag);
}

bool SocketUtil::SetUpdateAcceptSocket(SOCKET sock, SOCKET listenSock)
{
	return SetSocketOption(sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSock);
}

bool SocketUtil::BindWindowFunction(SOCKET sock, GUID guid, void** function)
{
	unsigned long bytes = 0;
	return SOCKET_ERROR != WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), function, sizeof(*function), &bytes, nullptr, nullptr);
}
