#pragma once

class CAppSoundsRegistry {
public:

	static void RegisterFcAppSounds( void );
	static void UnregisterFcAppSounds( void );

private:

	static void _RegisterEventLabels( wchar_t const* pwzExeName );
	static void _RegisterApp( wchar_t const* pwzExeName );

	static void _UnregisterEventLabels( void );
	static void _UnregisterApp( void );

};
