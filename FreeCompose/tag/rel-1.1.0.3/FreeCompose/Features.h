#pragma once

#include "OptionsData.h"

class CFeatures: public CPropertyPage {
public:
	enum { IDD = IDD_FEATURES };

	CFeatures( COptionsData& Options );
	virtual ~CFeatures( );

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	afx_msg void OnCheckboxChanged( UINT uID );

protected:
	DECLARE_MESSAGE_MAP( )

private:
	COptionsData& m_Options;

	DECLARE_DYNAMIC( CFeatures )
};
