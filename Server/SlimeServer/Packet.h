#pragma once

DECLARE_SHARED(Packet);

class Packet
{
public:
	enum : uint16
	{
		PACKET_OVERFLOW = 1,
		PACKET_UNDERFLOW = 2,
		HEADER_SIZE = 4,
		DATA_SIZE = MAX_PACKET_SIZE - HEADER_SIZE,
	};

public:
	Packet();
	Packet(const Packet& packet);
	Packet(uint16 protocol);
	template <typename... Param>
	Packet(uint16 protocol, Param&&... param);

	~Packet();

	Packet& operator=(const Packet& packet);
	template <typename Param>
	Packet& operator<<(const Param& param);
	template <typename Param>
	Packet& operator>>(Param& param);

public:
	void SetProtocol(uint16 protocol) { this->protocol = protocol; }
	uint16 GetProtocol() { return protocol; }
	uint8* GetAll() { return buffer; }
	uint16 GetAllSize() { return size; }
	uint8* GetData() { return data; }
	uint16 GetDataSize() { return size - HEADER_SIZE; }

	void Clear();

	void ResetWrite();
	void ResetRead();

	void Copy(const Packet& packet);
	void Serialize(uint8* buffer);
	void Parse(uint8* buffer, uint16 size);

	template <typename T>
	void Push(T value) { PushData(&value, static_cast<uint16>(sizeof(T))); }
	void Push(char* value) { PushString(value, static_cast<uint16>(::strlen(value))); }
	void Push(wchar* value) { PushString(value, static_cast<uint16>(::wcslen(value))); }
	void Push(const char* value) { Push(const_cast<char*>(value)); }
	void Push(const wchar* value) { Push(const_cast<wchar*>(value)); }

	template <typename T, typename... Param>
	void Push(T value, Param... param);

	template <typename T>
	void Pop(T& value) { PopData(&value, static_cast<uint16>(sizeof(T))); }
	void Pop(char* value) { PopString(value); }
	void Pop(wchar* value) { PopString(value); }

	template <typename T, typename... Param>
	void Pop(T& value, Param&... param);

	template <typename T>
	T* Skip();
	void Subtract(uint16 offset, uint16 size);

private:
	bool IsOverflow(uint16 size);
	bool IsUnderflow(uint16 size);

	void PushData(void* value, uint16 size);
	void PushString(void* value, uint16 size);

	void PopData(void* value, uint16 size);
	void PopString(void* value);

private:
	union
	{
		struct
		{
			uint16 size;
			uint16 protocol;
			uint8 data[DATA_SIZE];
		};
		uint8 buffer[MAX_PACKET_SIZE];
	};

	uint8* offsetWrite{ nullptr };
	uint8* offsetRead{ nullptr };

	USE_ALLOCATE(Packet)
};

template<typename ...Param>
inline Packet::Packet(uint16 protocol, Param && ...param)
{
	Clear();
	this->protocol = protocol;
	Push(forward<Param>(param)...);
}

template<typename Param>
inline Packet& Packet::operator<<(const Param& param)
{
	Push(param);
	return *this;
}

template<typename Param>
inline Packet& Packet::operator>>(Param& param)
{
	Pop(param);
	return *this;
}

template<typename T, typename ...Param>
inline void Packet::Push(T value, Param ...param)
{
	Push(value);
	Push(param...);
}

template<typename T, typename ...Param>
inline void Packet::Pop(T& value, Param & ...param)
{
	Pop(value);
	Pop(param...);
}

template<typename T>
inline T* Packet::Skip()
{
	auto size = static_cast<uint16>(sizeof(T));
	if (true == IsOverflow(size))
		return nullptr;

	auto data = offsetWrite;
	offsetWrite += size;
	this->size += size;

	return reinterpret_cast<T*>(data);
}