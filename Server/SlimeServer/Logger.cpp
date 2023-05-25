#include "pch.h"
#include "Logger.h"
#include <fstream>

Logger::Logger()
{
	_wsetlocale(LC_ALL, L"korean");
	format = L"[%02d-%02d %02d:%02d:%02d][%hs] %ls\n";
}

Logger::~Logger()
{
	Flush();
}

void Logger::Initialize(string folderName, Display display, bool writeBinary)
{	
	this->writeBinary = writeBinary;
	this->display = display;
	initialized = true;

	if (false == writeBinary)
		return;

	binaryPath = GetLocalPath();
	if (false == folderName.empty())
		binaryPath += ("\\" + folderName);

	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesA(binaryPath.c_str()))
		CreateDirectoryA(binaryPath.c_str(), NULL);

	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	char name[0xFF]{};
	sprintf_s(name, "%s\\[%04d-%02d-%02d]%02d%02d%02d.log", binaryPath.c_str(), systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
	binaryName = name;
}

void Logger::WriteLog(bool onlyDebug, const char* functionName, const wchar* reason, ...)
{
#ifndef _DEBUG
	if (true == onlyDebug)
		return;
#endif

	if (false == initialized)
		return;

	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	wchar va[0x200]{};
	va_list args;
	va_start(args, reason);
	vswprintf_s(va, 0x200, reason, args);
	va_end(args);

	wchar log[0x300]{};
	swprintf_s(log, format.c_str(), systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond, functionName, va);

	WRITE_LOCK(latch);
	logs.append(log);
}

void Logger::Flush()
{
	if (true == logs.empty())
		return;

	wstring flushLog;
	{
		WRITE_LOCK(latch);
		flushLog.append(logs.c_str());
		logs.clear();
	}

	if (true == writeBinary)
		WriteBinary(flushLog.c_str(), static_cast<uint32>(flushLog.length()));
	display(flushLog.c_str());
}

string Logger::GetLocalPath()
{
	char fileName[0xFF]{};
	GetModuleFileNameA(nullptr, fileName, 0xFF);

	string path = fileName;
	return path.substr(0, path.find_last_of("\\/"));
}

void Logger::WriteBinary(const wchar* reason, uint32 length)
{
	ofstream stream;
	stream.open(binaryName.c_str(), ios::app);

	if (false == stream.is_open())
		return;

	char log[0x300]{};
	WideCharToMultiByte(CP_ACP, 0, reason, -1, log, 0x300, 0, 0);

	stream.write(log, length);
	stream.close();
}
