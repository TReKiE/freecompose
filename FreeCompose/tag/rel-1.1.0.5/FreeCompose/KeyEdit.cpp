#include "stdafx.h"

#include "FreeCompose.h"
#include "KeyEdit.h"

#include "Utils.h"

IMPLEMENT_DYNAMIC(CKeyEdit, CEdit)

BEGIN_MESSAGE_MAP(CKeyEdit, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(EN_SETFOCUS, &CKeyEdit::OnEnSetFocus)
END_MESSAGE_MAP()

#define KEY_XALNUM(x) ( ( (x) >= '0'      && (x) <= '9'      ) || \
					    ( (x) >= 'A'      && (x) <= 'Z'      ) || \
					    ( (x) >= VK_OEM_1 && (x) <= VK_OEM_3 ) || \
					    ( (x) >= VK_OEM_4 && (x) <= VK_OEM_7 )    )

CKeyEdit::CKeyEdit( ): m_dwVk ( 0 ) {
}

CKeyEdit::~CKeyEdit( ) {
}

void CKeyEdit::_Update( ) {
	if ( 0 == m_dwVk ) {
		SetWindowText( L"" );
		return;
	}

	CString str( VkToString( m_dwVk ) );
	SetWindowText( str );
	SetSel( str.GetLength( ), str.GetLength( ), TRUE );
}

void CKeyEdit::OnKeyDown( UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/ ) {
	//debug( _T("OnKeyDown: nChar=%u nRepCnt=%u nFlags=%08x\n"), nChar, nRepCnt, nFlags );

	if ( IsKeyDown( VK_MENU ) ) {
		return;
	}
	if ( IsKeyDown( VK_CONTROL ) ) {
		return;
	}
	if ( ! KEY_XALNUM( nChar ) )
		return;

	m_dwVk = (DWORD) nChar | ( IsKeyDown( VK_SHIFT ) ? 0x80000000UL : 0UL );
	_Update( );
}

void CKeyEdit::OnKeyUp( UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/ ) {
	// do nothing
}

void CKeyEdit::OnChar( UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/ ) {
	// do nothing
}

int CKeyEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if ( -1 == CEdit::OnCreate(lpCreateStruct) ) {
		return -1;
	}

	_Update( );
	return 0;
}

void CKeyEdit::OnEnSetFocus( ) {
	//_Update( );
	int n = GetWindowTextLength( );
	SetSel( n, n, TRUE );
}
