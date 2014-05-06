#include "stdafx.h"

#include <Unicode.h>

#include "FreeCompose.h"
#include "ComposeSequenceEditor.h"

#include "Utils.h"

IMPLEMENT_DYNAMIC(CComposeSequenceEditor, CDialog)

BEGIN_MESSAGE_MAP(CComposeSequenceEditor, CDialog)
END_MESSAGE_MAP()

CComposeSequenceEditor::CComposeSequenceEditor( ComposeSequence& sequence, SEQUENCE_EDITOR_MODE mode, CWnd* pParent ):
	CDialog    ( IDD, pParent ),
	m_sequence ( sequence ),
	m_mode     ( mode ),
	m_strTitle ( LoadFromStringTable( ( semAdd == mode ) ? IDS_EDITKEYSEQUENCE_TITLE_ADD : IDS_EDITKEYSEQUENCE_TITLE_EDIT ) )
{
}

CComposeSequenceEditor::~CComposeSequenceEditor( ) {
}

void CComposeSequenceEditor::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange( pDX );
	DDX_Control ( pDX, IDC_SEQUENCE,  m_editSequence      );
	DDX_Control ( pDX, IDC_RESULT,    m_editResult        );
	DDX_Text    ( pDX, IDC_SEQUENCE,  m_sequence.Sequence );
	DDX_Result  ( pDX, IDC_RESULT,    m_sequence.Result   );
}

void CComposeSequenceEditor::DDX_Result( CDataExchange* pDX, int nIDC, CString& result ) {
	if ( IDC_RESULT != nIDC ) {
		debug( L"CComposeSequenceEditor::DDX_Char: wrong control ID?!\n", nIDC );
		return;
	}

	CString str;
	unsigned ch = 0;
	if ( pDX->m_bSaveAndValidate ) {
		result.Empty( );
		m_editResult.GetWindowText( str );
		if ( !str.IsEmpty( ) && !Utf16ToUtf32( str, ch ) ) {
			if ( 1 == str.GetLength( ) ) {
				debug( L"CComposeSequenceEditor::DDX_Char: Couldn't convert UTF-16 character U+%04hX to UTF-32?\n", str[0] );
			} else {
				debug( L"CComposeSequenceEditor::DDX_Char: Couldn't convert UTF-16 characters U+%04hX U+%04hX to UTF-32?\n", str[0], str[1] );
			}
			pDX->Fail( );
		} else {
			result = str;
		}
	} else {
		if ( result.IsEmpty( ) ) {
			m_editResult.SetWindowText( L"" );
		} else {
			Utf16ToUtf32( result, ch );
			str.Format( L"U+%06X %s", ch, static_cast<wchar_t const*>( result ) );
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
