#pragma once

class JobManager
{
	DECLARE_SINGLE(JobManager)
private:
	JobManager() = default;
	~JobManager() = default;

public:
	void UpdateTick(uint64 tick)
	{
		LMaxFlushJobTick = GetTickCount64() + tick; 
	}
	
	void Flush()
	{
		GET_SINGLE(GlobalJobQueue)->Flush();
		GET_SINGLE(JobTimerManager)->Flush();
	}
};
