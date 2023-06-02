#pragma once

class SocketAddress
{
public:
	SocketAddress();
	SocketAddress(const SOCKADDR_IN& address);
	SocketAddress(const wchar* hostName, uint16 portNumber);
	~SocketAddress();

public:
	static IN_ADDR HostNameToAddress(const wchar* hostName);
	wstring GetHostName();
	uint16 GetPortNumber();
	SOCKADDR_IN& GetAddress();

private:
	SOCKADDR_IN address{};
};
