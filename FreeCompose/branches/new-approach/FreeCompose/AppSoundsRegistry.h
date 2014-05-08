#pragma once

class CAppSoundsRegistry {
public:

	static void RegisterFcAppSounds( void );

private:

	static void _RegisterEventLabels( wchar_t const* pwzExeName );
	static void _RegisterApp( wchar_t const* pwzExeName );

};
