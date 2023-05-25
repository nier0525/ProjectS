#pragma once
#include "LockQueue.h"

class Logger
{
	DECLARE_SINGLE(Logger)

	using Logs = LockQueue<wstring>;
	using Display = function<void(const wchar*)>;
private:
	Logger();
	~Logger();

public:
	void Initialize(string folderName, Display display, bool writeBinary = true);
	void WriteLog(bool onlyDebug, const char* functionName, const wchar* reason, ...);
	void Flush();

private:
	string GetLocalPath();
	void WriteBinary(const wchar* reason, uint32 length);

private:
	SpinLock latch;
	Display display;
	wstring logs;
	string binaryPath;
	string binaryName;
	wstring format;

	bool writeBinary{ false };
	bool initialized{ false };
};

#define LOG(...) GET_SINGLE(Logger)->WriteLog(false, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...) GET_SINGLE(Logger)->WriteLog(true, __FUNCTION__, __VA_ARGS__)