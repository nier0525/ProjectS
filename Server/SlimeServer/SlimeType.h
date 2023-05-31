#pragma once
#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

using mchar = char;
using wchar = wchar_t;

#define MAX_PACKET_SIZE 8192

#define CRASH(reason)						\
{											\
	int32* crash = nullptr;					\
	__analysis_assume(crash != nullptr);	\
	*crash = 0;								\
}

#define DECLARE_SINGLE(Type)													\
public:																			\
	static Type* Instance()														\
	{																			\
		struct MakeEnabler : public Type {};									\
		static shared_ptr<MakeEnabler> instance = make_shared<MakeEnabler>();	\
		return static_cast<Type*>(instance.get());								\
	}

#define GET_SINGLE(Type) Type::Instance()

#define DECLARE_SHARED(Type) using Type##Ref = shared_ptr<class Type>
#define GET_SHARED(Type) static_pointer_cast<Type>(shared_from_this())
