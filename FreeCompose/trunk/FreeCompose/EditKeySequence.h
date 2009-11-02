#pragma once
#include "afxwin.h"

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

private:
	CEdit m_editFirstKey;
	CEdit m_editSecondKey;
	CEdit m_editResult;
};
