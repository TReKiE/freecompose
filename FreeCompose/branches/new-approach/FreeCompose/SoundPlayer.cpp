#include "stdafx.h"

#include "SoundPlayer.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

//
// Request queue stuff
//

namespace {

	// Local methods

	inline unsigned long long _GetSystemTime( void ) {
		unsigned long long now = 0;
		GetSystemTimeAsFileTime( reinterpret_cast<FILETIME*>( &now ) );
		return now;
	}

	inline double _TimeDelta( unsigned long long const start, unsigned long long const finish ) {
		return ( finish - start ) / 10000000.0;
	}

	inline double _TimeSince( unsigned long long const then ) {
		return _TimeDelta( then, _GetSystemTime( ) );
	}

	//
	// Class Request: Abstract base class for Request hierarchy.
	//

	class Request {

	public:
		inline Request( ): _createdAt( _GetSystemTime( ) ), _startedAt( 0 ), _finishedAt( 0 ) { }
		inline virtual ~Request( ) { }

		virtual void Perform( void ) = 0;

	protected:
		unsigned long long _createdAt;
		unsigned long long _startedAt;
		unsigned long long _finishedAt;

	};

	//
	// Class CompositionSoundRequest: Derivative of class Request that plays one of FreeCompose's configured application sounds through the system sounds API.
	//

	class CompositionSoundRequest: public Request {

	public:
		inline CompositionSoundRequest( ): Sound( CompositionSound::none ) { }
		inline CompositionSoundRequest( CompositionSound const sound ): Sound( sound ) { }
		inline virtual ~CompositionSoundRequest( ) { }

		inline virtual void Perform( void ) {
			debug( L"CompositionSoundRequest::Perform: '%s'\n", CompositionSoundNames[static_cast<int>( Sound )] );
			debug( L"+ Time since queueing: %.3f s\n", _TimeSince( _createdAt ) );

			_startedAt = _GetSystemTime( );
			PlaySound( CompositionSoundNames[static_cast<int>( Sound )], nullptr, SND_ALIAS | SND_APPLICATION | SND_SYNC );
			_finishedAt = _GetSystemTime( );
			debug( L"+ Time to perform: %.3f s\n", _TimeDelta( _startedAt, _finishedAt ) );
		}

		CompositionSound Sound;

	};

	//
	// Class SilenceRequest: Derivative of class Request that simply produces silence.
	//

	class SilenceRequest: public Request {
		
	public:
		inline SilenceRequest( ): dwDuration( 0 ) { }
		inline SilenceRequest( DWORD const duration ): dwDuration( duration ) { }
		inline virtual ~SilenceRequest( ) { }

		inline virtual void Perform( void ) {
			debug( L"SilenceRequest::Perform: silence for %lu ms\n", dwDuration );
			debug( L"+ Time since queueing: %.3f s\n", _TimeSince( _createdAt ) );

			_startedAt = _GetSystemTime( );
			Sleep( dwDuration );
			_finishedAt = _GetSystemTime( );
			debug( L"+ Time to perform: %.3f s\n", _TimeDelta( _startedAt, _finishedAt ) );
		}

		DWORD dwDuration;

	};

	//
	// Class ToneRequest: Derivative of class Request that plays a single tone.
	//

	class ToneRequest: public Request {

	public:
		inline ToneRequest( ): dwFrequency( 0 ), dwDuration( 0 ) { }
		inline ToneRequest( DWORD const frequency, DWORD const duration ): dwFrequency( frequency ), dwDuration( duration ) { }
		inline virtual ~ToneRequest( ) { }

		inline virtual void Perform( void ) {
			debug( L"ToneRequest::Perform: %lu Hz for %lu ms\n", dwFrequency, dwDuration );
			debug( L"+ Time since queueing: %.3f s\n", _TimeSince( _createdAt ) );

			_startedAt = _GetSystemTime( );
			Beep( dwFrequency, dwDuration );
			_finishedAt = _GetSystemTime( );
			debug( L"+ Time to perform: %.3f s\n", _TimeDelta( _startedAt, _finishedAt ) );
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
	debug( L"Dequeue: 0x%p\n", p );
	return p;
}

static void DrainQueue( void ) {
	debug( L"DrainQueue\n" );
	Request* pReq;
	while ( nullptr != ( pReq = Dequeue( ) ) ) {
		pReq->Perform( );
		delete pReq;
	}
}

static UINT SoundPlayerThreadFunction( LPVOID /*pvParam*/ ) {
	HANDLE waitHandles[2] = { QueueEvent, ShutdownEvent };

	while ( true ) {
		SetLastError( ERROR_SUCCESS );
		DWORD dw = WaitForMultipleObjectsEx( 2, waitHandles, false, INFINITE, true );
		if ( WAIT_OBJECT_0 == dw ) {
			DrainQueue( );
		} else if ( WAIT_OBJECT_0 + 1 == dw ) {
			return 0;
		} else {
			debug( L"SoundPlayerThreadFunction: Something went wrong with WaitForMultipleObjects. dw=0x%08X, error=%lu\n", dw, GetLastError( ) );
			return 1;
		}
	}
	
	return 0;
}

//
// Class SoundPlayer
//

void SoundPlayer::_EnqueueCompositionSound( CompositionSound const sound ) {
	Enqueue( new CompositionSoundRequest( sound ) );
}

void SoundPlayer::_EnqueueSilence( DWORD const dwDuration ) {
	Enqueue( new SilenceRequest( dwDuration ) );
}

void SoundPlayer::_EnqueueTone( DWORD const dwFrequency, DWORD const dwDuration ) {
	Enqueue( new ToneRequest( dwFrequency, dwDuration ) );
}

void SoundPlayer::_ClearQueue( void ) {
	LOCK( QueueLock ) {
		Queue.RemoveAll( );
		QueueEvent.Reset( );
	} UNLOCK( QueueLock );
}

void SoundPlayer::_CheckStartThread( void ) {
	LOCK( ThreadLock ) {
		if ( ThreadObject ) {
			break;
		}
		
		ThreadObject = AfxBeginThread( SoundPlayerThreadFunction, nullptr );
		ThreadObject->m_bAutoDelete = FALSE;
	} UNLOCK( ThreadLock );
}

void SoundPlayer::_StopThread( void ) {
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
		debug( L"SoundPlayer::_StopThread: WaitForSingleObject returned WAIT_TIMEOUT on a timeout of INFINITE?\n" );
	} else if ( WAIT_FAILED == dw ) {
		debug( L"SoundPlayer::_StopThread: WaitForSingleObject failed, error=%lu\n", GetLastError( ) );
	} else {
		debug( L"SoundPlayer::_StopThread: huh? dw=0x%08X, error=%lu\n", dw, GetLastError( ) );
	}

	delete ThreadObject;
	ThreadObject = nullptr;

	_ClearQueue( );
}
