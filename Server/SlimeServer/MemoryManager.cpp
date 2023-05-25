#include "pch.h"
#include "MemoryManager.h"

MemoryManager::MemoryManager()
{
	for (int i = 1; i < MAX_POOL_SIZE; ++i)
		pools[i] = new MemoryPool(i);
}

MemoryManager::~MemoryManager()
{
	for (int i = 1; i < MAX_POOL_SIZE; ++i)
		delete pools[i];
}
