#pragma once

/* External Include */
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

#include <iostream>
#include <functional>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <algorithm>
using namespace std;

/* Internal Include */
#include "SlimeType.h"
#include "ThreadLock.h"
#include "MemoryLeakDetector.h"
#include "SlimeAllocator.h"
#include "JobQueue.h"
#include "JobTimer.h"
#include "IOCPSession.h"
#include "IOCPModule.h"
#include "Logger.h"