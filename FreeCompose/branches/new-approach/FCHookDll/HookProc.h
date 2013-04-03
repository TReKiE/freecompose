#pragma once

#include "KeyEventHandler.h"

//==============================================================================
// Globals
//==============================================================================

//#pragma data_seg( push, ".shareddata" )

extern KeyEventHandler* keyEventHandler[256];

//#pragma data_seg( pop )

//==============================================================================
// Function prototypes
//==============================================================================

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );

void InitializeKeyEventDispatcher( void );
void ChangeComposeKey( DWORD const vkNew );
void ConfigureCapsLockHandling( void );
