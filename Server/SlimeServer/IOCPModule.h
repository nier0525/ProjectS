#pragma once
#include "SocketUtil.h"
#include "IOCPEvent.h"
#include "IOCPListener.h"

DECLARE_SHARED(IOCPModule);

class IOCPModule : public enable_shared_from_this<IOCPModule>
{
public:
	enum IOCPType : uint8 { SERVER , CLIENT };

	using Sessions = set<IOCPSessionRef>;
	using SessionFactory = function<IOCPSessionRef(void)>;	
protected:
	IOCPModule(IOCPType type, SocketAddress address, SessionFactory sessionFactory);
	~IOCPModule();

public:
	virtual IOCPSessionRef NewSession();
	virtual void Close();

	HANDLE GetHandle() { return handler; }
	IOCPType GetType() { return type; }
	SocketAddress* GetAddress() { return &address; }
	bool IsClosed() { return isClosed.load(); }

	bool RegisterIOCP(IOCPContextRef context);
	bool OnDispatch(uint32 timeoutMS = INFINITE);

protected:
	IOCPType type;
	HANDLE handler{ INVALID_HANDLE_VALUE };
	SocketAddress address;
	SessionFactory sessionFactory;
	atomic_bool isClosed{ false };
};

class IOCPServer : public IOCPModule
{
public:
	IOCPServer(SocketAddress address, SessionFactory sessionFactory);
	virtual ~IOCPServer();

public:
	void Open();
	void Listen(int32 listenCount = 1);

private:

	USE_ALLOCATE(IOCPServer)
};

class IOCPClient : public IOCPModule
{
public:
	IOCPClient(SocketAddress address, SessionFactory sessionFactory);
	virtual ~IOCPClient();

public:
	bool Connect(uint32 connectCount = 1);

private:
	USE_ALLOCATE(IOCPClient)
};