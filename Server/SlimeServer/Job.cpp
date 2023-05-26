#include "pch.h"
#include "Job.h"

Job::Job(Action&& job) : job(move(job))
{

}

Job::~Job()
{

}

void Job::Invoke()
{
	job();
}
