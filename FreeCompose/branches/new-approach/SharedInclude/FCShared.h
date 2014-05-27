#pragma once

#ifdef FCSHARED_EXPORTS
#define FCSHARED_API __declspec(dllexport)
#else
#define FCSHARED_API __declspec(dllimport)
#endif

#ifdef _DEBUG
#define unreachable_return DebugBreak( ); return
#else
#define unreachable_return __assume( 0 ); return
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

#define LOCK(x) (x).Enter( ); do
#define UNLOCK(x) while ( false ); (x).Leave( );

//=============================================================================

#ifdef _DEBUG
FCSHARED_API void InitializeDebugLogFile( void );
FCSHARED_API void TerminateDebugLogFile( void );

FCSHARED_API void debug( _In_z_ _Printf_format_string_ LPCWSTR format, ... );

#ifdef _NOISY_DEBUG
FCSHARED_API void noisydebug( _In_z_ _Printf_format_string_ LPCWSTR format, ... );
#else
extern inline void noisydebug( _In_z_ _Printf_format_string_ LPCWSTR /*format*/, ... ) { }
#endif

#else
extern inline void InitializeDebugLogFile( void ) { }
extern inline void TerminateDebugLogFile( void ) { }

extern inline void debug( _In_z_ _Printf_format_string_ LPCWSTR /*format*/, ... ) { }
extern inline void noisydebug( _In_z_ _Printf_format_string_ LPCWSTR /*format*/, ... ) { }
#endif

FCSHARED_API CString GetFreeComposeFolder( void );
FCSHARED_API bool EnsureFreeComposeFolderExists( void );
