#ifndef __SQLITE_DLL_H
#define __SQLITE_DLL_H

#ifdef WIN32
	#if defined(matog_sqlite3_EXPORTS) || defined(matog_sqlite3_debug_EXPORTS)
	#define SQLITE_API __declspec(dllexport)
	#else
	#define SQLITE_API __declspec(dllimport)
	#endif
#else
	#define SQLITE_API
#endif

#endif