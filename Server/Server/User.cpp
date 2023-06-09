#include "pch.h"
#include "User.h"

User::User()
{

}

User::~User()
{

}

void User::OnAccepted()
{
	LOG(L"Accepted User");	
	GET_SINGLE(UserManager)->PushJob(&UserManager::InsertUser, GetShared<User>());
	
	SendToTest();
}

void User::OnDisconnected()
{
	LOG(L"Disconnected User");
	GET_SINGLE(UserManager)->PushJob(&UserManager::RemoveUser, GetShared<User>());
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

	LOG(L"%s", message);
	ReserveJob(1000, &User::SendToTest);
}

void User::SendToTest()
{
	DoSend({ 0, L"this is packet.." });
}

UserManager::UserManager()
{

}

UserManager::~UserManager()
{
	users.clear();
}
