#pragma once

class CFcAppSoundsRegistry {

public:
	static void Register( void );
	static void Unregister( void );

private:
	static CString _GetCurrentSoundScheme( void );

	static void _RegisterEventLabels( wchar_t const* pwzExeName );
	static void _RegisterApp( wchar_t const* pwzExeName );

	static void _UnregisterEventLabels( void );
	static void _UnregisterApp( void );

};

int const NumberOfApplicationSounds = static_cast<int>( ApplicationSound::max );

extern wchar_t const* ApplicationSoundNames[NumberOfApplicationSounds];
extern int const ApplicationSoundDisplayNameIds[NumberOfApplicationSounds];
