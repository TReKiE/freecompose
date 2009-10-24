#pragma once
#include "KeySequences.h"

class COptionsPropSheet: public CPropertySheet {
	DECLARE_DYNAMIC( COptionsPropSheet )

public:
	COptionsPropSheet( );
	virtual ~COptionsPropSheet( );

private:
	COptionsPropSheet( UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	COptionsPropSheet( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );

protected:
	DECLARE_MESSAGE_MAP( )

	CKeySequences m_page1;
};
