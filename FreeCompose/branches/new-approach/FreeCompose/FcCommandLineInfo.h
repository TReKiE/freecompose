#pragma once

class CFcCommandLineInfo: public CCommandLineInfo {

public:
	CFcCommandLineInfo( ):
		CCommandLineInfo              ( ),
		m_bForceDefaultConfiguration  ( FALSE ),
		m_bForceRegistryConfiguration ( FALSE )
	{
		
	}

	inline virtual void ParseParam( wchar_t const* pwzParam, BOOL bFlag, BOOL /*bLast*/ ) {
		if ( bFlag ) {
			CString param( pwzParam );

			if ( 0 == param.CompareNoCase( L"forceDefConf" ) ) {
				m_bForceDefaultConfiguration = TRUE;
			} else if ( 0 == param.CompareNoCase( L"forceRegConf" ) ) {
				m_bForceRegistryConfiguration = TRUE;
			} else {
				debug( L"CFcCommandLineInfo::ParseParam: Unknown parameter '/%s'\n", pwzParam );
			}
		}
	}

	BOOL m_bForceDefaultConfiguration;
	BOOL m_bForceRegistryConfiguration;

};
