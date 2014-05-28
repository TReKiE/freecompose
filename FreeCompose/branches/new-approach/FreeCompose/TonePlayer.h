#pragma once

#include <AutoCriticalSection.h>
#include <AutoEvent.h>

class TonePlayer {

public:
	inline TonePlayer( ) {
		_CheckStartThread( );
	}

	inline ~TonePlayer( ) {
		_StopThread( );
	}

	void PlayTone( DWORD const dwFrequency, DWORD const dwDuration ) {
		_Enqueue( dwFrequency, dwDuration );
	}

	void PlaySilence( DWORD const dwDuration ) {
		_Enqueue( 0, dwDuration );
	}

	void CancelPendingTones( void ) {
		_ClearQueue( );
	}

	void ShutDown( void ) {
		_StopThread( );
	}

protected:
	void _Enqueue( DWORD const dwFrequency, DWORD const dwDuration );
	void _ClearQueue( void );

	void _CheckStartThread( void );
	void _StopThread( void );

};

extern TonePlayer* g_pTonePlayer;
