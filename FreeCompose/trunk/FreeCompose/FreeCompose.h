#pragma once

typedef CArray< COMPOSE_KEY_ENTRY > CComposeKeyEntryArray;

class CFreeComposeApp: public CWinApp {
public:
	CFreeComposeApp( );

	virtual BOOL InitInstance( );

protected:
	DECLARE_MESSAGE_MAP()
};

extern CFreeComposeApp theApp;
