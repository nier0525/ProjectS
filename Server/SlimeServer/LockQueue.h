#pragma once

template <class T>
class LockQueue
{
public:
	LockQueue() = default;
	~LockQueue() = default;

public:
	void Clear();
	void Push(T data);
	T Pop();
	void Pop(list<T>& datas, int count);
	void PopAll(list<T>& datas);
	T Peek();
	bool Empty();
	uint32 Count();

private:
	SpinLock latch;
	queue<T> contain;
};

template<class T>
inline void LockQueue<T>::Clear()
{
	WRITE_LOCK(latch);
	while (false == contain.empty())
		contain.pop();
}

template<class T>
inline void LockQueue<T>::Push(T data)
{
	WRITE_LOCK(latch);
	contain.push(data);
}

template<class T>
inline T LockQueue<T>::Pop()
{	
	WRITE_LOCK(latch);
	if (true == contain.empty())
		return T();
	
	auto data = contain.front();
	contain.pop();

	return data;
}

template<class T>
inline void LockQueue<T>::Pop(list<T>& datas, int count)
{
	WRITE_LOCK(latch);
	for (int i = 0; i < count; ++i)
	{ 
		if (true == contain.empty())
			return;

		datas.push_back(contain.front());
		contain.pop();
	}
}

template<class T>
inline void LockQueue<T>::PopAll(list<T>& datas)
{
	WRITE_LOCK(latch);
	while (false == contain.empty())
	{
		datas.push_back(contain.front());
		contain.pop();
	}
}

template<class T>
inline T LockQueue<T>::Peek()
{
	READ_LOCK(latch);
	if (true == contain.empty())
		return T();
	return contain.front();
}

template<class T>
inline bool LockQueue<T>::Empty()
{
	READ_LOCK(latch);
	return contain.empty();
}

template<class T>
inline uint32 LockQueue<T>::Count()
{
	return static_cast<uint32>(contain.size());
}
