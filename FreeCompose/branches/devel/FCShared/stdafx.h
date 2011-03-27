#pragma once

#define WIN32_LEAN_AND_MEAN
#include "targetver.h"

#include <windows.h>

#ifdef FCSHARED_EXPORTS
#define FCSHARED_API __declspec(dllexport)
#else
#define FCSHARED_API __declspec(dllimport)
#endif
