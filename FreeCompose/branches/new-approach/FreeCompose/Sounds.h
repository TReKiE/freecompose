#pragma once

class CSounds: public CPropertyPage {
	DECLARE_DYNAMIC( CSounds )
	DECLARE_MESSAGE_MAP( )

public:
	enum { IDD = IDD_SOUNDS };

	CSounds( );
	virtual ~CSounds( );

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
};
