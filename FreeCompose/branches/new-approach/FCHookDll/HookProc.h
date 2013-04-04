#pragma once

#include <Bitset.h>

#include "KeyEventHandler.h"

//==============================================================================
// Types
//==============================================================================

enum COMPOSE_STATE {
	csNORMAL,
	csCOMPOSE_FIRST,
	csCOMPOSE_SECOND,
	csCAPS_LOCK_FIRST,
};

//==============================================================================
// Globals
//==============================================================================

extern KeyEventHandler* keyEventHandler[256];
extern COMPOSE_STATE ComposeState;
extern zive::bitset< 256, DWORD > WantedKeys;

//==============================================================================
// Function prototypes
//==============================================================================

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );

void InitializeKeyEventDispatcher( void );
void ChangeComposeKey( DWORD const vkNew );
void ChangeCapsLockSwapKey( DWORD const vkNew );
void ConfigureCapsLockHandling( void );
