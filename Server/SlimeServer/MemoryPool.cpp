#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(uint32 size, uint32 count) : capacity(max(size, sizeof(int8*))), count(count)
{

}

MemoryPool::~MemoryPool()
{
	while (false == chunks.empty())
	{
		auto chunk = chunks.front();
		chunks.pop();

		free(chunk);
	}
}

void* MemoryPool::Acquire()
{
	WRITE_LOCK(latch);

	if (nullptr == header)
		NewChunk();

	auto ptr = header;
	header = *reinterpret_cast<int8**>(ptr);
	return ptr;
}

void MemoryPool::Release(void* ptr)
{
	WRITE_LOCK(latch);

	*reinterpret_cast<int8**>(ptr) = header;
	header = reinterpret_cast<int8*>(ptr);
}

void MemoryPool::NewChunk()
{
	auto chunk = reinterpret_cast<int8*>(malloc(capacity * count));
	chunks.push(chunk);

	for (uint32 i = 0; i < count; ++i)
	{
		auto ptr = chunk + i * capacity;
		*reinterpret_cast<int8**>(ptr) = header;
		header = ptr;
	}
}
