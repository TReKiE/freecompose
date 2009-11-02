#include "stdafx.h"

#include "FreeCompose.h"
#include "EditKeySequence.h"

IMPLEMENT_DYNAMIC(CEditKeySequence, CDialog)

BEGIN_MESSAGE_MAP(CEditKeySequence, CDialog)
END_MESSAGE_MAP()

CEditKeySequence::CEditKeySequence( CWnd* pParent ):
	CDialog( CEditKeySequence::IDD, pParent )
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
