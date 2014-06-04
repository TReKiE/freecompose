#pragma once

class CFcAppSoundsRegistry {

public:
	static void Register( void );
	static void Unregister( void );

	static CString GetCurrentSoundScheme( void );

	static CString GetEventFileName( CString const& strScheme, CString const& strEvent );
	static bool SetEventFileName( CString const& strScheme, CString const& strEvent, CString const& strNewFileName );

private:
	static void _RegisterEventLabels( wchar_t const* pwzExeName );
	static void _RegisterApp( wchar_t const* pwzExeName );

	static void _UnregisterEventLabels( void );
	static void _UnregisterApp( void );

};

int const NumberOfApplicationSounds = static_cast<int>( ApplicationSound::max );

extern wchar_t const* ApplicationSoundNames[NumberOfApplicationSounds];
extern int const ApplicationSoundDisplayNameIds[NumberOfApplicationSounds];
