#include "pch.h"
#include "SocketAddress.h"

SocketAddress::SocketAddress()
{
	memset(&address, 0, sizeof(SOCKADDR_IN));
}

SocketAddress::SocketAddress(const SOCKADDR_IN& address) : address(address)
{

}

SocketAddress::SocketAddress(const wchar* hostName, uint16 portNumber)
{
	memset(&address, 0, sizeof(SOCKADDR_IN));

	address.sin_family = AF_INET;
	address.sin_port = htons(portNumber);
	address.sin_addr = HostNameToAddress(hostName);
}

SocketAddress::~SocketAddress()
{

}

IN_ADDR SocketAddress::HostNameToAddress(const wchar* hostName)
{
	IN_ADDR address;
	InetPtonW(AF_INET, hostName, &address);

	return address;
}

wstring SocketAddress::GetHostName()
{
	wchar hostName[0x40]{};
	InetNtop(AF_INET, &address.sin_addr, hostName, static_cast<uint32>(sizeof(hostName) / sizeof(hostName[0])));

	return { hostName };
}

uint16 SocketAddress::GetPortNumber()
{
	return ntohs(address.sin_port);
}

SOCKADDR_IN& SocketAddress::GetAddress()
{
	return address;
}
