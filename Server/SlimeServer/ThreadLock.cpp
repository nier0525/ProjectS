#include "pch.h"
#include "ThreadLock.h"
#include "ThreadManager.h"
#include "DeadLockDetector.h"

uint64 ILock::GetHash() const
{
	return hash<const void*>()(static_cast<const void*>(this));
}

Lock::Lock(ILock& lock)
{
	if (true == lock.Lock())
		this->lock = &lock;
}

Lock::Lock(ILock* lock)
{
	if (nullptr != lock && true == lock->Lock())
		this->lock = lock;
}

Lock::~Lock()
{
	if (nullptr != lock)
		lock->Unlock();
}

WriteLock::WriteLock(IReadWriteLock& lock)
{
	if (true == lock.WriteLock())
		this->lock = &lock;
}

WriteLock::WriteLock(IReadWriteLock* lock)
{
	if (nullptr != lock && true == lock->WriteLock())
		this->lock = lock;
}

WriteLock::~WriteLock()
{
	if (nullptr != lock)
		lock->WriteUnlock();
}

ReadLock::ReadLock(IReadWriteLock& lock)
{
	if (true == lock.ReadLock())
		this->lock = &lock;
}

ReadLock::ReadLock(IReadWriteLock* lock)
{
	if (nullptr != lock && true == lock->ReadLock())
		this->lock = lock;
}

ReadLock::~ReadLock()
{
	if (nullptr != lock)
		lock->ReadUnlock();
}

bool SpinLock::WriteLock()
{
	if (0 == LThreadID)
		ThreadManager::Initialize();

	auto lockedID = writeID.load();
	if (LThreadID == lockedID)
		return false;

	if (false == GET_SINGLE(DeadLockDetector)->VaildLockStack(this))
		CRASH("Dead Lock");

	do
	{
		for (int i = 0; i < MAX_SPIN_COUNT; ++i)
		{
			uint32 expected = EMPTY;
			if (true == writeID.compare_exchange_strong(expected, LThreadID))
			{
				lockedID = writeID.load();
				break;
			}
			this_thread::sleep_for(1ms);
		}

		if (LThreadID != lockedID)
			break;

		for (int i = 0; i < MAX_SPIN_COUNT; ++i)
		{
			if (EMPTY == readCount.load())
				return true;
			this_thread::sleep_for(1ms);
		}
	} while (false);

	if (LThreadID != lockedID)
		LOG(L"Write Lock Timeout %u", LThreadID);
	else
		LOG(L"Read Lock Timeout %u", readCount.load());

	CRASH("Lock Timeout");
	return true;
}

void SpinLock::WriteUnlock()
{
	writeID.store(EMPTY);
}

bool SpinLock::ReadLock()
{
	if (0 == LThreadID)
		ThreadManager::Initialize();

	if (LThreadID == writeID.load())
		return false;

	if (false == GET_SINGLE(DeadLockDetector)->VaildLockStack(this))
		return false;

	for (int i = 0; i < MAX_SPIN_COUNT; ++i)
	{
		if (EMPTY == writeID.load())
		{
			GET_SINGLE(DeadLockDetector)->Register(this);
			readCount.fetch_add(1);
			return true;
		}
		this_thread::sleep_for(1ms);
	}

	LOG(L"Write Lock Timeout %u", writeID.load());
	CRASH("Lock Timeout");
	return false;
}

void SpinLock::ReadUnlock()
{
	GET_SINGLE(DeadLockDetector)->Release(this);
	readCount.fetch_sub(1);
}

bool TicketLock::Lock()
{
	if (false == GET_SINGLE(DeadLockDetector)->VaildLockStack(this))
		CRASH("Dead Lock");

	auto ticket = nextTicket.fetch_add(1);
	while (ticket != nowTicket.load())
		this_thread::sleep_for(1ms);

	GET_SINGLE(DeadLockDetector)->Register(this);
	return true;
}

void TicketLock::Unlock()
{
	GET_SINGLE(DeadLockDetector)->Release(this);
	nowTicket.fetch_add(1);
}
