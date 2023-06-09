#include "pch.h"
#include "Dummy.h"

Dummy::Dummy()
{

}

Dummy::~Dummy()
{

}

void Dummy::OnConnected()
{
	GET_SINGLE(DummyManager)->PushJob(&DummyManager::InsertDummy, GET_SHARED(Dummy));
}

void Dummy::OnDisconnected()
{
	GET_SINGLE(DummyManager)->PushJob(&DummyManager::RemoveDummy, GET_SHARED(Dummy));
}

void Dummy::OnReceivePacket()
{
	auto protocol = receivePacket->GetProtocol();
	switch (protocol)
	{
	case 0: return OnReceiveTest();
	default:
		return LOG(L"Unknown Protocol [%u]", protocol);
	}
}

void Dummy::OnReceiveTest()
{
	wchar message[0xff]{};
	receivePacket->Pop(message);

	LOG(L"%s", message);
	DoSend({ 0, message });
}

DummyManager::DummyManager()
{

}

DummyManager::~DummyManager()
{
	dummys.clear();
}

