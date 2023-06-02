#pragma once
#include "IOCPEvent.h"

class IOCPListener : public IOCPContext
{
public:
	IOCPListener();
	virtual ~IOCPListener();

public:
	virtual HANDLE GetHandle() override;
	virtual void OnDispatch(IOCPEvent* iocpEvent, int32 bytes);

public:
	bool Open()
};
