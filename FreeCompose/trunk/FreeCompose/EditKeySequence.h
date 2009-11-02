#pragma once
#include "afxwin.h"

class CEditKeySequence: public CDialog {
	DECLARE_DYNAMIC( CEditKeySequence )

public:
	CEditKeySequence( COMPOSE_KEY_ENTRY& cke, bool fAddMode, CWnd* pParent = NULL );
	virtual ~CEditKeySequence( );

	// Dialog Data
	enum { IDD = IDD_EDITKEYSEQUENCE };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

	DECLARE_MESSAGE_MAP( )

private:
	COMPOSE_KEY_ENTRY m_cke;
	bool m_fAdd;

	virtual BOOL OnInitDialog( );

	CEdit m_editFirstKey;
	CEdit m_editSecondKey;
	CEdit m_editResult;
};
