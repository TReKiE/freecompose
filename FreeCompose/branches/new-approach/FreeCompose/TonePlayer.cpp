#include "stdafx.h"

#include "TonePlayer.h"

//
// Data types
//

namespace {

	class Request {

	public:
		inline Request( ) { }
		inline virtual ~Request( ) { }

		virtual void Perform( void ) = 0;

	};

	class CompositionSoundRequest: public Request {

	public:
		inline CompositionSoundRequest( ): Sound( CompositionSound::none ) { }
		inline CompositionSoundRequest( CompositionSound const sound ): Sound( sound ) { }
		inline virtual ~CompositionSoundRequest( ) { }

		inline virtual void Perform( void ) {
			PlaySound( CompositionSoundNames[static_cast<int>( Sound )], nullptr, SND_ALIAS | SND_APPLICATION | SND_SYNC );
		}

		CompositionSound Sound;

	};

	class SilenceRequest: public Request {
		
	public:
		inline SilenceRequest( ): dwDuration( 0 ) { }
		inline SilenceRequest( DWORD const duration ): dwDuration( duration ) { }
		inline virtual ~SilenceRequest( ) { }

		inline virtual void Perform( void ) {
			Sleep( dwDuration );
		}

		DWORD dwDuration;

	};

	class ToneRequest: public Request {

	public:
		inline ToneRequest( ): dwFrequency( 0 ), dwDuration( 0 ) { }
		inline ToneRequest( DWORD const frequency, DWORD const duration ): dwFrequency( frequency ), dwDuration( duration ) { }
		inline virtual ~ToneRequest( ) { }

		inline virtual void Perform( void ) {
			Beep( dwFrequency, dwDuration );
		}

		DWORD dwFrequency;
		DWORD dwDuration;

	};

};

//
// Local variables
//

static CArray<Request*> Queue;
static AutoEvent QueueEvent;
static AutoCriticalSection QueueLock;

static CWinThread* ThreadObject = nullptr;
static AutoCriticalSection ThreadLock;

static AutoEvent ShutdownEvent;

//
// Local functions
//

static void Enqueue( Request* pRequest ) {
	LOCK( QueueLock ) {
		Queue.Add( pRequest );
		QueueEvent.Set( );
	} UNLOCK( QueueLock );
}

static Request* Dequeue( void ) {
	Request* p = nullptr;
	LOCK( QueueLock ) {
		if ( Queue.GetCount( ) > 0 ) {
			p = Queue[0];
			Queue.RemoveAt( 0, 1 );
		}
	} UNLOCK( QueueLock );
	return p;
}

static void DrainQueue( void ) {
	Request* pReq;
	while ( nullptr != ( pReq = Dequeue( ) ) ) {
		pReq->Perform( );
	}
}

static UINT TonePlayerThreadFunction( LPVOID /*pvParam*/ ) {
	HANDLE waitHandles[2] = { QueueEvent, ShutdownEvent };

	while ( true ) {
		SetLastError( ERROR_SUCCESS );
		DWORD dw = WaitForMultipleObjectsEx( 2, waitHandles, false, INFINITE, true );
		if ( WAIT_OBJECT_0 == dw ) {
			DrainQueue( );
		} else if ( WAIT_OBJECT_0 + 1 == dw ) {
			return 0;
		} else {
			debug( L"TonePlayerThreadFunction: Something went wrong with WaitForMultipleObjects. dw=0x%08X, error=%lu\n", dw, GetLastError( ) );
			return 1;
		}
	}
	
	return 0;
}

//
// Class TonePlayer
//

void TonePlayer::_EnqueueCompositionSound( CompositionSound const sound ) {
	Enqueue( new CompositionSoundRequest( sound ) );
}

void TonePlayer::_EnqueueSilence( DWORD const dwDuration ) {
	Enqueue( new SilenceRequest( dwDuration ) );
}

void TonePlayer::_EnqueueTone( DWORD const dwFrequency, DWORD const dwDuration ) {
	Enqueue( new ToneRequest( dwFrequency, dwDuration ) );
}

void TonePlayer::_ClearQueue( void ) {
	LOCK( QueueLock ) {
		Queue.RemoveAll( );
		QueueEvent.Reset( );
	} UNLOCK( QueueLock );
}

void TonePlayer::_CheckStartThread( void ) {
	LOCK( ThreadLock ) {
		if ( ThreadObject ) {
			break;
		}
		
		ThreadObject = AfxBeginThread( TonePlayerThreadFunction, nullptr );
		ThreadObject->m_bAutoDelete = FALSE;
	} UNLOCK( ThreadLock );
}

void TonePlayer::_StopThread( void ) {
	HANDLE hThread = INVALID_HANDLE_VALUE;	
	LOCK( ThreadLock ) {
		if ( !ThreadObject ) {
			break;
		}
		hThread = ThreadObject->m_hThread;
	} UNLOCK( ThreadLock );
	if ( INVALID_HANDLE_VALUE == hThread ) {
		return;
	}

	ShutdownEvent.Set( );

	SetLastError( ERROR_SUCCESS );
	DWORD dw = WaitForSingleObject( hThread, INFINITE );
	// WAIT_ABANDONED only applies to mutexes.
	if ( WAIT_OBJECT_0 == dw ) {
		// done \o/
	} else if ( WAIT_TIMEOUT == dw ) {
		// wtf?
		debug( L"TonePlayer::_StopThread: WaitForSingleObject returned WAIT_TIMEOUT on a timeout of INFINITE?\n" );
	} else if ( WAIT_FAILED == dw ) {
		debug( L"TonePlayer::_StopThread: WaitForSingleObject failed, error=%lu\n", GetLastError( ) );
	} else {
		debug( L"TonePlayer::_StopThread: huh? dw=0x%08X, error=%lu\n", dw, GetLastError( ) );
	}

	delete ThreadObject;
	ThreadObject = nullptr;

	_ClearQueue( );
}
