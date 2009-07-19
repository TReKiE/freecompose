// FreeCompose.h : main header file for the FreeCompose application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CFreeComposeApp:
// See FreeCompose.cpp for the implementation of this class
//

class CFreeComposeApp : public CWinApp
{
public:
	CFreeComposeApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	afx_msg void OnAppEnable();
	afx_msg void OnAppDisable();
	afx_msg void OnAppConfigure();
	DECLARE_MESSAGE_MAP()
//public:
//	BOOL ExitInstance(void);
};

extern CFreeComposeApp theApp;
extern CArray< COMPOSE_KEY_ENTRY > ComposeKeyEntries;

void debug( LPCTSTR lpsz, ... );

template<typename T> inline T compare_keys( void* /*context*/, const void* _elem1, const void* _elem2 ) {
	T elem1 = *( (T*) _elem1 );
	T elem2 = *( (T*) _elem2 );

	if ( elem1 < elem2 )
		return -1;
	else if ( elem1 > elem2 )
		return 1;
	else
		return 0;
}

