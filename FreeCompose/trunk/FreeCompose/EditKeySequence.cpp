#include "stdafx.h"

#include "FreeCompose.h"
#include "EditKeySequence.h"

#include "Utils.h"

IMPLEMENT_DYNAMIC(CEditKeySequence, CDialog)

BEGIN_MESSAGE_MAP(CEditKeySequence, CDialog)
END_MESSAGE_MAP()

CEditKeySequence::CEditKeySequence( COMPOSE_KEY_ENTRY& cke, bool fAddMode, CWnd* pParent ):
	CDialog ( CEditKeySequence::IDD, pParent ),
	m_cke   ( cke                            ),
	m_fAdd  ( fAddMode                       )
{
}

CEditKeySequence::~CEditKeySequence( ) {
}

void CEditKeySequence::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_FIRSTKEY,  m_editFirstKey  );
	DDX_Control( pDX, IDC_SECONDKEY, m_editSecondKey );
	DDX_Control( pDX, IDC_RESULT,    m_editResult    );
}

BOOL CEditKeySequence::OnInitDialog( ) {
	if ( ! CDialog::OnInitDialog( ) )
		return FALSE;

	if ( m_fAdd )
		this->SetWindowText( _T("Add key sequence") );

	if ( 0 != m_cke.vkFirst && 0 != m_cke.vkSecond ) {
		m_editFirstKey.SetWindowText( VkToString( m_cke.vkFirst ) );
		m_editSecondKey.SetWindowText( VkToString( m_cke.vkSecond ) );
		m_editResult.SetWindowText( CString( m_cke.wchComposed ) );
	}

	return TRUE;
}
