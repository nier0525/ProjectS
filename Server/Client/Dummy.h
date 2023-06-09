#pragma once

DECLARE_SHARED(Dummy);

class Dummy : public PacketSession
{
public:
	Dummy();
	virtual ~Dummy();

private:
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnReceivePacket() override;

private:
	void OnReceiveTest();

private:

	USE_ALLOCATE(Dummy)
};

class DummyManager : public JobQueue
{
	DECLARE_SINGLE(DummyManager)
private:
	DummyManager();
	~DummyManager();

public:
	void InsertDummy(DummyRef dummy) { dummys.insert(dummy); }
	void RemoveDummy(DummyRef dummy) { dummys.erase(dummy); }

private:
	set<DummyRef> dummys;
};