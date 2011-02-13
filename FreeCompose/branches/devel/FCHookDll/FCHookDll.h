#pragma once

#ifdef FCHOOKDLL_EXPORTS
#define FCHOOKDLL_API __declspec(dllexport)
#else
#define FCHOOKDLL_API __declspec(dllimport)
#endif

#define FCHOOKDLL_API_VERSION 0x001F

struct COMPOSE_KEY_ENTRY {
	DWORD vkFirst;
	DWORD vkSecond;
	wchar_t wchComposed;
};

FCHOOKDLL_API BOOL FcSetComposeKeyEntries( COMPOSE_KEY_ENTRY* rgEntries, DWORD cEntries );
FCHOOKDLL_API BOOL FcEnableHook( void );
FCHOOKDLL_API BOOL FcDisableHook( void );
FCHOOKDLL_API BOOL FcEnableCapsLock( void );
FCHOOKDLL_API BOOL FcDisableCapsLock( void );
FCHOOKDLL_API DWORD FcGetApiVersion( void );
FCHOOKDLL_API BOOL FcSetComposeKey( DWORD vkCompose );
FCHOOKDLL_API DWORD FcGetComposeKey( void );

const WPARAM PIP_OK_1  = 1;
const WPARAM PIP_OK_2  = 2;
const WPARAM PIP_OK_3  = 3;
const WPARAM PIP_ABORT = 4;
const WPARAM PIP_ERROR = 5;
const WPARAM PIP_FAIL  = 6;
