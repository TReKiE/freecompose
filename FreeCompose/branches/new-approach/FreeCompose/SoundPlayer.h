#pragma once

#include <AutoCriticalSection.h>
#include <AutoEvent.h>

#include "AppSoundsRegistry.h"

class SoundPlayer {

public:
	inline SoundPlayer( ) {
		_CheckStartThread( );
	}

	inline ~SoundPlayer( ) {
		_StopThread( );
	}

	void PlayCompositionSound( CompositionSound const sound ) {
		_EnqueueCompositionSound( sound );
	}

	void PlaySilence( DWORD const dwDuration ) {
		_EnqueueSilence( dwDuration );
	}

	void PlayTone( DWORD const dwFrequency, DWORD const dwDuration ) {
		_EnqueueTone( dwFrequency, dwDuration );
	}

	void CancelPending( void ) {
		_ClearQueue( );
	}

	void ShutDown( void ) {
		_StopThread( );
	}

protected:
	void _EnqueueCompositionSound( CompositionSound const sound );
	void _EnqueueSilence( DWORD const dwDuration );
	void _EnqueueTone( DWORD const dwFrequency, DWORD const dwDuration );
	void _ClearQueue( void );

	void _CheckStartThread( void );
	void _StopThread( void );

};

extern SoundPlayer* g_pSoundPlayer;
