#include "pch.h"
#include "IOCPEvent.h"

IOCPEvent::IOCPEvent(IOCPEventType type) : type(type)
{
	Initialize();
}

IOCPEvent::~IOCPEvent()
{

}

void IOCPEvent::Initialize()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
