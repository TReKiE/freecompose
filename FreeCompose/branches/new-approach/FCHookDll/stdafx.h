#pragma once

#define UI_DEBUGGING_ONLY 1

#include "targetver.h"

#pragma warning( disable: 4127 )

// C++ standard library
#include <cctype>
#include <unordered_map>

// Windows SDK
#include <windows.h>

// FCShared
#include "FCShared.h"
#include "Bitset.h"
#include "AutoCriticalSection.h"

// FCHookDll
#include "FCHookDll.h"
#include "Common.h"
#include "KeyEventHandler.h"
