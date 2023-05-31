#include "pch.h"
#include "SendBuffer.h"

thread_local SendBufferChunkRef LSendBufferChunk;

SendBuffer::SendBuffer(SendBufferChunkRef chunk, uint8* buffer, int32 size) : chunk(chunk), buffer(buffer), size(size)
{

}

SendBuffer::~SendBuffer()
{

}

void SendBuffer::Write(void* data, size_t size)
{
	memcpy(&buffer[offset], data, size);
}

void SendBuffer::Close(int32 writeSize)
{
	if (writeSize > size)
		CRASH("Over Write");

	offset = writeSize;
	chunk->Close(writeSize);
}

SendBufferChunk::SendBufferChunk()
{

}

SendBufferChunk::~SendBufferChunk()
{

}

void SendBufferChunk::Clear()
{
	opend = false;
	offset = 0;
}

SendBufferRef SendBufferChunk::Open(int32 size)
{
	if (true == opend)
		return nullptr;

	if (0 == size)
		return nullptr;

	if (size > GetFreeSize())
		return nullptr;

	opend = true;
	return SlimeAllocator::MakeShared<SendBuffer>(shared_from_this(), GetBuffer(), size);
}

void SendBufferChunk::Close(int32 size)
{
	if (false == opend)
		return;

	opend = false;
	offset += size;
}

SendBufferManager::SendBufferManager()
{

}

SendBufferManager::~SendBufferManager()
{

}

SendBufferRef SendBufferManager::Acquire(int32 size)
{
	if (nullptr == LSendBufferChunk)
		LSendBufferChunk = Pop();

	if (true == LSendBufferChunk->Opend())
		CRASH("Not Closed");

	if (LSendBufferChunk->GetFreeSize() < size)
		LSendBufferChunk = Pop();

	if (LSendBufferChunk->GetFreeSize() < size)
		CRASH("Overflow");

	return LSendBufferChunk->Open(size);
}

void SendBufferManager::Push(SendBufferChunk* chunk)
{
	GET_SINGLE(SendBufferManager)->chunks.Push({ chunk, Push });
}

SendBufferChunkRef SendBufferManager::Pop()
{
	if (true == chunks.Empty())
		return { SlimeAllocator::Malloc<SendBufferChunk>(), Push };

	auto chunk = chunks.Pop();
	chunk->Clear();

	return chunk;
}
