#pragma once

class CKeySequences: public CPropertyPage {
	DECLARE_DYNAMIC( CKeySequences )

public:
	CKeySequences( );
	virtual ~CKeySequences( );

	// Dialog Data
	enum { IDD = IDD_KEYSEQUENCES };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

	DECLARE_MESSAGE_MAP( )
};
