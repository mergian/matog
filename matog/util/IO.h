// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_UTIL_IO
#define __MATOG_UTIL_IO

#include <matog/util.h>
#include <list>

namespace matog {
	namespace util {
//-------------------------------------------------------------------
/// Helper class for easy I/O operations
class MATOG_UTIL_DLL IO {
private:
	/// prevent initiation
	inline IO(void) {}

public:
	/// FileFilter interface
	class FileFilter {
	public:
		/// is file name valid to be listed?
		virtual bool isValid(const String& name) const = 0;
	};

	/// get filename of string
	static String				getFileName			(const char* file, const bool withExtension = true);

	/// get path of a string
	static String				getFilePath			(const char* file);

	/// get extension of a string
	static String				getFileExtension	(const char* file);

	/// remove file
	static bool					rm					(const char* file);

	/// remove dir
	static void					rmdir				(const char* dir);

	/// create dir
	static void					mkdir				(const char* dir);

	/// remove files in dir
	static void					cleardir			(const char* dir, const FileFilter* filter = 0);

	/// is a directory?
	static bool					isDir				(const char* dir);

	/// is a file?
	static bool					isFile				(const char* file);

	/// create file if does not exist
	static bool					touch				(const char* file);

	/// checks if file exists
	static bool					doesFileExist		(const char* file);

	/// get modification time of file
	static uint64_t				getModificationTime	(const char* file);

	/// lists all files in dir
	static void					dir					(std::list<String>& files, String folder, const FileFilter* filter = 0);

	/// creates substrings of folders in path
	static std::list<String>	pathToList			(const char* path);

	/// creates a path string from list of folders
	static String				listToPath			(const std::list<String>& list);

	/// simplifies path string (e.g. removes double //)
	static String				simplifyPath		(const char* file);

	/// get current executable
	static String				getExecutable		(void);

	/// get current executable path
	static String				getExecutablePath	(void);

	/// get current execution command line
	static const char*			getCommandline		(void);

	/// read file ASCII
	static const char*			readFile			(const char* file);

	/// read file Binary
	static blob					readBinary			(const char* file, const bool nullTerminated = false);

	/// get path to tmp folder
	static const char*			getTmpFolder		(void);

	/// get size of file
	static size_t				getSize				(const char* file);

	/// copy from src to dest
	static void					cp					(const char* src, const char* dest);
};
//-------------------------------------------------------------------
	}
}

#endif