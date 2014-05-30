#pragma once

#include <AutoCriticalSection.h>
#include <AutoEvent.h>

#include "AppSoundsRegistry.h"

class SoundPlayer {

public:
	inline SoundPlayer( ) { }
	inline ~SoundPlayer( ) { }

	inline void ShutDown( void ) {
		
	}
	
};

extern SoundPlayer* g_pSoundPlayer;
