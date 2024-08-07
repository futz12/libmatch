#ifndef LIBMATCH_LIBRARY_H
#define LIBMATCH_LIBRARY_H


#ifdef _WIN32
#ifdef LIBMATCH_EXPORTS
#define LIBMATCH_API extern "C"  __declspec(dllexport)
#else
#define LIBMATCH_API extern "C" __declspec(dllimport)
#endif
#else
#define LIBMATCH_API extern "C"
#endif

LIBMATCH_API const char *libmatch_version();

#endif //LIBMATCH_LIBRARY_H
