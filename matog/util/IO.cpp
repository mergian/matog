// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "IO.h"
#include "Mem.h"
#include "String.h"
#include "Env.h"
#include <matog/log.h>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <algorithm>
#include <matog/macros.h>

#ifdef _WIN32
#include <direct.h>
#include <Windows.h>
#define STAT _stat
#else
#include <unistd.h>
#include <dirent.h>
#define STAT stat
#endif

namespace matog {
	namespace util {
//-------------------------------------------------------------------
uint64_t IO::getModificationTime(const char* file) {
	struct STAT st;
	if(STAT(file, &st) == 0)
		return st.st_mtime;

	return 0;
}

//-------------------------------------------------------------------
std::list<String> IO::pathToList(const char* in) {
	std::list<String> out;
	std::istringstream ss(in);

	std::string str;

	while(ss.good()) {
		std::getline(ss, str, '/');

		if(!str.empty())
			out.push_back(str);
	}

	return out;
}

//-------------------------------------------------------------------
void IO::mkdir(const char* dirs) {
	std::istringstream ss(dirs);
	std::string dir;
	std::string fulldir(".");

	std::getline(ss, dir, '/');

	do {
		fulldir.append("/");
		fulldir.append(dir);
	
		if(!isDir(fulldir.c_str())) {
			L_TRACE("creating directory '%s'", fulldir.c_str());
			#ifdef _WIN32
				_mkdir(fulldir.c_str());
			#else
				struct STAT st;
				if(STAT(".", &st) != 0)
					st.st_mode = S_IRUSR | S_IWUSR | S_IXUSR;
				::mkdir(fulldir.c_str(), st.st_mode);
			#endif
		}	
		
		std::getline(ss, dir, '/');
	} while(ss.good());
}

//-------------------------------------------------------------------
bool IO::isFile(const char* file) {
	return !isDir(file);
}

//-------------------------------------------------------------------
bool IO::isDir(const char* dir) {
	struct STAT st;
	return STAT(dir, &st) == 0 && ((st.st_mode & S_IFDIR) != 0);
}
	
//-------------------------------------------------------------------
bool IO::doesFileExist(const char* file) {
	// open file stream
	std::ifstream istream(file);

	// is stream open?
	if(istream) {
		istream.close();
		return true;
	}

	// file does not exist
	return false;
}

//-------------------------------------------------------------------
bool IO::touch(const char* file) {
	// open file stream
	std::ifstream ostream(file, std::ios::app);

	// is stream open?
	if(ostream) {
		ostream.close();
		return true;
	}

	return false;
}

//-------------------------------------------------------------------
bool IO::rm(const char* file) {
	return remove(file) == 0;
}

//-------------------------------------------------------------------
void IO::rmdir(const char* dir) {
	THROW("NOT_IMPLEMENTED");
}

//-------------------------------------------------------------------
void IO::cleardir(const char* dir, const FileFilter* filter) {
	// get files
	std::list<String> files;
	IO::dir(files, dir, filter);

	// remove files
	for(auto&item : files) {
		std::ostringstream file;
		file << dir << "/" << item;
		IO::rm(file.str().c_str());
	}
}

//-------------------------------------------------------------------
void IO::dir(std::list<String>& files, String folder, const FileFilter* filter) {
#ifdef _WIN32
	// append * to path
	folder.append("\\*");

	// init handle
	WIN32_FIND_DATA ffd;
	HANDLE handle = INVALID_HANDLE_VALUE;
	
	// open handle
	handle = FindFirstFile(folder.c_str(), &ffd);

	// check handle
	if(handle == INVALID_HANDLE_VALUE)
		return;

	// iterate
	do {
		if(filter == 0 || filter->isValid(ffd.cFileName))
			files.push_back(ffd.cFileName);
	} while(FindNextFile(handle, &ffd));

	// close handle
	FindClose(handle);
#else
	DIR *pDIR = opendir(folder.c_str());
	struct dirent *entry;
	
	if(pDIR) {
		entry = readdir(pDIR);
		while(entry) {
			if(filter == 0 || filter->isValid(entry->d_name))
				files.push_back(entry->d_name);
				
			entry = readdir(pDIR);
		}
		closedir(pDIR);
	}
#endif
}

//-------------------------------------------------------------------
String IO::getFileName(const char* file, const bool withExtension) {
	size_t pos;
	std::string f(file);
	
	if(!withExtension) {
		pos = f.find_last_of('.');
		if(pos != SIZE_MAX)
			f = f.substr(0, pos);
	}
	
	pos = f.find_last_of('/');
	
	if(pos == SIZE_MAX)
		return f;

	return f.substr(pos + 1);
}

//-------------------------------------------------------------------
String IO::getFileExtension(const char* file) {
	size_t pos;
	std::string f(file);
	
	pos = f.find_last_of('.');
	
	if(pos == SIZE_MAX)
		return "";

	return f.substr(pos + 1);
}

//-------------------------------------------------------------------
String IO::getFilePath(const char* file) {
	size_t pos = std::string(file).find_last_of('/');
	
	if(pos == SIZE_MAX)
		return "";

	return String(file).substr(0, (uint32_t)pos + 1);
}

//-------------------------------------------------------------------
String IO::listToPath(const std::list<String>& list) {
	std::ostringstream out;

	bool isFirst = true;

	for(auto&item : list) {
		if(!isFirst)
			out << '/';
		else
			isFirst = false;
		out << item;
	}

	return out.str();
}

//-------------------------------------------------------------------
String IO::simplifyPath(const char* file) {
	// get path as list
	std::list<String> list = pathToList(file);
	std::list<String> out;

	for(auto&item : list) {
		if(item == ".") {
			continue;
		} else if(item == "..") {
			if(out.empty() || *out.rbegin() == "..") {
				out.push_back(item);
			} else {
				out.pop_back();
			}
		} else {
			out.push_back(item);
		}
	}

	return listToPath(out);
}

//-------------------------------------------------------------------
String IO::getExecutable(void) {
	char buffer[1024];
	String output;

#ifdef WIN32
	GetModuleFileName(NULL, buffer, 1024);
	output = String(buffer);
	output.replaceAll("\\", "/");
#else
	if(readlink("/proc/self/exe", buffer, 1024) == -1)
		CHECK("unable to fetch executable name");
	output = String(buffer);
#endif

	return output;
}

//-------------------------------------------------------------------
String IO::getExecutablePath(void) {
	return getFilePath(getExecutable());
}

//-------------------------------------------------------------------
const char* IO::readFile(const char* file) {
	std::ifstream in(file, std::ios::binary);
	
	// does file exist and is readable?
	if(!in)
		return 0;
	
	in.seekg(0, std::ios::end);

	size_t size = in.tellg();
	in.seekg(0, std::ios::beg);

	char* buffer = NEW_A(char, size + 1);
	in.read(buffer, size);
	in.close();

	buffer[size] = 0;

	return buffer;
}

//-------------------------------------------------------------------
blob IO::readBinary(const char* file, const bool nullTerminated) {
	if(!isFile(file))
		return blob();

	std::ifstream in(file, std::ifstream::binary);
	
	in.seekg(0, std::ios::end);
	const size_t size = in.tellg();
	char* ptr = NEW_A(char, size + (nullTerminated ? 1 : 0));
	in.seekg(0, std::ios::beg);
	
	in.read(ptr, size);

	if(nullTerminated)
		ptr[size] = 0;

	in.close();

	return blob(ptr, size);
}

//-------------------------------------------------------------------
size_t IO::getSize(const char* file) {
	std::ifstream in(file, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
} 

//-------------------------------------------------------------------
const char* IO::getTmpFolder(void) {
	const char* tmp = Env::get("TMP");

	if(!tmp) {
	#ifndef WIN32
		return "/tmp";
	#else
		return ".";
	#endif
	}

	return tmp;
}

//-------------------------------------------------------------------
const char* IO::getCommandline(void) {
	// get CMD line (OS depending)
#ifdef WIN32
	const char* tmp = GetCommandLine();
#else
	std::ostringstream out;

	char buf[2048];
	sprintf(buf, "/usr/bin/strings /proc/%u/cmdline", getpid());
	
	FILE* cmd = popen(buf, "r");
	while(fgets(buf, sizeof(buf), cmd)) {
		size_t len = strlen(buf);
		if(len > 1) {
			buf[len-1] = '\0';
			out << buf << " ";
		}
	}
	
	pclose(cmd);

	std::string str = out.str();
	const char* tmp = str.c_str();
#endif

	// copy to own char segment
	return NEW_STRING(tmp);
}

//-------------------------------------------------------------------
void IO::cp(const char* src, const char* dest) {
	std::ifstream in (src,  std::ios::binary);
	std::ofstream out(dest, std::ios::binary);
	
	char buffer[4096];
	size_t bytes = sizeof(buffer);
	
	while(in) {
		in.read(buffer, sizeof(buffer));
		if(!in)
			bytes = in.gcount();
		
		out.write(buffer, bytes);
	}

	THROWIF(!out.good(), "IO_UNABLE_TO_READ_FILE");

	in.close();
	out.close();
}

//-------------------------------------------------------------------
	}
}