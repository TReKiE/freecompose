#pragma once

//==============================================================================
// Types
//==============================================================================

enum COMPOSE_STATE {
	csNORMAL,
	csCOMPOSE,
	csCAPS_LOCK,
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

extern ComposeSequence*           ComposeSequences;
extern INT_PTR                    cComposeSequences;

extern CComposeSequenceTree       ComposeSequenceTree;

extern CAPS_LOCK_TOGGLE_MODE      clToggleMode;
extern CAPS_LOCK_SWAP_MODE        clSwapMode;
extern bool                       fSwapCapsLock;

extern DWORD                      vkCompose;
extern DWORD                      vkCapsLockSwap;

extern KeyEventHandler*           KeyEventHandlers[256];
extern COMPOSE_STATE              ComposeState;

extern HINSTANCE                  hDllInst;

extern HWND                       hwndNotifyWindow;

//==============================================================================
// Prototypes
//==============================================================================

int CompareComposeSequences( const void*, const void* );
void ReleaseComposeSequences( void );

//==============================================================================
// Functions
//==============================================================================

namespace Stringify {

	extern wchar_t const* stringsFor_enum_COMPOSE_STATE[];

	static inline wchar_t const* from_COMPOSE_STATE( COMPOSE_STATE const value ) {
		return stringsFor_enum_COMPOSE_STATE[static_cast<int>( value )];
	}

};
