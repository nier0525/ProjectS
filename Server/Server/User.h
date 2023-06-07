#pragma once

DECLARE_SHARED(User);

class User : public PacketSession
{
public:
	User();
	virtual ~User();

private:
	virtual void OnAccepted() override;
	virtual void OnDisconnected() override;
	virtual void OnReceive(PacketRef packet) override;

private:
	USE_ALLOCATE(User)
};

class UserManager : public JobQueue
{
	DECLARE_SINGLE(UserManager)
private:
	UserManager();
	~UserManager();

public:
	void InsertUser(UserRef user) { users.insert(user); }
	void RemoveUser(UserRef user) { users.erase(user); }

private:	
	set<UserRef> users;
};