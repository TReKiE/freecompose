#pragma once

class CAppSoundsRegistry {

public:
	static void RegisterFcAppSounds( void );
	static void UnregisterFcAppSounds( void );

private:
	static CString _GetCurrentSoundScheme( void );

	static void _RegisterEventLabels( wchar_t const* pwzExeName );
	static void _RegisterApp( wchar_t const* pwzExeName );

	static void _UnregisterEventLabels( void );
	static void _UnregisterApp( void );

};

extern wchar_t const* CompositionSoundNames[];
