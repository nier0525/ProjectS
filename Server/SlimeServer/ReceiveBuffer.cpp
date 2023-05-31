#include "pch.h"
#include "ReceiveBuffer.h"

ReceiveBuffer::ReceiveBuffer(int32 size, int32 count) : capacity(size * count), size(size)
{
	buffer.resize(capacity);
}

ReceiveBuffer::~ReceiveBuffer()
{
	buffer.clear();
}

void ReceiveBuffer::Clear()
{
	auto usedSize = GetSize();
	if (0 == usedSize)
	{
		offsetWrite = 0;
		offsetRead = 0;
	}
	else if (GetFreeSize() < size)
	{
		memcpy(&buffer[0], &buffer[offsetRead], usedSize);
		offsetWrite = usedSize;
		offsetRead = 0;
	}
}

bool ReceiveBuffer::Write(int32 size)
{
	if (size > GetFreeSize())
		return false;
	offsetWrite += size;
	return true;
}

bool ReceiveBuffer::Read(int32 size)
{
	if (size > GetSize())
		return false;
	offsetRead += size;
	return true;
}
