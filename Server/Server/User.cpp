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
	GET_SINGLE(UserManager)->InsertUser(GET_SHARED(User));
}

void User::OnDisconnected()
{
	LOG(L"Disconnected User");
	GET_SINGLE(UserManager)->RemoveUser(GET_SHARED(User));
}

void User::OnReceive(PacketRef packet)
{
	auto protocol = packet->GetProtocol();
	switch (protocol)
	{

	default:
		return LOG(L"Unknown Protocol [%u]", protocol);
	}
}

UserManager::UserManager()
{

}

UserManager::~UserManager()
{
	users.clear();
}
