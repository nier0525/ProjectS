#pragma once

class Dumper
{
private:
	Dumper() = default;
	~Dumper() = default;

public:
	static void	Launch();

private:
	static void	GenerateMiniDump(PEXCEPTION_POINTERS exception, bool isNormal);
	static long __stdcall UnhandledException(PEXCEPTION_POINTERS exception);
	static void	InvalidParameter(const wchar*, const wchar*, const wchar*, uint32, uint64);
	static void	PureCall();
	static void	HandlerSignal(int32);
};