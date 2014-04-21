#include "stdafx.h"

#include <Unicode.h>

#include "FreeCompose.h"
#include "ComposeSequenceEditor.h"

#include "Utils.h"

IMPLEMENT_DYNAMIC(CComposeSequenceEditor, CDialog)

BEGIN_MESSAGE_MAP(CComposeSequenceEditor, CDialog)
END_MESSAGE_MAP()

CComposeSequenceEditor::CComposeSequenceEditor( COMPOSE_SEQUENCE& sequence, SEQUENCE_EDITOR_MODE mode, CWnd* pParent ):
	CDialog    ( CComposeSequenceEditor::IDD, pParent ),
	m_sequence ( sequence ),
	m_mode     ( mode ),
	m_strTitle ( (LPCWSTR) ( ( semAdd == mode ) ? IDS_EDITKEYSEQUENCE_TITLE_ADD : IDS_EDITKEYSEQUENCE_TITLE_EDIT ) )
{
}

CComposeSequenceEditor::~CComposeSequenceEditor( ) {
}

void CComposeSequenceEditor::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange( pDX );
	DDX_Control ( pDX, IDC_FIRSTKEY,  m_editFirstKey );
	DDX_Control ( pDX, IDC_SECONDKEY, m_editSecondKey );
	DDX_Control ( pDX, IDC_RESULT,    m_editResult );
	DDX_Key     ( pDX, IDC_FIRSTKEY,  m_sequence.chFirst );
	DDX_Key     ( pDX, IDC_SECONDKEY, m_sequence.chSecond );
	DDX_Char    ( pDX, IDC_RESULT,    m_sequence.chComposed );
	DDV_Key     ( pDX, m_sequence.chFirst );
	DDV_Key     ( pDX, m_sequence.chSecond );
}

void CComposeSequenceEditor::DDX_Key( CDataExchange* pDX, int nIDC, unsigned& ch ) {
	if ( IDC_FIRSTKEY == nIDC ) {
		if ( pDX->m_bSaveAndValidate ) {
			ch = m_editFirstKey.GetKey( );
		} else {
			m_editFirstKey.SetKey( ch );
		}
	} else if ( IDC_SECONDKEY == nIDC ) {
		if ( pDX->m_bSaveAndValidate ) {
			ch = m_editSecondKey.GetKey( );
		} else {
			m_editSecondKey.SetKey( ch );
		}
	} else {
		ch = 0;
	}
}

void CComposeSequenceEditor::DDV_Key( CDataExchange* pDX, unsigned& ch ) {
	if ( ! pDX->m_bSaveAndValidate ) {
		return;
	}
	if ( 0 == ch ) {
		MessageBox( CString( (LPCWSTR) IDS_EDITKEYSEQUENCE_CANTBEBLANK ), m_strTitle, MB_OK|MB_ICONERROR );
		pDX->Fail( );
	}
}

void CComposeSequenceEditor::DDX_Char( CDataExchange* pDX, int nIDC, unsigned& ch ) {
	if ( IDC_RESULT != nIDC ) {
		debug( L"CComposeSequenceEditor::DDX_Char: wrong control ID?!\n", nIDC );
		return;
	}

	CString str;
	if ( pDX->m_bSaveAndValidate ) {
		ch = 0;
		m_editResult.GetWindowText( str );
		if ( ! str.IsEmpty( ) && ! Utf16ToUtf32( str, ch ) ) {
			if ( 1 == str.GetLength( ) ) {
				debug( L"CComposeSequenceEditor::DDX_Char: Couldn't convert UTF-16 character U+%04hX to UTF-32?\n", str[0] );
			} else {
				debug( L"CComposeSequenceEditor::DDX_Char: Couldn't convert UTF-16 characters U+%04hX U+%04hX to UTF-32?\n", str[0], str[1] );
			}
			pDX->Fail( );
		}
	} else {
		if ( 0 == ch ) {
			m_editResult.SetWindowText( L"" );
		} else {
			str.Format( L"U+%06X %s", ch, static_cast<wchar_t const*>( Utf32ToUtf16( ch ) ) );
			m_editResult.SetWindowText( str );
		}
	}
}

BOOL CComposeSequenceEditor::OnInitDialog( ) {
	if ( ! CDialog::OnInitDialog( ) )
		return FALSE;

	SetWindowText( m_strTitle );

	// has to be 2 to support UTF-16
	m_editResult.SetLimitText( 2 );
	UpdateData( FALSE );

	return TRUE;
}
