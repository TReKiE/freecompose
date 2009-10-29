#pragma once

class CFeatures: public CPropertyPage {
	DECLARE_DYNAMIC( CFeatures )

public:
	CFeatures( );
	virtual ~CFeatures( );

	// Dialog Data
	enum { IDD = IDD_FEATURES };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

	DECLARE_MESSAGE_MAP( )
};
