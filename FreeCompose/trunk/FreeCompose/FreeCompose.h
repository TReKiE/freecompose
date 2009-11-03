// FreeCompose.h : main header file for the FreeCompose application
//
#pragma once

typedef CArray< COMPOSE_KEY_ENTRY > CComposeKeyEntryArray;

class CFreeComposeApp: public CWinApp {
public:
	CFreeComposeApp( );

// Overrides
public:
	virtual BOOL InitInstance( );

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

extern CFreeComposeApp theApp;
