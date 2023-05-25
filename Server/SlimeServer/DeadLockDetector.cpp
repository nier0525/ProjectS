#include "pch.h"
#include "DeadLockDetector.h"

thread_local unordered_set<uint64> LLockedID;

void DeadLockDetector::Register(ILock* lock)
{
	LLockedID.insert(lock->GetHash());
}

void DeadLockDetector::Release(ILock* lock)
{
	LLockedID.erase(lock->GetHash());
}

bool DeadLockDetector::VaildLockStack(ILock* lock)
{
	if (true == LLockedID.empty())
		return true;
	return LLockedID.find(lock->GetHash()) == LLockedID.end();
}
