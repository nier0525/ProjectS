#pragma once
#include "MemoryManager.h"

class SlimeAllocator
{
public:
	template <class T, typename... Param>
	static T* Malloc(Param&&... param);

	template <class T>
	static void Free(T* ptr);

	template <class T, typename... Param>
	static shared_ptr<T> MakeShared(Param&&... param);
};

template<class T, typename ...Param>
inline T* SlimeAllocator::Malloc(Param && ...param)
{
	auto data = reinterpret_cast<T*>(GET_SINGLE(MemoryManager)->Acquire<T>());
	new(data)T(forward<Param>(param)...);

	return data;
}

template<class T>
inline void SlimeAllocator::Free(T* ptr)
{
	if (nullptr == ptr)
		return;

	ptr->~T();
	GET_SINGLE(MemoryManager)->Release<T>(ptr);
}

template<class T, typename ...Param>
inline shared_ptr<T> SlimeAllocator::MakeShared(Param && ...param)
{	
	return { SlimeAllocator::Malloc<T>(forward<Param>(param)...), SlimeAllocator::Free<T> };
}

#define USE_ALLOCATE(Class)												\
public:																	\
template <typename... Param>											\
static Class* Malloc(Param&&... param)									\
{																		\
	return SlimeAllocator::Malloc<Class>(forward<Param>(param)...);		\
}																		\
																		\
static void Free(Class* ptr)											\
{																		\
	SlimeAllocator::Free<Class>(ptr);									\
}																		\
																		\
template <typename... Param>											\
static shared_ptr<Class> MakeShared(Param&&... param)					\
{																		\
	return SlimeAllocator::MakeShared<Class>(forward<Param>(param)...);	\
}