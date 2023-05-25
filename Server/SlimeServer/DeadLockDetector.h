#pragma once

extern thread_local unordered_set<uint64> LLockedID;

class DeadLockDetector
{
	DECLARE_SINGLE(DeadLockDetector)
private:
	DeadLockDetector() = default;
	~DeadLockDetector() = default;

public:
	void Register(ILock* lock);
	void Release(ILock* lock);

	bool VaildLockStack(ILock* lock);
};
