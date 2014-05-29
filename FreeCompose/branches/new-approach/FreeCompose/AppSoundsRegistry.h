#pragma once

enum class CompositionSound: int {
	none = -1,
	Starting = 0,
	KeyPressed,
	Succeeded,
	Failed,
	Cancelled,
	Escape,
};

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

extern wchar_t const* CompositionSoundNames[];
