#pragma once

#ifdef FCSHARED_EXPORTS
#define FCSHARED_API __declspec(dllexport)
#else
#define FCSHARED_API __declspec(dllimport)
#endif

#if NDEBUG
#define unreachable_return __assume( 0 ); return
#else
#define unreachable_return DebugBreak( ); return
#endif

//=============================================================================

//
// Ice Karma's standard lock constructs. Use like this:
//
// LOCK( someCriticalSection ) {
//     ...
//     ...
//     if ( somethingDisagreeable )
//         break;
//     ...
//     ...
// } UNLOCK( someCriticalSection );
//
// Note that you must use the _same_ critical section object in both places!
// And yes, since it's implemented with a do/while loop, you can SAFELY use
// "break" to exit a locked region!
//

#define LOCK(x) EnterCriticalSection(&(x)); do
#define UNLOCK(x) while ( false ); LeaveCriticalSection(&(x))

//=============================================================================

#ifdef NDEBUG
static inline void debug( LPCWSTR /*format*/, ... ) { }
#else
FCSHARED_API void debug( LPCWSTR format, ... );
#endif

FCSHARED_API bool GetFreeComposeFolder( LPWSTR& lpsz );
FCSHARED_API bool EnsureFreeComposeFolderExists( void );

FCSHARED_API void InitializeDebugLogFile( void );
FCSHARED_API void TerminateDebugLogFile( void );
