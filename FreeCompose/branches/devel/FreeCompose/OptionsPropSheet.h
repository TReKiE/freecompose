#pragma once

#include "KeySequences.h"
#include "Features.h"

extern const UINT APP_RECONFIGURE;

class COptionsPropSheet: public CPropertySheet {
public:
	COptionsPropSheet( const COptionsData& options, CWnd* pParentWnd, UINT iSelectPage = 0 );
	virtual ~COptionsPropSheet( );

	virtual void OnApplyNow( );
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	afx_msg int OnCreate( LPCREATESTRUCT lpcs );

	inline const COptionsData& GetCurrentOptions ( void ) const { return m_CurOptions; }
	inline const COptionsData& GetNewOptions     ( void ) const { return m_NewOptions; }


protected:
	DECLARE_MESSAGE_MAP( )

private:
	COptionsPropSheet( UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	COptionsPropSheet( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );

	CKeySequences m_KeySequences;
	CFeatures m_Features;
	COptionsData m_CurOptions;
	COptionsData m_NewOptions;

	DECLARE_DYNAMIC( COptionsPropSheet )
};
