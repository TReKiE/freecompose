#pragma once

#include "KeyEventHandler.h"

//==============================================================================
// Globals
//==============================================================================

extern KeyEventHandler* keyEventHandler[256];

//==============================================================================
// Function prototypes
//==============================================================================

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );

void InitializeKeyEventDispatcher( void );
void ChangeComposeKey( DWORD const vkNew );
void ChangeCapsLockSwapKey( DWORD const vkNew );
void ConfigureCapsLockHandling( void );
