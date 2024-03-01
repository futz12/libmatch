#ifndef LIBMATCH_LIBRARY_H
#define LIBMATCH_LIBRARY_H

#ifdef LIBMATCH_EXPORTS
#define LIBMATCH_API extern "C"  __declspec(dllexport)
#else
#define LIBMATCH_API extern "C" __declspec(dllimport)
#endif

LIBMATCH_API const char* libmatch_version();

#endif //LIBMATCH_LIBRARY_H
