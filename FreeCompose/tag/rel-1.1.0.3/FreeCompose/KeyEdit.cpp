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

CKeyEdit::CKeyEdit( ):
	dwVk ( 0 )
{
}

CKeyEdit::~CKeyEdit( ) {
}

void CKeyEdit::_ChangeText( const DWORD dwVk ) {
	CString str( VkToString( dwVk ) );
	SetWindowText( str );
	SetSel( str.GetLength( ), str.GetLength( ), TRUE );
}

void CKeyEdit::SetKey( const DWORD _dwVk ) {
	dwVk = _dwVk;
	_ChangeText( dwVk );
}

DWORD CKeyEdit::GetKey( void ) const {
	return dwVk;
}

void CKeyEdit::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) {
	debug( _T("OnKeyDown: nChar=%u nRepCnt=%u nFlags=%08x\n"), nChar, nRepCnt, nFlags );

	if ( ( GetKeyState( VK_MENU ) & 0x8000 ) == 0x8000 ) {
		return;
	}
	if ( ( GetKeyState( VK_CONTROL ) & 0x8000 ) == 0x8000 ) {
		return;
	}
	if ( ! KEY_XALNUM( nChar ) )
		return;

	DWORD dwCapital = 0;
	if ( ( GetKeyState( VK_SHIFT ) & 0x8000 ) == 0x8000 ) {
		dwCapital = 0x80000000;
	}

	dwVk = nChar | dwCapital;

	CString str( VkToString( dwVk ) );

	SetWindowText( str );
	SetSel( str.GetLength( ), str.GetLength( ), TRUE );

//	CEdit::OnKeyDown( nChar, nRepCnt, nFlags );
}

void CKeyEdit::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags ) {
	debug( _T("OnKeyDown: nChar=%u nRepCnt=%u nFlags=%08x\n"), nChar, nRepCnt, nFlags );

	if ( ( GetKeyState( VK_MENU ) & 0x8000 ) == 0x8000 ) {
		return;
	}
	if ( ( GetKeyState( VK_CONTROL ) & 0x8000 ) == 0x8000 ) {
		return;
	}
	if ( ! KEY_XALNUM( nChar ) )
		return;

	CString str( VkToString( dwVk ) );
	SetWindowText( str );
	SetSel( str.GetLength( ), str.GetLength( ), TRUE );
}

void CKeyEdit::OnChar(UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/) {
	_ChangeText( dwVk );
}

int CKeyEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	debug( _T("OnCreate: dwVk=%08x\n"), dwVk );
	_ChangeText( dwVk );

	return 0;
}

void CKeyEdit::OnEnSetFocus( ) {
	_ChangeText( dwVk );
}
