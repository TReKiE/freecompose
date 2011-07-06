#pragma once

typedef CArray< COMPOSE_SEQUENCE > CComposeSequenceArray;

class CFreeComposeApp: public CWinApp {
public:
	CFreeComposeApp( );

	virtual BOOL InitInstance( );
	virtual int ExitInstance( );

protected:
	DECLARE_MESSAGE_MAP()
};

extern CFreeComposeApp theApp;
