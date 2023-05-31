#pragma once

class ReceiveBuffer
{
public:
	ReceiveBuffer(int32 size, int32 count);
	~ReceiveBuffer();

public:
	int32 GetSize() { return offsetWrite - offsetRead; }
	int32 GetFreeSize() { return capacity - offsetWrite; }

	uint8* GetWrite() { return &buffer[offsetWrite]; }
	uint8* GetRead() { return &buffer[offsetRead]; }

	void Clear();
	bool Write(int32 size);
	bool Read(int32 size);
	
private:
	vector<uint8> buffer;
	int32 capacity{ 0 };
	int32 size{ 0 };
	int32 offsetRead{ 0 };
	int32 offsetWrite{ 0 };
};
