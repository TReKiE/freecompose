#pragma once

#include <AutoCriticalSection.h>
#include <AutoEvent.h>

#include "AppSoundsRegistry.h"

class SoundPlayer {

public:
	SoundPlayer( );
	~SoundPlayer( );

	void PlaySoundForEvent( Pip soundEvent );
	void ShutDown( void );

};

extern SoundPlayer* g_pSoundPlayer;
