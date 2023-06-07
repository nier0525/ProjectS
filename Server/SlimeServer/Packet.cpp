#include "pch.h"
#include "Packet.h"

Packet::Packet()
{
	Clear();
}

Packet::Packet(const Packet& packet)
{
	Copy(packet);
}

Packet::Packet(uint16 protocol)
{
	Clear();
	this->protocol = protocol;
}

Packet::~Packet()
{
	Clear();
}

Packet& Packet::operator=(const Packet& packet)
{
	memcpy(buffer, packet.buffer, packet.size);
	ResetWrite();
	ResetRead();

	return *this;
}

void Packet::Clear()
{
	memset(buffer, 0, sizeof(buffer));

	size = HEADER_SIZE;
	protocol = 0;
	offsetWrite = data;
	offsetRead = data;
}

void Packet::ResetWrite()
{
	offsetWrite = buffer + size;
}

void Packet::ResetRead()
{
	offsetRead = data;
}

void Packet::Copy(const Packet& packet)
{
	*this = packet;
}

void Packet::Serialize(uint8* buffer)
{
	memcpy(buffer, this->buffer, this->size);
}

void Packet::Parse(uint8* buffer, uint16 size)
{
	memcpy(this->buffer, buffer, size);

	ResetWrite();
	ResetRead();
}

void Packet::Subtract(uint16 offset, uint16 size)
{
	auto length = GetDataSize() - (offset + size);

	memcpy(data + offset, data + (offset + size), length);
	this->size -= size;

	memset(data + this->size, 0, size);
	ResetWrite();
}

bool Packet::IsOverflow(uint16 size)
{
	if (this->size + size < MAX_PACKET_SIZE)
		return false;

	CRASH("Packet Overflow");
	return true;
}

bool Packet::IsUnderflow(uint16 size)
{
	if ((offsetRead + size) <= (buffer + this->size))
		return false;

	CRASH("Packet Underflow");
	return true;
}

void Packet::PushData(void* value, uint16 size)
{
	if (true == IsOverflow(size))
		return;

	memcpy(offsetWrite, value, size);
	offsetWrite += size;
	this->size += size;
}

void Packet::PushString(void* value, uint16 size)
{
	PushData(&size, 2);
	PushData(value, size);
}

void Packet::PopData(void* value, uint16 size)
{
	if (true == IsUnderflow(size))
		return;

	memcpy(value, offsetRead, size);
	offsetRead += size;
}

void Packet::PopString(void* value)
{
	uint16 length = 0;
	PopData(&length, 2);
	PopData(value, length);
}
