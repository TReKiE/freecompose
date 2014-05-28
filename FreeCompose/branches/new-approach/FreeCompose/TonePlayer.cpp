#include "stdafx.h"

#include "TonePlayer.h"

//
// Data types
//

namespace {

	class Request {

	public:
		Request( ): dwFrequency( 0 ), dwDuration( 0 ) { }
		Request( DWORD const frequency, DWORD const duration ): dwFrequency( frequency ), dwDuration( duration ) { }

		DWORD dwFrequency;
		DWORD dwDuration;

	};

};

//
// Local variables
//

static CArray<Request> Queue;
static AutoEvent QueueEvent;
static AutoCriticalSection QueueLock;

static CWinThread* ThreadObject = nullptr;
static AutoCriticalSection ThreadLock;

static AutoEvent ShutdownEvent;

//
// Local functions
//

static bool Dequeue( DWORD& dwFrequency, DWORD& dwDuration ) {
	bool ret = false;
	LOCK( QueueLock ) {
		if ( Queue.GetCount( ) > 0 ) {
			dwFrequency = Queue[0].dwFrequency;
			dwDuration  = Queue[0].dwDuration;
			Queue.RemoveAt( 0, 1 );
			ret = true;
		}
	} UNLOCK( QueueLock );
	return ret;
}

static void DrainQueue( void ) {
	while ( true ) {
		DWORD dwFrequency, dwDuration;
		bool ret;

		LOCK( QueueLock ) {
			ret = Dequeue( dwFrequency, dwDuration );
		} UNLOCK( QueueLock );
		if ( !ret ) {
			return;
		}

		if ( !dwFrequency ) {
			Sleep( dwDuration );
		} else {
			Beep( dwFrequency, dwDuration );
		}
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

void TonePlayer::_Enqueue( DWORD const dwFrequency, DWORD const dwDuration ) {
	LOCK( QueueLock ) {
		Queue.Add( { dwFrequency, dwDuration } );
		QueueEvent.Set( );
	} UNLOCK( QueueLock );
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
