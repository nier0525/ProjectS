#include "pch.h"
#include "Dumper.h"
#pragma warning (disable:4996)

#include <signal.h>
#include <DbgHelp.h>
#pragma comment (lib, "dbghelp.lib")

void Dumper::Launch()
{
	if (true == static_cast<bool>(IsDebuggerPresent()))
		return;

	SetUnhandledExceptionFilter(UnhandledException);
	_set_invalid_parameter_handler(InvalidParameter);
	_set_purecall_handler(PureCall);

	signal(SIGABRT, HandlerSignal);
}

void Dumper::GenerateMiniDump(PEXCEPTION_POINTERS exception, bool isNormal)
{
	char path[0xFF]{};
	GetModuleFileNameA(nullptr, path, sizeof(path));

	SYSTEMTIME genTime;
	GetSystemTime(&genTime);

	MINIDUMP_EXCEPTION_INFORMATION dump;
	dump.ThreadId = GetCurrentThreadId();
	dump.ExceptionPointers = exception;
	dump.ClientPointers = 0;

	char dumpFileName[0xff] = {};
	sprintf(dumpFileName, "%04d-%02d-%02d %02d-%02d-%02d [p%04x t%04x].dmp", genTime.wYear, genTime.wMonth, genTime.wDay, genTime.wHour, genTime.wMinute, genTime.wSecond, ::GetCurrentProcessId(), dump.ThreadId);

	string fileName = path;
	fileName = fileName.substr(0, fileName.find_last_of("\\/")) + "\\" + dumpFileName;

	HANDLE fileHandler = CreateFileA(fileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == fileHandler)
		return;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), fileHandler, isNormal ? MiniDumpNormal : static_cast<MINIDUMP_TYPE>(MiniDumpWithFullMemory | MiniDumpWithHandleData | MiniDumpWithProcessThreadData | MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo), &dump, nullptr, nullptr);
	CloseHandle(fileHandler);
}

long __stdcall Dumper::UnhandledException(PEXCEPTION_POINTERS exception)
{
	GenerateMiniDump(exception, false);
	return EXCEPTION_EXECUTE_HANDLER;
}

void Dumper::InvalidParameter(const wchar*, const wchar*, const wchar*, uint32, uint64)
{
	::RaiseException(0, 0, 0, nullptr);
}

void Dumper::PureCall()
{
	::RaiseException(0, 0, 0, nullptr);
}

void Dumper::HandlerSignal(int32)
{
	::RaiseException(0, 0, 0, nullptr);
}
