#ifndef HOOKANY64_SHARED_H
#define HOOKANY64_SHARED_H

#if defined( BUILD_HOOKANY64 )
#define HOOKANY64_EXPORT __declspec(dllexport)
#else
#define HOOKANY64_EXPORT __declspec(dllimport)
#endif


#endif