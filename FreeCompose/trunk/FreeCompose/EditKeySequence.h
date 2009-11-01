#pragma once

class CEditKeySequence: public CDialog {
	DECLARE_DYNAMIC( CEditKeySequence )

public:
	CEditKeySequence( CWnd* pParent = NULL );
	virtual ~CEditKeySequence( );

	// Dialog Data
	enum { IDD = IDD_EDITKEYSEQUENCE };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

	DECLARE_MESSAGE_MAP( )
};
