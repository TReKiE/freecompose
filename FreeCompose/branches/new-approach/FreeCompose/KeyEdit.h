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

	inline void SetKey( const DWORD vk ) {
		m_dwVk = vk;
		_Update( );
	}

	inline DWORD GetKey( void ) const {
		return m_dwVk;
	}

protected:
	DECLARE_MESSAGE_MAP( )

private:
	DWORD m_dwVk;

	void _Update( );

	DECLARE_DYNAMIC( CKeyEdit )
};
