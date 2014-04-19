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

extern UINT const APP_ACTIVATE;
extern UINT const APP_RECONFIGURE;
extern UINT const APP_NOTIFYICON;

extern UINT const FCM_KEY;
extern UINT const FCM_PIP;
