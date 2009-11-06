#include "stdafx.h"

#include "FreeCompose.h"
#include "KeyEdit.h"

#include "Utils.h"

IMPLEMENT_DYNAMIC(CKeyEdit, CEdit)

BEGIN_MESSAGE_MAP(CKeyEdit, CEdit)
END_MESSAGE_MAP()

CKeyEdit::CKeyEdit( ):
	dwVk ( 0 )
{
}

CKeyEdit::~CKeyEdit( ) {
}

void CKeyEdit::SetKey( const DWORD _dwVk ) {
	dwVk = _dwVk;
	SetWindowText( VkToString( dwVk ) );
}

DWORD CKeyEdit::GetKey( void ) const {
	return dwVk;
}
