#pragma once

#define _CRGDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#ifdef malloc
#undef malloc
#endif
#define malloc(size) (_malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__))

#ifdef free
#undef free
#endif
#define free(ptr) (_free_dbg(ptr, _NORMAL_BLOCK))
#endif

class MemoryLeakDetector
{
public:
	static void Launch() { _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); }
};