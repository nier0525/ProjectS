#pragma once

#pragma region [Interface]
class ILock
{
	friend class Lock;
public:
	uint64 GetHash() const;

protected:
	virtual bool Lock() abstract;
	virtual void Unlock() abstract;
};

class IReadWriteLock : public ILock
{
	friend class WriteLock;
	friend class ReadLock;
private:
	virtual bool Lock() final { return false; }
	virtual void Unlock() final {}

protected:
	virtual bool WriteLock() abstract;
	virtual void WriteUnlock() abstract;
	virtual bool ReadLock() abstract;
	virtual void ReadUnlock() abstract;
};
#pragma endregion

#pragma region [RAII]
class Lock
{
public:
	Lock(ILock& lock);
	Lock(ILock* lock);
	~Lock();

private:
	ILock* lock{ nullptr };
};
#define LOCK(latch) Lock lock(latch)

class WriteLock
{
public:
	WriteLock(IReadWriteLock& lock);
	WriteLock(IReadWriteLock* lock);
	~WriteLock();

private:
	IReadWriteLock* lock{ nullptr };
};
#define WRITE_LOCK(latch) WriteLock writeLock(latch)

class ReadLock
{
public:
	ReadLock(IReadWriteLock& lock);
	ReadLock(IReadWriteLock* lock);
	~ReadLock();

private:
	IReadWriteLock* lock{ nullptr };
};
#define READ_LOCK(latch) ReadLock readLock(latch)
#pragma endregion

class SpinLock : public IReadWriteLock
{
	enum : uint32
	{
		EMPTY = 0x0000'0000,
		MAX_SPIN_COUNT = 5000
	};

public:
	SpinLock() = default;
	virtual ~SpinLock() = default;

public:
	bool WriteLock() override;
	void WriteUnlock() override;
	bool ReadLock() override;
	void ReadUnlock() override;

private:
	atomic_uint writeID{ EMPTY };
	atomic_uint readCount{ EMPTY };
};

class TicketLock : public ILock
{
public:
	TicketLock() = default;
	virtual ~TicketLock() = default;

public:
	bool Lock() override;
	void Unlock() override;

private:
	atomic<uint64> nextTicket{ 0 };
	atomic<uint64> nowTicket{ 0 };
};
