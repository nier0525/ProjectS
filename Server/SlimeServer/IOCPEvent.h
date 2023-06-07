#pragma once

DECLARE_SHARED(IOCPContext);

class IOCPContext : public enable_shared_from_this<IOCPContext>
{
public:
	virtual void OnDispatch(class IOCPEvent* iocpEvent, int32 bytes) abstract;
	virtual HANDLE GetHandle() abstract;
};

enum class IOCPEventType : uint8
{
	ACCEPT,
	CONNECT,
	DISCONNECT,
	SEND,
	RECEIVE,
};

class IOCPEvent : public OVERLAPPED
{
public:
	IOCPEvent(IOCPEventType type);
	~IOCPEvent();

public:
	void Initialize();

public:
	IOCPEventType type;
	IOCPContextRef owner{ nullptr };
};

class AcceptEvent : public IOCPEvent
{
public:
	AcceptEvent() : IOCPEvent(IOCPEventType::ACCEPT) {}

public:
	shared_ptr<class IOCPSession> session{ nullptr };
};

class ConnectEvent : public IOCPEvent
{
public:
	ConnectEvent() : IOCPEvent(IOCPEventType::CONNECT) {}
};

class DisconnectEvent : public IOCPEvent
{
public:
	DisconnectEvent() : IOCPEvent(IOCPEventType::DISCONNECT) {}
};

class SendEvent : public IOCPEvent
{
public:
	SendEvent() : IOCPEvent(IOCPEventType::SEND) {}
};

class ReceiveEvent : public IOCPEvent
{
public:
	ReceiveEvent() : IOCPEvent(IOCPEventType::RECEIVE) {}
};

