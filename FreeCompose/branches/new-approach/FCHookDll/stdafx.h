#pragma once

#define _ATL_APARTMENT_THREADED 
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _CRTDBG_MAP_ALLOC
#define  NOMINMAX
#define _SECURE_ATL 1
#define  VC_EXTRALEAN
#define  WIN32_LEAN_AND_MEAN

#include "targetver.h"

#pragma warning( disable: 4127 )

// C standard library
#include <cctype>
#include <crtdbg.h>
#include <cstdlib>
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

// International Components for Unicode
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/brkiter.h>

// FCShared
#include "FCShared.h"
#include "Bitset.h"
#include "AutoCriticalSection.h"

// FCHookDll
#include "FCHookDll.h"
#include "ComposeSequenceTree.h"
#include "Common.h"
#include "KeyEventHandler.h"
