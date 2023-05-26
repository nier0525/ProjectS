#pragma once

using Action = function<void()>;

DECLARE_SHARED(Job);
class Job
{
public:
	Job(Action&& job);
	template <typename CallBack, typename... Param>
	Job(CallBack callback, Param&&... param);
	template <class Owner, typename CallBack, typename... Param>
	Job(shared_ptr<Owner> owner, CallBack(Owner::* callback)(Param...), Param&&... param);
	template <class Owner, typename CallBack, typename... Param>
	Job(Owner* owner, CallBack(Owner::* callback)(Param...), Param&&... param);
	~Job();

public:
	void Invoke();

private:
	Action job;

	USE_ALLOCATE(Job)
};

template<typename CallBack, typename ...Param>
inline Job::Job(CallBack callback, Param && ...param)
{
	job = [callback, param...]() { callback(param...); };
}

template<class Owner, typename CallBack, typename ...Param>
inline Job::Job(shared_ptr<Owner> owner, CallBack(Owner::* callback)(Param...), Param && ...param)
{
	job = [owner, callback, param...]() { (owner.get()->*callback)(param...); };
}

template<class Owner, typename CallBack, typename ...Param>
inline Job::Job(Owner* owner, CallBack(Owner::* callback)(Param...), Param && ...param)
{
	job = [owner, callback, param...]() { (owner->*callback)(param...); };
}
