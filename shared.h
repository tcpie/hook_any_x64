#ifndef SHARED_H
#define SHARED_H

#if defined( LIBRARY_CODE )
#define ANY_HOOK __declspec(dllexport)
#else
#define ANY_HOOK __declspec(dllimport)
#endif


#endif