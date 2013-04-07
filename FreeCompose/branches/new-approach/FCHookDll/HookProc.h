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
// Function prototypes
//==============================================================================

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );

void InitializeKeyEventDispatcher( void );
void ChangeComposeKey( DWORD const vkNew );
void ChangeCapsLockSwapKey( DWORD const vkNew );
void ConfigureCapsLockHandling( void );
