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

	ON_EN_UPDATE( IDC_CSE_SEQUENCE, &CComposeSequenceEditor::OnUpdateComposeSequence )
	ON_EN_UPDATE( IDC_CSE_RESULT,   &CComposeSequenceEditor::OnUpdateComposeResult   )
END_MESSAGE_MAP( )

CComposeSequenceEditor::CComposeSequenceEditor( ComposeSequence& sequence, SEQUENCE_EDITOR_MODE mode, CWnd* pParent ):
	CDialog    ( IDD, pParent ),
	m_sequence ( sequence ),
	m_mode     ( mode ),
	m_strTitle ( LoadFromStringTable( ( semAdd == mode ) ? IDC_CSE_ADD_MODE_TITLE : IDC_CSE_EDIT_MODE_TITLE ) ),
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

	for ( index = 0, ch = pqz[index]; 0 != ch; index++ ) {
		unsigned mask = U_GET_GC_MASK( pqz[index] );
		if ( 0 != ( mask & GcAlnumMask ) ) {
			converting = true;
			int digit = u_digit( ch, static_cast<int8_t>( base ) );
			if ( -1 == digit ) {
				debug( L"CComposeSequenceEditor::_ParseCodePointList: Unacceptable alphanumeric value &#%d;\n", ch );
				return false;
			}
			current = current * base + digit;
			if ( current > UCHAR_MAX_VALUE ) {
				debug( L"CComposeSequenceEditor::_ParseCodePointList: Code point value overflow, 0x%X\n", ch );
				return false;
			}
		} else if ( 0 != ( mask & GcDelimMask ) ) {
			if ( converting ) {
				output.Add( current );
				current = 0;
				converting = false;
			}
		} else {
			debug( L"CComposeSequenceEditor::_ParseCodePointList: Unacceptable character &#%d;\n", ch );
		}

		index++;
	}
	delete[] pqz;

	return true;
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
	DDX_Text    ( pDX, IDC_CSE_RESULT,              m_strComposeResult          );
	DDX_Radio   ( pDX, IDC_CSE_RESULT_AS_CHARACTER, m_nResultMode               );
	DDX_Check   ( pDX, IDC_CSE_ENABLED,             m_fEnabled                  );
	DDX_Check   ( pDX, IDC_CSE_CASE_INSENSITIVE,    m_fCaseInsensitive          );
	DDX_Check   ( pDX, IDC_CSE_REVERSIBLE,          m_fReversible               );
}

bool CComposeSequenceEditor::_InterpretComposeResult( void ) {
	if ( rmCharacter == m_nResultMode ) {
		// nothing to do here
	} else if ( rmDecCodePoint == m_nResultMode || rmHexCodePoint == m_nResultMode ) {
		CArray<UChar32> codePoints;
		bool fResult = _ParseCodePointList( m_strComposeResult, ( rmHexCodePoint == m_nResultMode ) ? 16 : 10, codePoints );
		if ( !fResult ) {
			return false;
		}
		m_strComposeResult = Utf32ToUtf16( codePoints.GetData( ) );
	} else {
		return false;
	}

	if ( !m_CompositeCharacter.SetContents( m_strComposeResult ) ) {
		return false;
	}

	m_staticPreview.Invalidate( );

	return true;
}

BOOL CComposeSequenceEditor::OnInitDialog( ) {
	m_strComposeSequence =  m_sequence.Sequence;
	m_strComposeResult   =  m_sequence.Result;
	m_nResultMode        = ( -1 == nLastRadioGroupSelection ) ? 0 : nLastRadioGroupSelection;
	m_fEnabled           = !m_sequence.Disabled;
	m_fCaseInsensitive   =  m_sequence.CaseInsensitive;
	m_fReversible        =  m_sequence.Reversible;

	SetWindowText( m_strTitle );

	CString junk( FormatCodePoint( CString( L"e\u0301" ) ) );

	if ( !CDialog::OnInitDialog( ) ) {
		return FALSE;
	}

	return TRUE;
}

void CComposeSequenceEditor::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) {
	if ( IDC_CSE_PREVIEW != nIDCtl ) {
		CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct );
		return;
	}

	CRect rcItem( lpDrawItemStruct->rcItem );
	CDC dc;
	dc.Attach( lpDrawItemStruct->hDC );
	if ( !m_pFont ) {
		m_pFont = new CFont( );
		m_pFont->CreateFont( rcItem.Height( ), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, L"MS Shell Dlg2" );
	}

	CFont* pOldFont = dc.SelectObject( m_pFont );
	dc.DrawText( m_CompositeCharacter.GetPCWSTR( ), -1, rcItem, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE );
	dc.SelectObject( pOldFont );
	dc.Detach( );
}

void CComposeSequenceEditor::OnUpdateComposeSequence( ) {
	// TODO allow only two composite characters
}

void CComposeSequenceEditor::OnUpdateComposeResult( ) {
	m_editComposeResult.GetWindowText( m_strComposeResult );
	_InterpretComposeResult( );
}
