#pragma once

#include "OptionsData.h"

class CSounds: public CPropertyPage {
	DECLARE_DYNAMIC( CSounds )
	DECLARE_MESSAGE_MAP( )

public:
	CSounds( COptionsData& Options );
	virtual ~CSounds( );

private:
	COptionsData& m_Options;

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

};
