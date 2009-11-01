#pragma once

#include "KeySequences.h"
#include "Features.h"

class COptionsPropSheet: public CPropertySheet {
	DECLARE_DYNAMIC( COptionsPropSheet )

public:
	COptionsPropSheet( CWnd* pParentWnd, const COptionsData& options );
	virtual ~COptionsPropSheet( );

	inline const COptionsData& GetCurrentOptions ( void ) const { return m_CurOptions; }
	inline const COptionsData& GetNewOptions     ( void ) const { return m_NewOptions; }

protected:

private:
	COptionsPropSheet( UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	COptionsPropSheet( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );

private:
	CKeySequences m_KeySequences;
	CFeatures m_Features;
	COptionsData m_CurOptions;
	COptionsData m_NewOptions;

	afx_msg int OnCreate( LPCREATESTRUCT lpcs );

	DECLARE_MESSAGE_MAP( )
};
