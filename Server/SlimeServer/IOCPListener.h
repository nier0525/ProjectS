#pragma once
#include "IOCPEvent.h"

DECLARE_SHARED(IOCPListener);

class IOCPListener : public IOCPContext
{
public:
	IOCPListener();
	virtual ~IOCPListener();

public:
	virtual HANDLE GetHandle() override;
	virtual void OnDispatch(IOCPEvent* iocpEvent, int32 bytes);

public:
	bool Open(IOCPServerRef server);
	void Close();

	void DoAccept(int32 count);

private:
	void CreateAcceptEvent();
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);

protected:
	list<AcceptEvent*> acceptEvents;
	IOCPServerRef server{ nullptr };
	SOCKET sock{INVALID_SOCKET};
};
