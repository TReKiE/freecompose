#pragma once

//==============================================================================
// Classes
//==============================================================================

class CFcCommandLineInfo: public CCommandLineInfo {

public:
	inline virtual void ParseParam( wchar_t const* pwzParam, BOOL bFlag, BOOL /*bLast*/ ) {
		if ( bFlag ) {
			CString param( pwzParam );

			if ( 0 == param.CompareNoCase( L"forceDefConf" ) ) {
				g_ConfigurationSource = ConfigurationSource::Default;
			} else if ( 0 == param.CompareNoCase( L"forceRegConf" ) ) {
				g_ConfigurationSource = ConfigurationSource::Registry;
			} else if ( 0 == param.CompareNoCase( L"showOptions" ) ) {
				g_ShowOptionsDialogImmediately = true;
			} else {
				debug( L"CFcCommandLineInfo::ParseParam: Unknown parameter '/%s'\n", pwzParam );
			}
		}
	}

};
