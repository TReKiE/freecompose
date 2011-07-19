#pragma once

#if _MSC_VER
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include "targetver.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <limits>
#include <typeinfo>
#include <intrin.h>

#if _MSC_VER
#include <afx.h>
#include <afxwin.h>
#include <afxext.h>
#include <afxdtctl.h>
#include <afxcmn.h>

#include <atlbase.h>
#include <atlstr.h>
#endif
