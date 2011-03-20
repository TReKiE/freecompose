#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FCHOOKDLL_EXPORTS
#define FCHOOKDLL_API __declspec(dllexport)
#else
#define FCHOOKDLL_API __declspec(dllimport)
#endif

#define FCHOOKDLL_API_VERSION 0x0024

struct COMPOSE_KEY_ENTRY {
	DWORD vkFirst;
	DWORD vkSecond;
	unsigned u32Composed;
};

enum CAPS_LOCK_MODE {
	CLM_NORMAL     = 1,
	CLM_PRESSTWICE,
	CLM_DISABLED,
};

FCHOOKDLL_API DWORD FcGetApiVersion( void );

FCHOOKDLL_API BOOL FcSetComposeKeyEntries( COMPOSE_KEY_ENTRY* rgEntries, DWORD cEntries );

FCHOOKDLL_API BOOL FcEnableHook( void );
FCHOOKDLL_API BOOL FcDisableHook( void );

FCHOOKDLL_API BOOL FcSetCapsLockMode( CAPS_LOCK_MODE mode );
FCHOOKDLL_API CAPS_LOCK_MODE FcGetCapsLockMode( void );

FCHOOKDLL_API BOOL FcEnableSwapCapsLock( void );
FCHOOKDLL_API BOOL FcDisableSwapCapsLock( void );

FCHOOKDLL_API BOOL FcSetComposeKey( DWORD vkCompose );
FCHOOKDLL_API DWORD FcGetComposeKey( void );

FCHOOKDLL_API BOOL FcSetSwapCapsLockKey( DWORD vkCapsLockSwap );
FCHOOKDLL_API DWORD FcGetSwapCapsLockKey( void );

const WPARAM PIP_OK_1  = 1;
const WPARAM PIP_OK_2  = 2;
const WPARAM PIP_OK_3  = 3;
const WPARAM PIP_ABORT = 4;
const WPARAM PIP_ERROR = 5;
const WPARAM PIP_FAIL  = 6;

#ifdef __cplusplus
};
#endif
