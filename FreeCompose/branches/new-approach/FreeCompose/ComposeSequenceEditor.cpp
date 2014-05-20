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

enum ResultMode {
	rmCharacter,
	rmHexCodePoint,
	rmDecCodePoint,
};

int static nLastRadioGroupSelection = -1;

IMPLEMENT_DYNAMIC( CComposeSequenceEditor, CDialog )

BEGIN_MESSAGE_MAP( CComposeSequenceEditor, CDialog )
	ON_WM_DRAWITEM( )

	ON_BN_CLICKED( IDOK,             &CComposeSequenceEditor::OnOK                    )
	ON_EN_UPDATE ( IDC_CSE_SEQUENCE, &CComposeSequenceEditor::OnUpdateComposeSequence )
	ON_EN_UPDATE ( IDC_CSE_RESULT,   &CComposeSequenceEditor::OnUpdateComposeResult   )

	ON_CONTROL_RANGE( BN_CLICKED, IDC_CSE_RESULT_AS_CHARACTER, IDC_CSE_REVERSIBLE, &CComposeSequenceEditor::OnResultModeClicked )
END_MESSAGE_MAP( )

CComposeSequenceEditor::CComposeSequenceEditor( ComposeSequence& sequence, SEQUENCE_EDITOR_MODE mode, CWnd* pParent ):
	CDialog    ( IDD, pParent ),
	m_sequence ( sequence ),
	m_mode     ( mode ),
	m_pFont    ( nullptr )
{

}

CComposeSequenceEditor::~CComposeSequenceEditor( ) {
	if ( m_pFont ) {
		delete m_pFont;
		m_pFont = nullptr;
	}
}

bool CComposeSequenceEditor::_ParseCodePointList( CString const& input, int const base, CArray<UChar32>& output ) {
	int const GcDelimMask = U_GC_P_MASK  | U_GC_S_MASK  | U_GC_ZS_MASK;
	int const GcAlnumMask = U_GC_LC_MASK | U_GC_ND_MASK;

	UChar32* pqz = Utf16ToUtf32( input, input.GetLength( ) );
	output.RemoveAll( );

	UChar32 current = 0;
	int index = 0;
	bool converting = false;

	CArray<UChar32> elements;
	UChar32 ch;
	bool fRet = false;

	for ( index = 0, ch = pqz[index]; 0 != ch; index++, ch = pqz[index] ) {
		unsigned mask = U_GET_GC_MASK( pqz[index] );
		if ( 0 != ( mask & GcAlnumMask ) ) {
			converting = true;

			int digit = u_digit( ch, static_cast<int8_t>( base ) );
			if ( -1 == digit ) {
				debug( L"CComposeSequenceEditor::_ParseCodePointList: Unacceptable alphanumeric value &#%d;\n", ch );
				goto out;
			}

			current = current * base + digit;
			if ( current > UCHAR_MAX_VALUE ) {
				debug( L"CComposeSequenceEditor::_ParseCodePointList: Code point value overflow, 0x%X\n", ch );
				goto out;
			}
		} else if ( 0 != ( mask & GcDelimMask ) ) {
			if ( converting ) {
				output.Add( current );
				current = 0;
				converting = false;
			}
		} else {
			debug( L"CComposeSequenceEditor::_ParseCodePointList: Unacceptable character &#%d;\n", ch );
			goto out;
		}
	}
	if ( converting ) {
		output.Add( current );
	}
	fRet = true;

out:
	delete[] pqz;
	return fRet;
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
			wchar_t const* pszFormatString = ( rmHexCodePoint == m_nResultMode ) ? L"%s%X" : L"%s%d";
			int limit = m_strComposeResult.GetLength( );
			UChar32* pqzCodePoints = Utf16ToUtf32( m_strComposeResult, limit );
			for ( int index = 0; index < limit; index++ ) {
				m_strResultInput.AppendFormat( pszFormatString, ( index > 0 ) ? L", " : L"", pqzCodePoints[index] );
			}
			delete[] pqzCodePoints;
			break;
		}

		default:
			debug( L"CComposeSequenceEditor::_SetInputFromResult: unknown result mode %d\n", m_nResultMode );
			return;
	}

	m_editComposeResult.SetWindowText( m_strResultInput );
	m_staticPreview.Invalidate( TRUE );
}

void CComposeSequenceEditor::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange( pDX );

	DDX_Control ( pDX, IDC_CSE_SEQUENCE,            m_editComposeSequence       );
	DDX_Control ( pDX, IDC_CSE_RESULT,              m_editComposeResult         );
	DDX_Control ( pDX, IDC_CSE_RESULT_AS_CHARACTER, m_radioResultAsCharacter    );
	DDX_Control ( pDX, IDC_CSE_RESULT_AS_HEX,       m_radioResultAsHexCodePoint );
	DDX_Control ( pDX, IDC_CSE_RESULT_AS_DECIMAL,   m_radioResultAsDecCodePoint );
	DDX_Control ( pDX, IDC_CSE_PREVIEW,             m_staticPreview             );
	DDX_Control ( pDX, IDC_CSE_ENABLED,             m_checkEnabled              );
	DDX_Control ( pDX, IDC_CSE_CASE_INSENSITIVE,    m_checkCaseInsensitive      );
	DDX_Control ( pDX, IDC_CSE_REVERSIBLE,          m_checkReversible           );

	DDX_Text    ( pDX, IDC_CSE_SEQUENCE,            m_strComposeSequence        );
	DDX_Text    ( pDX, IDC_CSE_RESULT,              m_strResultInput            );
	DDX_Radio   ( pDX, IDC_CSE_RESULT_AS_CHARACTER, m_nResultMode               );
	DDX_Check   ( pDX, IDC_CSE_ENABLED,             m_fEnabled                  );
	DDX_Check   ( pDX, IDC_CSE_CASE_INSENSITIVE,    m_fCaseInsensitive          );
	DDX_Check   ( pDX, IDC_CSE_REVERSIBLE,          m_fReversible               );

	// TODO: DDV. =(
}

BOOL CComposeSequenceEditor::OnInitDialog( ) {
	SetWindowText( LoadFromStringTable( ( semAdd == m_mode ) ? IDC_CSE_ADD_MODE_TITLE : IDC_CSE_EDIT_MODE_TITLE ) );

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

	return TRUE;
}

void CComposeSequenceEditor::OnOK( ) {
	UpdateData( TRUE );

	m_sequence.Sequence        =   m_strComposeSequence;
	m_sequence.Result          =   m_strComposeResult;
	m_sequence.Disabled        =  !m_fEnabled;
	m_sequence.CaseInsensitive = !!m_fCaseInsensitive;
	m_sequence.Reversible      = !!m_fReversible;

	CDialog::OnOK( );
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
		m_pFont = new CFont( );
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
	// TODO allow only two composite characters
}

void CComposeSequenceEditor::OnUpdateComposeResult( ) {
	// TODO allow only one composite character
	_SetResultFromInput( );
}

void CComposeSequenceEditor::OnResultModeClicked( UINT uID ) {
	m_nResultMode = nLastRadioGroupSelection = uID - IDC_CSE_RESULT_AS_CHARACTER;
	_SetInputFromResult( );
}
