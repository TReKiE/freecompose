#include "stdafx.h"

#include "FreeCompose.h"
#include "EditKeySequence.h"

#include "Utils.h"

IMPLEMENT_DYNAMIC(CEditKeySequence, CDialog)

BEGIN_MESSAGE_MAP(CEditKeySequence, CDialog)
END_MESSAGE_MAP()

CEditKeySequence::CEditKeySequence( COMPOSE_KEY_ENTRY& cke, bool fAddMode, CWnd* pParent ):
	CDialog       ( CEditKeySequence::IDD, pParent ),
	m_cke         ( cke ),
	m_fAdd        ( fAddMode )
{
}

CEditKeySequence::~CEditKeySequence( ) {
}

void CEditKeySequence::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange( pDX );
	DDX_Control ( pDX, IDC_FIRSTKEY,  m_editFirstKey    );
	DDX_Control ( pDX, IDC_SECONDKEY, m_editSecondKey   );
	DDX_Control ( pDX, IDC_RESULT,    m_editResult      );
	DDX_Key     ( pDX, IDC_FIRSTKEY,  m_cke.vkFirst     );
	DDX_Key     ( pDX, IDC_SECONDKEY, m_cke.vkSecond    );
	DDX_Char    ( pDX, IDC_RESULT,    m_cke.wchComposed );
}

void CEditKeySequence::DDX_Key( CDataExchange* pDX, int nIDC, DWORD& dwVk ) {
	if ( IDC_FIRSTKEY == nIDC ) {
		if ( pDX->m_bSaveAndValidate ) {
			dwVk = m_editFirstKey.GetKey( );
		} else {
			m_editFirstKey.SetKey( dwVk );
		}
	} else if ( IDC_SECONDKEY == nIDC ) {
		if ( pDX->m_bSaveAndValidate ) {
			dwVk = m_editSecondKey.GetKey( );
		} else {
			m_editSecondKey.SetKey( dwVk );
		}
	} else {
		dwVk = 0;
	}
}

void CEditKeySequence::DDX_Char( CDataExchange* pDX, int nIDC, wchar_t& wch ) {
	if ( IDC_RESULT != nIDC ) {
		wch = 0;
		return;
	}

	if ( pDX->m_bSaveAndValidate ) {
		CString str;
		m_editResult.GetWindowText( str );
		if ( str.GetLength() > 0 ) {
			wch = str[0];
		} else {
			wch = 0;
		}
	} else {
		CString str;
		if ( 0 != wch )
			str = wch;
		m_editResult.SetWindowText( str );
	}
}

BOOL CEditKeySequence::OnInitDialog( ) {
	if ( ! CDialog::OnInitDialog( ) )
		return FALSE;

	if ( m_fAdd )
		this->SetWindowText( _T("Add key sequence") );

	m_editResult.SetLimitText( 1 );
	UpdateData( FALSE );

	return TRUE;
}
