#pragma once

class CKeyEdit: public CEdit {
public:
	CKeyEdit( );
	virtual ~CKeyEdit( );

	void SetKey( const DWORD _dwVk );
	DWORD GetKey( void ) const;

protected:
	DECLARE_MESSAGE_MAP( )

private:
	DWORD dwVk;

	DECLARE_DYNAMIC( CKeyEdit )
};
