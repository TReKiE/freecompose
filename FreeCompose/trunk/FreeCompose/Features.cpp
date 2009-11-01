// Features.cpp : implementation file

#include "stdafx.h"

#include "FreeCompose.h"
#include "Features.h"

#include "Utils.h"

IMPLEMENT_DYNAMIC( CFeatures, CPropertyPage )

BEGIN_MESSAGE_MAP( CFeatures, CPropertyPage )
	ON_CONTROL_RANGE( BN_CLICKED, IDC_ENABLEONSTARTUP, IDC_SWAPCTRLANDCAPS, OnCheckboxChanged )
END_MESSAGE_MAP( )

CFeatures::CFeatures( COptionsData& Options ):
	CPropertyPage ( CFeatures::IDD ),
	m_Options     ( Options        )
{
}

CFeatures::~CFeatures( ) {
}

void CFeatures::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );
	DDX_Check( pDX, IDC_STARTWITHWINDOWS, m_Options.m_fStartWithWindows );
	DDX_Check( pDX, IDC_ENABLEONSTARTUP,  m_Options.m_fStartActive      );
	DDX_Check( pDX, IDC_SWAPCTRLANDCAPS,  m_Options.m_fSwapCtrlAndCaps  );
	DDX_Check( pDX, IDC_DISABLECAPSLOCK,  m_Options.m_fDisableCapsLock  );
}

BOOL CFeatures::OnInitDialog( ) {
	UpdateData( FALSE );
	return CPropertyPage::OnInitDialog( );
}

void CFeatures::OnCheckboxChanged( UINT /*uID*/ ) {
	SetModified( );
}
