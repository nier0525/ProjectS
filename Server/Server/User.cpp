#include "pch.h"
#include "User.h"

User::User()
{
	jobQueue = SlimeAllocator::MakeShared<JobQueue>();
}

User::~User()
{
	jobQueue = nullptr;
}

void User::OnAccepted()
{
	LOG(L"Accepted User");	
	GET_SINGLE(UserManager)->PushJob(&UserManager::InsertUser, GET_SHARED(User));
	
	SendToTest();
}

void User::OnDisconnected()
{
	LOG(L"Disconnected User");
	GET_SINGLE(UserManager)->PushJob(&UserManager::RemoveUser, GET_SHARED(User));
}

void User::OnReceivePacket()
{
	auto protocol = receivePacket->GetProtocol();
	switch (protocol)
	{
	case 0: return OnReceiveTest();
	default:
		return LOG(L"Unknown Protocol [%u]", protocol);
	}
}

void User::OnReceiveTest()
{
	wchar message[0xFF]{};
	receivePacket->Pop(message);

	//LOG(L"%s", message);
	jobQueue->ReserveJob(1000, GET_SHARED(User), &User::SendToTest);
}

void User::SendToTest()
{
	DoSend(Packet(0, L"this is packet.."));
}

UserManager::UserManager()
{

}

UserManager::~UserManager()
{
	users.clear();
}
