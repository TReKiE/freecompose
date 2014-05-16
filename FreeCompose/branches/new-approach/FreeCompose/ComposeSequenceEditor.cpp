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

	DDX_Result  ( pDX, IDC_EDITRESULT,             m_strResult );
}

void CComposeSequenceEditor::DDX_Result( CDataExchange* pDX, int nIDC, CString& result ) {
	if ( IDC_RESULT != nIDC ) {
		debug( L"CComposeSequenceEditor::DDX_Result: wrong control ID %d?!\n", nIDC );
		return;
	}

	CString str;
	unsigned ch = 0;
	if ( pDX->m_bSaveAndValidate ) {
		result.Empty( );
		m_editResult.GetWindowText( str );
		switch ( m_nResultMode ) {
			case rmCharacter: {
				if ( str.IsEmpty( ) ) {
					return;
				}

				UErrorCode status = U_ZERO_ERROR;

				BreakIterator* pbi = BreakIterator::createCharacterInstance( Locale::getDefault( ), status );
				UnicodeString ustr( static_cast<LPCWSTR>( str ) );
				pbi->setText( ustr );

				int32_t p1 = pbi->first( );
				int32_t p2 = BreakIterator::DONE;
				if ( BreakIterator::DONE != p1 ) {
					p2 = pbi->next( );
				}

				if ( BreakIterator::DONE != p1 ) {
					result = CString( str, p2 - p1 );

					int32_t cbResult = 0;
					status = U_ZERO_ERROR;

					u_strToUTF32( nullptr, 0, &cbResult, static_cast<LPCWSTR>( result ), -1, &status );
					if ( U_ZERO_ERROR != status ) {
						debug( L"CComposeSequenceEditor::DDX_Result: u_strToUTF32 failed, status=%d\n", status );
						return;
					}

					int32_t cbStr = cbResult;
					UChar32* pqzStr = new UChar32[cbStr];
					u_strToUTF32( pqzStr, cbStr, &cbResult, static_cast<LPCWSTR>( result ), -1, &status );
					if ( U_ZERO_ERROR != status ) {
						debug( L"CComposeSequenceEditor::DDX_Result: u_strToUTF32 failed, status=%d\n", status );
						return;
					}

					delete[] pqzStr;
				} else {
					pDX->Fail( );
				}

				delete pbi;
				break;
			}

			case rmHexCodePoint:
			case rmDecCodePoint:
				{
					CArray<CString> parts;
					int index = 0;

					str = str.Trim( );
					int limit = str.GetLength( );

					while ( index < limit && iswspace( str[index] ) ) {
						index++;
					}
				}
				break;

			default:
				break;
		}
		if ( !str.IsEmpty( ) && !Utf16ToUtf32( str, ch ) ) {
			if ( 1 == str.GetLength( ) ) {
				debug( L"CComposeSequenceEditor::DDX_Result: Couldn't convert UTF-16 character U+%04hX to UTF-32?\n", str[0] );
			} else {
				debug( L"CComposeSequenceEditor::DDX_Result: Couldn't convert UTF-16 characters U+%04hX U+%04hX to UTF-32?\n", str[0], str[1] );
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
