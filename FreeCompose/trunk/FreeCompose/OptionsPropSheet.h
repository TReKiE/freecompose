#pragma once
#include "KeySequences.h"

class COptionsPropSheet: public CPropertySheet {
	DECLARE_DYNAMIC( COptionsPropSheet )

public:
	COptionsPropSheet( CWnd* pParentWnd );
	virtual ~COptionsPropSheet( );

protected:

private:
	COptionsPropSheet( UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	COptionsPropSheet( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );

private:
	CKeySequences m_KeySequences;

	afx_msg int OnCreate( LPCREATESTRUCT lpcs );

	DECLARE_MESSAGE_MAP( )
};
