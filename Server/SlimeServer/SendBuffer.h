#pragma once

DECLARE_SHARED(SendBuffer);
DECLARE_SHARED(SendBufferChunk);

extern thread_local SendBufferChunkRef LSendBufferChunk;

class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef chunk, uint8* buffer, int32 size);
	~SendBuffer();

public:
	uint8* GetBuffer() { return buffer; }
	int32 GetSize() { return size; }
	int32 GetOffset() { return offset; }

	void Write(void* data, size_t size);
	void Close(int32 writeSize);

private:
	SendBufferChunkRef chunk{ nullptr };
	uint8* buffer{ nullptr };
	int32 size{ 0 };
	int32 offset{ 0 };
};

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	friend class SendBuffer;
	friend class SendBufferManager;

	using Buffer = array<uint8, MAX_PACKET_SIZE * 10>;
public:
	SendBufferChunk();
	~SendBufferChunk();

private:
	uint8* GetBuffer() { return &buffer[offset]; }
	int32 GetFreeSize() { return static_cast<int32>(buffer.size()) - offset; }
	bool Opend() { return opend; }
	
	void Clear();
	SendBufferRef Open(int32 size);
	void Close(int32 size);

private:
	Buffer buffer{};
	int32 offset{ 0 };
	bool opend{ false };
};

class SendBufferManager
{
	DECLARE_SINGLE(SendBufferManager)
	using Chunks = LockQueue<SendBufferChunkRef>;
private:
	SendBufferManager();
	~SendBufferManager();

public:
	SendBufferRef Acquire(int32 size);

private:
	static void Push(SendBufferChunk* chunk);
	SendBufferChunkRef Pop();

private:
	Chunks chunks;
};