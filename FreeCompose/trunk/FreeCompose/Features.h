#pragma once

#include "OptionsData.h"

class CFeatures: public CPropertyPage {
	DECLARE_DYNAMIC( CFeatures )

public:
	CFeatures( COptionsData& Options );
	virtual ~CFeatures( );

	// Dialog Data
	enum { IDD = IDD_FEATURES };

protected:
	COptionsData& m_Options;

	virtual void DoDataExchange( CDataExchange* pDX );

	virtual BOOL OnInitDialog( );

	afx_msg void OnCheckboxChanged( UINT uID );

	DECLARE_MESSAGE_MAP( )
};
