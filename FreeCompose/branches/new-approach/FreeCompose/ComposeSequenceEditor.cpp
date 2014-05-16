#include "stdafx.h"

#include <Unicode.h>

#include "FreeCompose.h"
#include "ComposeSequenceEditor.h"

#include "Utils.h"

//==============================================================================
//
// Guide to this dialog's controls
// ===============================
//
// Type    Resource ID
// ------  --------------------------
// Edit    IDC_EDITKEYSEQUENCE
//
// Radio   IDC_RESULT_ASCHARACTER
// Radio   IDC_RESULT_ASHEXCODEPOINT
// Radio   IDC_RESULT_ASDECCODEPOINT
// Edit    IDC_EDITRESULT
//
// Check   IDC_OPTION_ENABLED
// Check   IDC_OPTION_CASEINSENSITIVE
// Check   IDC_OPTION_REVERSIBLE
//
// Button  IDOK_ADDANOTHER
// Button  IDOK
// Button  IDCANCEL
//
//==============================================================================

enum ResultMode {
	rmCharacter,
	rmHexCodePoint,
	rmDecCodePoint,
};

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

	DDX_Control ( pDX, IDC_EDITKEYSEQUENCE,        m_editKeySequence           );
	DDX_Control ( pDX, IDC_EDITRESULT,             m_editResult                );
	DDX_Control ( pDX, IDC_RESULT_ASCHARACTER,     m_radioResultAsCharacter    );
	DDX_Control ( pDX, IDC_RESULT_ASHEXCODEPOINT,  m_radioResultAsHexCodePoint );
	DDX_Control ( pDX, IDC_RESULT_ASDECCODEPOINT,  m_radioResultAsDecCodePoint );
	DDX_Control ( pDX, IDC_OPTION_ENABLED,         m_checkEnabled              );
	DDX_Control ( pDX, IDC_OPTION_CASEINSENSITIVE, m_checkCaseInsensitive      );
	DDX_Control ( pDX, IDC_OPTION_REVERSIBLE,      m_checkReversible           );

	DDX_Text    ( pDX, IDC_EDITKEYSEQUENCE,        m_strKeySequence            );
	DDX_Radio   ( pDX, IDC_RESULT_ASCHARACTER,     m_nResultMode               );
	DDX_Check   ( pDX, IDC_OPTION_ENABLED,         m_fEnabled                  );
	DDX_Check   ( pDX, IDC_OPTION_CASEINSENSITIVE, m_fCaseInsensitive          );
	DDX_Check   ( pDX, IDC_OPTION_REVERSIBLE,      m_fReversible               );

	DDX_Result  ( pDX, IDC_EDITRESULT,             m_strResult                 );
}

void CComposeSequenceEditor::DDX_Result( CDataExchange* pDX, int nIDC, CString& result ) {
	if ( IDC_RESULT != nIDC ) {
		debug( L"CComposeSequenceEditor::DDX_Result: wrong control ID %d?!\n", nIDC );
		return;
	}

	if ( pDX->m_bSaveAndValidate ) {
		result.Empty( );

		int cchWindowText = m_editResult.GetWindowTextLength( );
		wchar_t* pwzWindowText = new wchar_t[cchWindowText + 1];
		int cchCopied = m_editResult.GetWindowText( pwzWindowText, cchWindowText + 1 );
		icu::UnicodeString ustrResult( pwzWindowText, cchCopied );

		switch ( m_nResultMode ) {
			case rmCharacter: {
				if ( !m_CompositeCharacter.SetContents( ustrResult ) ) {
					pDX->Fail( );
				}
				// TODO update new 'character display' static
				break;
			}

			case rmHexCodePoint:
			case rmDecCodePoint:
				{
					// TODO use u_digit()!

					//CArray<CString> parts;
					//int index = 0;
					//
					//str = str.Trim( );
					//int limit = str.GetLength( );
					//
					//while ( index < limit && iswspace( str[index] ) ) {
					//	index++;
					//}
				}
				break;

			default:
				break;
		}
	} else {
		// TODO
		//if ( result.IsEmpty( ) ) {
			m_editResult.SetWindowText( L"" );
		//} else {
		//	Utf16ToUtf32( result, ch );
		//	str.Format( L"U+%06X %s", ch, static_cast<wchar_t const*>( result ) );
		//	m_editResult.SetWindowText( str );
		//}
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
