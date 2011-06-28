#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#ifdef TARGET_VISTA
#include "targetver_vista.h"
#else
#include "targetver.h"
#endif

#include <Windows.h>
#include <ObjBase.h>

#ifdef FCSHARED_EXPORTS
#define FCSHARED_API __declspec(dllexport)
#else
#define FCSHARED_API __declspec(dllimport)
#endif
