#pragma once

#include <AutoCriticalSection.h>
#include <AutoEvent.h>

class SoundPlayer {

public:
	SoundPlayer( );
	~SoundPlayer( );

	void PlaySoundForEvent( CompositionSound soundEvent );
	void ShutDown( void );

};

extern SoundPlayer* g_pSoundPlayer;
