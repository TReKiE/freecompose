#pragma once

class AutoEvent {

public:
	inline AutoEvent( ) {
		m_hEvent = CreateEvent( nullptr, false, false, nullptr );
	}

	inline ~AutoEvent( ) {
		if ( m_hEvent ) {
			CloseHandle( m_hEvent );
			m_hEvent = nullptr;
		}
	}

	inline void Set( void ) {
		SetEvent( m_hEvent );
	}

	inline void Reset( void ) {
		ResetEvent( m_hEvent );
	}

	inline DWORD Wait( void ) {
		return WaitForSingleObject( m_hEvent, INFINITE );
	}

	inline DWORD Wait( DWORD dwMilliseconds ) {
		return WaitForSingleObject( m_hEvent, dwMilliseconds );
	}

	inline DWORD Wait( DWORD dwMilliseconds, bool fAlertable ) {
		return WaitForSingleObjectEx( m_hEvent, dwMilliseconds, fAlertable );
	}

protected:
	HANDLE m_hEvent;

};
