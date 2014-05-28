#pragma once

#define _ATL_APARTMENT_THREADED 
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _CRTDBG_MAP_ALLOC
#define  NOMINMAX
#define _SECURE_ATL 1
#define  VC_EXTRALEAN
#define  WIN32_LEAN_AND_MEAN

#ifdef TARGET_VISTA
#	include "targetver_vista.h"
#else
#	include "targetver.h"
#endif

// Microsoft Foundation Classes
#include <afx.h>

// International Components for Unicode
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/brkiter.h>
