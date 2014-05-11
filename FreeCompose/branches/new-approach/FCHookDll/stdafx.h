#pragma once

// Control whether to disable all backend functionality so the UI may be debugged in relative isolation.
#undef UI_DEBUGGING_ONLY

// Enable redirection of malloc, free, etc. to _malloc_dbg, _free_dbg, etc.
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#include "targetver.h"

#pragma warning( disable: 4127 )

// C standard library
#include <cctype>
#include <cstdlib>
#include <crtdbg.h>
#include <malloc.h>

// C++ standard library
#include <algorithm>
#include <map>
#include <utility>

// MFC
#include <afx.h>
#include <afxtempl.h>

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
