#pragma once
#include "MemoryPool.h"

class MemoryManager
{
	DECLARE_SINGLE(MemoryManager)
public:
	enum { MAX_POOL_SIZE = 0x4000 };

private:
	MemoryManager();
	~MemoryManager();

public:
	template <class T>
	void* Acquire();

	template <class T>
	void Release(void* ptr);

private:
	MemoryPool* pools[MAX_POOL_SIZE + 1]{ nullptr };
};

template<class T>
inline void* MemoryManager::Acquire()
{
	auto size = static_cast<uint32>(sizeof(T));
	if (0 == size)
		return nullptr;

	if (MAX_POOL_SIZE < size)
		return ObjectPool<T>::Acquire();

	return pools[size]->Acquire();
}

template<class T>
inline void MemoryManager::Release(void* ptr)
{
	auto size = static_cast<uint32>(sizeof(T));
	if (0 == size || nullptr == ptr)
		return;

	if (MAX_POOL_SIZE < size)
		return ObjectPool<T>::Release(ptr);

	pools[size]->Release(ptr);
}
