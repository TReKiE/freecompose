#pragma once

class CKeyEdit: public CEdit {
public:
	CKeyEdit( );
	virtual ~CKeyEdit( );

	afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg int  OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnEnSetFocus( );
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );

	void SetKey( const DWORD _dwVk );
	DWORD GetKey( void ) const;

protected:
	DECLARE_MESSAGE_MAP( )

private:
	DWORD dwVk;

	void _ChangeText( const DWORD dwVk );

	DECLARE_DYNAMIC( CKeyEdit )
};
