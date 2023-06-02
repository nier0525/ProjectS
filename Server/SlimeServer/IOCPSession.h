#pragma once
#include "SocketUtil.h"
#include "IOCPEvent.h"
#include "ReceiveBuffer.h"
#include "SendBuffer.h"

DECLARE_SHARED(IOCPSession);

class IOCPSession : public IOCPContext
{
	friend class IOCPModule;
	friend class IOCPListener;

	using SendQueue = LockQueue<SendBufferRef>;
protected:
	IOCPSession();
	virtual ~IOCPSession();

private:
	virtual HANDLE GetHandle() override;
	virtual void OnDispatch(IOCPEvent* iocpEvent, int32 bytes) override;

public:
	bool DoConnect(SocketAddress address);
	void DoDisconnect();
	void DoDisconnect(const wchar* reason, ...);
	void DoSend(SendBufferRef sendBuffer);

protected:
	virtual void OnAccepted() {}
	virtual void OnConnected() {}
	virtual void OnDisconnected() {}
	virtual void OnSend(int32 bytes) {}
	virtual int32 OnReceive(uint8* buffer, int32 bytes) { return bytes; }

private:
	bool IsPending(int32 errorCode);
	void HandleError(int32 errorCode);

	bool RegisterConnect();
	bool RegisterDisconnect();
	bool RegisterSend();
	bool RegisterReceive();

	void ProcessAccept();
	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessSend(int32 bytes);
	void ProcessReceive(int32 bytes);

public:
	SOCKET GetSocket() { return sock; }
	void SetAddress(const SocketAddress& address) { this->address = address; }
	SocketAddress* GetAddress() { return &address; }

	bool IsConnected() { return isConnected.load(); }

protected:
	SOCKET sock{ INVALID_SOCKET };
	SocketAddress address;
	atomic_bool isConnected{ false };

	atomic_bool registeredSend{ false };
	SendQueue sendQueue;

	ReceiveBuffer receiveBuffer;

	ConnectEvent connectEvent;
	DisconnectEvent disconnectEvent;
	SendEvent sendEvent;
	ReceiveEvent receiveEvent;
};

class IOCPSessionManager
{
	DECLARE_SINGLE(IOCPSessionManager)
private:
	IOCPSessionManager();
	~IOCPSessionManager();

public:	
	void Clear();
	bool Insert(IOCPSessionRef session);
	void Remove(IOCPSessionRef session);

private:
	SpinLock latch;
	set<IOCPSessionRef> sessions;
};