#pragma once

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );

void InitializeKeyEventDispatcher( void );
void ChangeComposeKey( DWORD const vkNew );
void ConfigureCapsLockHandling( void );
