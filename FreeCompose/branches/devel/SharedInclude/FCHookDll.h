#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FCHOOKDLL_EXPORTS
#define FCHOOKDLL_API __declspec(dllexport)
#else
#define FCHOOKDLL_API __declspec(dllimport)
#endif

#define FCHOOKDLL_API_VERSION 0x0027

#ifndef NOTYPES
struct COMPOSE_KEY_ENTRY {
	DWORD vkFirst;
	DWORD vkSecond;
	unsigned u32Composed;
};

enum CAPS_LOCK_TOGGLE_MODE {
	CLTM_NORMAL     = 1,
	CLTM_PRESSTWICE,
	CLTM_DISABLED,
};

enum CAPS_LOCK_SWAP_MODE {
	CLSM_SWAP    = 1,
	CLSM_REPLACE,
};
#endif

FCHOOKDLL_API DWORD FcGetApiVersion( void );

FCHOOKDLL_API BOOL FcSetComposeKeyEntries( COMPOSE_KEY_ENTRY* rgEntries, DWORD cEntries );

FCHOOKDLL_API BOOL FcEnableHook( void );
FCHOOKDLL_API BOOL FcDisableHook( void );
FCHOOKDLL_API BOOL FcIsHookEnabled( void );

FCHOOKDLL_API BOOL FcSetCapsLockToggleMode( CAPS_LOCK_TOGGLE_MODE mode );
FCHOOKDLL_API CAPS_LOCK_TOGGLE_MODE FcGetCapsLockToggleMode( void );

FCHOOKDLL_API BOOL FcSetCapsLockSwapMode( CAPS_LOCK_SWAP_MODE mode );
FCHOOKDLL_API CAPS_LOCK_SWAP_MODE FcGetCapsLockSwapMode( void );

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
