#pragma once

#include "KeyEdit.h"

class CComposeSequenceEditor: public CDialog {
	DECLARE_DYNAMIC( CComposeSequenceEditor )

public:
	CComposeSequenceEditor( COMPOSE_SEQUENCE& sequence, /*XXX*/bool fAddMode, CWnd* pParent = NULL );
	virtual ~CComposeSequenceEditor( );

	// Dialog Data
	enum { IDD = IDD_EDITKEYSEQUENCE };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual void DDX_Char( CDataExchange* pDX, int nIDC, unsigned& ch );
	virtual void DDX_Key( CDataExchange* pDX, int nIDC, unsigned& ch );
	virtual void DDV_Key( CDataExchange* pDX, unsigned& dwVk );

	DECLARE_MESSAGE_MAP( )

private:
	COMPOSE_SEQUENCE& m_sequence;
	bool m_fAdd;
	CString m_strTitle;

	virtual BOOL OnInitDialog( );

	CKeyEdit m_editFirstKey;
	CKeyEdit m_editSecondKey;
	CEdit m_editResult;
};
