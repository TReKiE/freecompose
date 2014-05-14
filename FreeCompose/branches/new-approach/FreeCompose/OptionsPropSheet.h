#pragma once

#include "KeySequences.h"
#include "Features.h"
#include "Sounds.h"

extern const UINT APP_RECONFIGURE;

class COptionsPropSheet: public CPropertySheet {
	DECLARE_DYNAMIC( COptionsPropSheet )
	DECLARE_MESSAGE_MAP( )

public:
	COptionsPropSheet( const COptionsData& options, CWnd* pParentWnd, UINT iSelectPage = 0 );
	virtual ~COptionsPropSheet( );

	inline const COptionsData& GetCurrentOptions ( void ) const { return m_CurOptions; }
	inline const COptionsData& GetNewOptions     ( void ) const { return m_NewOptions; }

private:
	COptionsPropSheet( UINT    nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0 );
	COptionsPropSheet( LPCWSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0 );

	virtual BOOL OnInitDialog( );
	virtual void OnApplyNow( );

	afx_msg int OnCreate( LPCREATESTRUCT lpcs );

	CKeySequences m_KeySequences;
	CFeatures m_Features;
	CSounds m_Sounds;

	COptionsData m_CurOptions;
	COptionsData m_NewOptions;
};
