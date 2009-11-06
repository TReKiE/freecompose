#pragma once

#include "KeyEdit.h"

class CEditKeySequence: public CDialog {
	DECLARE_DYNAMIC( CEditKeySequence )

public:
	CEditKeySequence( COMPOSE_KEY_ENTRY& cke, bool fAddMode, CWnd* pParent = NULL );
	virtual ~CEditKeySequence( );

	// Dialog Data
	enum { IDD = IDD_EDITKEYSEQUENCE };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual void DDX_Char( CDataExchange* pDX, int nIDC, wchar_t& wch );
	virtual void DDX_Key( CDataExchange* pDX, int nIDC, DWORD& dwVk );

	DECLARE_MESSAGE_MAP( )

private:
	COMPOSE_KEY_ENTRY& m_cke;
	bool m_fAdd;

	virtual BOOL OnInitDialog( );

	CKeyEdit m_editFirstKey;
	CKeyEdit m_editSecondKey;
	CEdit m_editResult;
};
