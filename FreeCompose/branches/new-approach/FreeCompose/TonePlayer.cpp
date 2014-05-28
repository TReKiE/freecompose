#include "stdafx.h"

#include "TonePlayer.h"

//
// Data types
//

class _Request {

public:
	_Request( ): dwFrequency( 0 ), dwDuration( 0 ) { }
	_Request( DWORD const frequency, DWORD const duration ): dwFrequency( frequency ), dwDuration( duration ) { }

	DWORD dwFrequency;
	DWORD dwDuration;

};

//
// Local variables
//

static CArray<_Request> Queue;
static AutoEvent QueueEvent;
static AutoCriticalSection QueueLock;

static CWinThread* ThreadObject = nullptr;
static AutoCriticalSection ThreadLock;

static AutoEvent ShutdownEvent;

//
// Local functions
//

static UINT TonePlayerThreadFunction( LPVOID /*pvParam*/ ) {
	
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

bool TonePlayer::_Dequeue( DWORD& dwFrequency, DWORD& dwDuration ) {
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
	while ( true ) {
		SetLastError( ERROR_SUCCESS );
		DWORD dw = WaitForSingleObject( hThread, INFINITE );
		// WAIT_ABANDONED only applies to mutexes.
		if ( WAIT_OBJECT_0 == dw ) {
			break;
		}
		if ( WAIT_TIMEOUT == dw ) {
			// wtf?
			debug( L"TonePlayer::_StopThread: WaitForSingleObject returned WAIT_TIMEOUT on a timeout of INFINITE?\n" );
			break;
		}
		if ( WAIT_FAILED == dw ) {
			debug( L"TonePlayer::_StopThread: WaitForSingleObject failed, error=%lu\n", GetLastError( ) );
			break;
		}
		debug( L"TonePlayer::_StopThread: huh? dw=%lu error=%lu (and retrying wait)\n", dw, GetLastError( ) );
	}

	delete ThreadObject;
	ThreadObject = nullptr;

	_ClearQueue( );
}
