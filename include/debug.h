#ifndef _debug_h
#define _debug_h

#ifndef DEBUG
#define debug(A, ...)
#else
#define debug(A, ...) printf(A)
#endif

#endif