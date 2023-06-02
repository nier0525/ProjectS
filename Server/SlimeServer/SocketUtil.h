#pragma once
#include "SocketAddress.h"

class SocketUtil
{
private:
	SocketUtil() = default;
	~SocketUtil() = default;

public:
	static void Initialize();
	static void Clear();

	static SOCKET CreateSocket();
	static void CloseSocket(SOCKET& sock);

	static bool Bind(SOCKET sock, const SocketAddress& address);
	static bool BindAny(SOCKET sock, uint16 portNumber = 0);
	static bool Listen(SOCKET sock, int32 listenCount = SOMAXCONN);

	static bool SetLinger(SOCKET sock, uint16 onoff, uint16 linger);
	static bool SetReuseAddress(SOCKET sock, bool flag);
	static bool SetReceiveBufferSize(SOCKET sock, int32 size);
	static bool SetSendBufferSize(SOCKET sock, int32 size);
	static bool SetNoDelay(SOCKET sock, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET sock, SOCKET listenSock);

private:
	template <typename T>
	static bool SetSocketOption(SOCKET sock, int32 level, int32 name, T value);
	static bool BindWindowFunction(SOCKET sock, GUID guid, void** function);

public:
	static LPFN_ACCEPTEX AcceptEx;
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
};

template<typename T>
inline bool SocketUtil::SetSocketOption(SOCKET sock, int32 level, int32 name, T value)
{
	return SOCKET_ERROR != setsockopt(sock, level, name, reinterpret_cast<char*>(&value), sizeof(T));
}
