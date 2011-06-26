#pragma once

#define countof(x) ( sizeof((x)) / sizeof((x)[0]) )

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
// And yes, since it's implemented with a do/while loop, you can cheerfully use
// "break" to exit a locked region!
//

#define LOCK(x) EnterCriticalSection(&(x)); do
#define UNLOCK(x) while ( false ); LeaveCriticalSection(&(x))
