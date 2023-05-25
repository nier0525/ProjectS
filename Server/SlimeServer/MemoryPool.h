#pragma once

class MemoryPool
{
public:
	enum { DEFAULT_COUNT = 100 };

public:
	MemoryPool(uint32 size, uint32 count = DEFAULT_COUNT);
	~MemoryPool();

public:
	void* Acquire();
	void Release(void* ptr);

private:
	void NewChunk();

private:
	SpinLock latch;
	queue<int8*> chunks;

	int8* header{ nullptr };
	uint32 capacity{ 0 };
	uint32 count{ 0 };
};

template <class T>
class ObjectPool
{
public:
	static void* Acquire() { return pool.Acquire(); }
	static void Release(void* ptr) { pool.Release(ptr); }

private:
	static MemoryPool pool;
};

template <class T>
MemoryPool ObjectPool<T>::pool{ sizeof(T), MemoryPool::DEFAULT_COUNT };