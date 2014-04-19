#pragma once

typedef CArray< COMPOSE_SEQUENCE > CComposeSequenceArray;

class CFreeComposeApp: public CWinApp {
	DECLARE_MESSAGE_MAP( )

public:
	virtual BOOL InitInstance( );
	virtual int ExitInstance( );

protected:
	HANDLE m_hInstanceMutex;

	bool IsAlreadyRunning( );
};

extern CFreeComposeApp theApp;
