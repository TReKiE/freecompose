#pragma once

#include <cstddef>
#include <new>

template<typename T> class FreeListAllocator {

public:
	inline void* operator new( const std::size_t size ) {
		void* pv = NULL;

		if ( sizeof( T ) == size && NULL != _head ) {
			pv = _head;
			_head = _head->_next;
			return pv;
		}

		pv = malloc( size );
		return pv;
	}

	inline void operator delete( void* pv ) {
		FreeListAllocator<T>* pthis = reinterpret_cast< FreeListAllocator<T>* >( pv );
		pthis->_next = _head;
		_head = pthis;
	}

private:
	static FreeListAllocator<T>* _head;
	FreeListAllocator<T>* _next;

};

template<typename T> FreeListAllocator<T>* FreeListAllocator<T>::_head;
