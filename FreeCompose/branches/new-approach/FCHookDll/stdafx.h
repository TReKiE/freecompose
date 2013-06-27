#pragma once

//#define UI_DEBUGGING_ONLY 1
#undef UI_DEBUGGING_ONLY

#include "targetver.h"

#pragma warning( disable: 4127 )

// C standard library
#include <cctype>

// C++ standard library
#include <algorithm>
#include <unordered_map>
#include <utility>

// Windows SDK
#include <windows.h>

// FCShared
#include "FCShared.h"
#include "Bitset.h"
#include "AutoCriticalSection.h"

// FCHookDll
#include "FCHookDll.h"
#include "ComposeSequenceTree.h"
#include "Common.h"
#include "KeyEventHandler.h"
