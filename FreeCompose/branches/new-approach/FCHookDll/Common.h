#pragma once

#include "AutoCriticalSection.h"

//==============================================================================
// Types
//==============================================================================

enum COMPOSE_STATE {
	csNORMAL,
	csCOMPOSE_FIRST,
	csCOMPOSE_SECOND,
	csCAPS_LOCK_FIRST,
};

class KeyEventHandler;

//==============================================================================
// Constants
//==============================================================================

extern UINT const FCM_PIP;
extern UINT const FCM_KEY;

//==============================================================================
// Global variables
//==============================================================================

extern AutoCriticalSection        cs;
extern HHOOK                      hHook;

extern COMPOSE_SEQUENCE*          ComposeSequences;
extern INT_PTR                    cComposeSequences;

extern CAPS_LOCK_TOGGLE_MODE      clToggleMode;
extern CAPS_LOCK_SWAP_MODE        clSwapMode;
extern bool                       fSwapCapsLock;

extern DWORD                      vkCompose;
extern DWORD                      vkCapsLockSwap;

extern KeyEventHandler*           KeyEventHandlers[256];
extern COMPOSE_STATE              ComposeState;
extern zive::bitset< 256, DWORD > WantedKeys;

extern HINSTANCE                  hDllInst;

extern HWND                       hwndNotifyWindow;

//==============================================================================
// Prototypes
//==============================================================================

int CompareComposeSequences( const void*, const void* );
void ReleaseComposeSequences( void );
