#include "stdafx.h"

#include <Unicode.h>

#include "FreeCompose.h"
#include "ComposeSequenceEditor.h"

#include "Utils.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

//==============================================================================
//
// Guide to this dialog's controls
// ===============================
//
// Type    Resource ID
// ------  --------------------------
// Edit    IDC_CSE_SEQUENCE
//
// Radio   IDC_CSE_RESULT_AS_CHARACTER
// Radio   IDC_CSE_RESULT_AS_HEX
// Radio   IDC_CSE_RESULT_AS_DECIMAL
// Edit    IDC_CSE_RESULT
//
// Check   IDC_CSE_ENABLED
// Check   IDC_CSE_CASE_INSENSITIVE
// Check   IDC_CSE_REVERSIBLE
//
// Button  IDOK_ADDANOTHER
// Button  IDOK
// Button  IDCANCEL
//
//==============================================================================

IMPLEMENT_DYNAMIC( CComposeSequenceEditor, CDialog )

BEGIN_MESSAGE_MAP( CComposeSequenceEditor, CDialog )
	ON_WM_DRAWITEM( )

	ON_BN_CLICKED( IDOK,             &CComposeSequenceEditor::OnOK                    )
	ON_BN_CLICKED( IDOK_ADDANOTHER,  &CComposeSequenceEditor::OnOKAddAnother          )
	ON_EN_UPDATE ( IDC_CSE_SEQUENCE, &CComposeSequenceEditor::OnUpdateComposeSequence )
	ON_EN_UPDATE ( IDC_CSE_RESULT,   &CComposeSequenceEditor::OnUpdateComposeResult   )

	ON_CONTROL_RANGE( BN_CLICKED, IDC_CSE_RESULT_AS_CHARACTER, IDC_CSE_RESULT_AS_DECIMAL, &CComposeSequenceEditor::OnResultModeClicked )
	ON_CONTROL_RANGE( BN_CLICKED, IDC_CSE_ENABLED,             IDC_CSE_REVERSIBLE,        &CComposeSequenceEditor::OnCheckboxClicked   )
END_MESSAGE_MAP( )

enum ResultMode {
	rmCharacter,
	rmHexCodePoint,
	rmDecCodePoint,
};

int static nLastRadioGroupSelection = -1;

CComposeSequenceEditor::CComposeSequenceEditor( ComposeSequence& sequence, SEQUENCE_EDITOR_MODE mode, CWnd* pParent ):
	CDialog     ( IDD, pParent ),
	m_sequence  ( sequence ),
	m_mode      ( mode ),
	m_pFont     ( nullptr ),
	m_fModified ( false )
{

}

CComposeSequenceEditor::~CComposeSequenceEditor( ) {
	if ( m_pFont ) {
		delete m_pFont;
		m_pFont = nullptr;
	}
}

bool CComposeSequenceEditor::_ParseCodePointList( CString const& input, int const base, CArray<UChar32>& output ) {
	int const GcAlnumMask = U_GC_LC_MASK | U_GC_ND_MASK;
	int const GcDelimMask = U_GC_P_MASK  | U_GC_S_MASK  | U_GC_ZS_MASK;

	int cchInput = 0;
	std::unique_ptr<UChar32[]> ptrInput( Utf16ToUtf32( input, input.GetLength( ), cchInput ) );
	UChar32* pqzInput = ptrInput.get( );
	UChar32 formingCharacter = 0;
	bool converting = false;

	output.RemoveAll( );

	for ( int index = 0; index < cchInput; index++ ) {
		UChar32 ch = pqzInput[index];
		unsigned mask = U_GET_GC_MASK( ch );
		if ( 0 != ( mask & GcAlnumMask ) ) {
			converting = true;

			int digit = u_digit( ch, static_cast<int8_t>( base ) );
			if ( -1 == digit ) {
				debug( L"CComposeSequenceEditor::_ParseCodePointList: Unacceptable alphanumeric value &#%d;\n", ch );
				return false;
			}

			formingCharacter = formingCharacter * base + digit;
			if ( formingCharacter > UCHAR_MAX_VALUE ) {
				debug( L"CComposeSequenceEditor::_ParseCodePointList: Code point value overflow, 0x%X\n", ch );
				return false;
			}
		} else if ( 0 != ( mask & GcDelimMask ) ) {
			if ( converting ) {
				output.Add( formingCharacter );
				formingCharacter = 0;
				converting = false;
			}
		} else {
			debug( L"CComposeSequenceEditor::_ParseCodePointList: Unacceptable character &#%d;\n", ch );
			return false;
		}
	}
	if ( converting ) {
		output.Add( formingCharacter );
	}

	return true;
}

void CComposeSequenceEditor::_SetResultFromInput( void ) {
	debug( L"CComposeSequenceEditor::_SetResultFromInput()\n" );

	m_editComposeResult.GetWindowText( m_strResultInput );

	CString str;
	switch ( m_nResultMode ) {
		case rmCharacter:
			str = m_strResultInput;
			break;

		case rmHexCodePoint:
		case rmDecCodePoint: {
			if ( m_strResultInput.IsEmpty( ) ) {
				break;
			}

			CArray<UChar32> codePoints;
			if ( _ParseCodePointList( m_strResultInput, ( rmHexCodePoint == m_nResultMode ) ? 16 : 10, codePoints ) ) {
				str = Utf32ToUtf16( codePoints.GetData( ), codePoints.GetCount( ) );
			}
			break;
		}

		default:
			debug( L"CComposeSequenceEditor::_SetResultFromInput: unknown result mode %d\n", m_nResultMode );
			return;
	}

	if ( !m_CompositeCharacter.SetContents( str ) ) {
		return;
	}

	m_strComposeResult = str;
	m_staticPreview.Invalidate( FALSE );
}

void CComposeSequenceEditor::_SetInputFromResult( void ) {
	debug( L"CComposeSequenceEditor::_SetInputFromResult()\n" );

	switch ( m_nResultMode ) {
		case rmCharacter:
			m_strResultInput = m_strComposeResult;
			break;

		case rmHexCodePoint:
		case rmDecCodePoint: {
			m_strResultInput.Empty( );

			int limit = m_CompositeCharacter.GetUtf32Length( );
			debug( L"CComposeSequenceEditor::_SetInputFromResult: hex/dec(%d), limit=%d\n", m_nResultMode, limit );
			if ( !limit ) {
				break;
			}

			wchar_t const* pwzFormatFirst = ( rmHexCodePoint == m_nResultMode ) ? L"%X"   : L"%u";
			wchar_t const* pwzFormatRest  = ( rmHexCodePoint == m_nResultMode ) ? L", %X" : L", %u";

			UChar32 const* pqzCodePoints = m_CompositeCharacter.GetUtf32( );
			debug( L"+ pqzCodePoints[0]: %u\n", pqzCodePoints[0] );
			m_strResultInput.Format( pwzFormatFirst, pqzCodePoints[0] );
			for ( int index = 1; index < limit; index++ ) {
				debug( L"+ pqzCodePoints[%d]: %u\n", index, pqzCodePoints[index] );
				m_strResultInput.AppendFormat( pwzFormatRest, pqzCodePoints[index] );
			}
			break;
		}

		default:
			debug( L"CComposeSequenceEditor::_SetInputFromResult: unknown result mode %d\n", m_nResultMode );
			return;
	}

	m_editComposeResult.SetWindowText( m_strResultInput );
	m_staticPreview.Invalidate( TRUE );
}

void CComposeSequenceEditor::_DDV_MinMaxCompositeCharacters( CDataExchange* pDX, unsigned /*uID*/, CString& strInput, int const cchMin, int const cchMax ) {
	debug( L"_DDV_MinMaxCompositeCharacters: pDX=0x%p (m_bSaveAndValidate?%s), strInput='%s' (length: %d), cchMin=%d, cchMax=%d\n", pDX, pDX->m_bSaveAndValidate ? L"TRUE" : L"false", strInput, strInput.GetLength( ), cchMin, cchMax );

	if ( !pDX->m_bSaveAndValidate ) {
		debug( L"+ Wrong mode, bailing\n" );
		return;
	}

	if ( strInput.IsEmpty( ) ) {
		debug( L"+ strInput is empty\n" );
		pDX->Fail( );
		return;
	}

	int cchComposeSequence = CountCompositeCharacters( strInput, strInput.GetLength( ) );
	debug( L"+ cchComposeSequence=%d\n", cchComposeSequence );
	if ( ( cchMin > -1 ) && ( cchComposeSequence < cchMin ) ) {
		debug( L"+ strInput is too short\n" );
		pDX->Fail( );
		return;
	}
	if ( ( cchMax > -1 ) && ( cchComposeSequence > cchMax ) ) {
		debug( L"+ strInput is too long\n" );
		pDX->Fail( );
		return;
	}
}

void CComposeSequenceEditor::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange( pDX );

	DDX_Control ( pDX, IDC_CSE_SEQUENCE,            m_editComposeSequence       );
	DDX_Control ( pDX, IDC_CSE_RESULT,              m_editComposeResult         );
	DDX_Control ( pDX, IDC_CSE_PREVIEW,             m_staticPreview             );
	DDX_Control ( pDX, IDOK_ADDANOTHER,             m_buttonAddAnother          );

	DDX_Text    ( pDX, IDC_CSE_SEQUENCE,            m_strComposeSequence        );
	DDX_Text    ( pDX, IDC_CSE_RESULT,              m_strResultInput            );
	DDX_Radio   ( pDX, IDC_CSE_RESULT_AS_CHARACTER, m_nResultMode               );
	DDX_Check   ( pDX, IDC_CSE_ENABLED,             m_fEnabled                  );
	DDX_Check   ( pDX, IDC_CSE_CASE_INSENSITIVE,    m_fCaseInsensitive          );
	DDX_Check   ( pDX, IDC_CSE_REVERSIBLE,          m_fReversible               );

	_DDV_MinMaxCompositeCharacters( pDX, IDC_CSE_SEQUENCE, m_strComposeSequence, 2, 16 );
	_DDV_MinMaxCompositeCharacters( pDX, IDC_CSE_RESULT,   m_strComposeResult,   1, 1 );
}

BOOL CComposeSequenceEditor::OnInitDialog( ) {
	bool fAddMode = ( semAdd == m_mode );
	SetWindowText( LoadFromStringTable( fAddMode ? IDC_CSE_ADD_MODE_TITLE : IDC_CSE_EDIT_MODE_TITLE ) );

	m_ComposeSequences.RemoveAll( );
	m_strComposeSequence =  m_sequence.Sequence;
	m_strComposeResult   =  m_sequence.Result;
	m_strResultInput     =  m_sequence.Result;
	m_fEnabled           = !m_sequence.Disabled;
	m_fCaseInsensitive   =  m_sequence.CaseInsensitive;
	m_fReversible        =  m_sequence.Reversible;
	m_nResultMode        = ( -1 == nLastRadioGroupSelection ) ? 0 : nLastRadioGroupSelection;

	m_CompositeCharacter.SetContents( m_strComposeResult );

	if ( !CDialog::OnInitDialog( ) ) {
		return FALSE;
	}

	m_buttonAddAnother.ShowWindow( fAddMode ? SW_SHOW : SW_HIDE );

	m_editComposeSequence.SetFocus( );

	return FALSE;
}

void CComposeSequenceEditor::OnOK( ) {
	if ( !UpdateData( TRUE ) ) {
		debug( L"CComposeSequenceEditor::OnOK: UpdateData(TRUE) failed\n" );
		return;
	}

	if ( m_fModified ) {
		m_sequence.Sequence        =   m_strComposeSequence;
		m_sequence.Result          =   m_strComposeResult;
		m_sequence.Disabled        =  !m_fEnabled;
		m_sequence.CaseInsensitive = !!m_fCaseInsensitive;
		m_sequence.Reversible      = !!m_fReversible;

		m_ComposeSequences.Add( m_sequence );
	}

	CDialog::OnOK( );
}

void CComposeSequenceEditor::OnOKAddAnother( ) {
	if ( !UpdateData( TRUE ) ) {
		debug( L"CComposeSequenceEditor::OnOK: UpdateData(TRUE) failed\n" );
		return;
	}

	m_sequence.Sequence        =   m_strComposeSequence;
	m_sequence.Result          =   m_strComposeResult;
	m_sequence.Disabled        =  !m_fEnabled;
	m_sequence.CaseInsensitive = !!m_fCaseInsensitive;
	m_sequence.Reversible      = !!m_fReversible;

	m_ComposeSequences.Add( m_sequence );

	m_strComposeSequence.Empty( );
	m_strComposeResult.Empty( );
	m_strResultInput.Empty( );
	m_fEnabled = TRUE;
	m_fCaseInsensitive = FALSE;
	m_fReversible = FALSE;

	UpdateData( FALSE );
	m_fModified = false;

	m_editComposeSequence.SetFocus( );
}

void CComposeSequenceEditor::OnDrawItem( int nID, LPDRAWITEMSTRUCT lpDrawItemStruct ) {
	if ( IDC_CSE_PREVIEW != nID ) {
		CDialog::OnDrawItem( nID, lpDrawItemStruct );
		return;
	}

	CRect rcItem( lpDrawItemStruct->rcItem );
	CDC dc;
	dc.Attach( lpDrawItemStruct->hDC );

	if ( !m_pFont ) {
#pragma push_macro( "new" )
#undef new
		m_pFont = new CFont( );
#pragma pop_macro( "new" )
		m_pFont->CreateFont( rcItem.Height( ), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, L"MS Shell Dlg 2" );
	}
	
	COLORREF bkColor = dc.GetBkColor( );
	dc.FillSolidRect( rcItem, GetSysColor( COLOR_3DFACE ) );
	dc.SetBkColor( bkColor );

	CFont* pOldFont = dc.SelectObject( m_pFont );
	dc.DrawText( m_CompositeCharacter.GetLPCWSTR( ), -1, rcItem, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE );
	dc.SelectObject( pOldFont );

	dc.Detach( );
}

void CComposeSequenceEditor::OnUpdateComposeSequence( ) {
	m_fModified = true;
}

void CComposeSequenceEditor::OnUpdateComposeResult( ) {
	_SetResultFromInput( );
	m_fModified = true;
}

void CComposeSequenceEditor::OnResultModeClicked( UINT uID ) {
	m_nResultMode = nLastRadioGroupSelection = uID - IDC_CSE_RESULT_AS_CHARACTER;
	_SetInputFromResult( );
}

void CComposeSequenceEditor::OnCheckboxClicked( UINT /*uID*/ ) {
	m_fModified = true;
}
