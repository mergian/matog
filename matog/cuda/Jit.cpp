// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/cuda/Jit.h>
#include <matog/cuda/jit/Target.h>
#include <matog/util/IO.h>
#include <matog/util/Mem.h>
#include <matog/Options.h>
#include <matog/log.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <matog/macros.h>

#define NVCC_LOG "nvcc.log"
#define JIT_LOG {"log", "jit"}, false

namespace matog {
	namespace cuda {
//-------------------------------------------------------------------
using util::IO;

bool s_nvccFound = false;;
std::mutex s_jitMutex;

//-------------------------------------------------------------------
const util::blob Jit::compile(const jit::Target& target, const char* logFile, const char* cubinFile, const char* matogFile) {
	locateNVCC();

	assert(target.getSourceFile());
	assert(target.getMatogCode());
	assert(target.getArch() == 32 || target.getArch() == 64);
	assert(target.getCC());

	// write code file
	{
		std::ofstream file(matogFile, std::ios_base::trunc);
		file << target.getMatogCode();
		file.close();
	}

	// compile code
	std::ostringstream cmd;
	cmd << "nvcc "
		<< " -I. "
		#if _MSC_VER >= 1900 
			// necessary for stupid VS2015
			<< " -I\"C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.10240.0\\ucrt\""
			<< " -I\"C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.10150.0\\ucrt\""
		#endif
		<< " -m" << target.getArch()
		<< " -cubin -o \"" << cubinFile << "\""
		<< " -code=sm_" << target.getCC()
		<< " -arch=compute_" << target.getCC()
		<< " -include \"" << matogFile << "\" "
		<< " -cudart=none "
		#ifndef WIN32
			// necessary for GCC
			<< " -std=c++11 "
		#endif
		<< target.getUserFlags()
		<< " \"" << target.getSourceFile() << "\" > \"" << logFile << "\""
		<< " 2>&1";

	// build cmd string
	const std::string nvccCall = cmd.str();

	// wait for "permission" to be freed
	std::this_thread::sleep_for(std::chrono::milliseconds(300));

	// execute
	int returnValue = system(nvccCall.c_str());

	// check return value
	util::blob code;

	if(returnValue == EXIT_SUCCESS) {
		code = IO::readBinary(cubinFile);

		L_INSANE_
			const char* log = IO::readFile(logFile);
			printf("compiler cmd: %s\n", cmd.str().c_str());
			printf("%s", log);
			FREE_A(log);
		L_END
		
		IO::rm(logFile);
		IO::rm(cubinFile);
		IO::rm(matogFile);
	} else {
		const char* log = IO::readFile(logFile);

		L_ERROR("Unable to compile file '%s'. Return Value: %u", target.getSourceFile(), returnValue);
		L_ERROR("compiler cmd: %s", cmd.str().c_str());
		C_DEBUG(matog::log::Color::GREEN,	"matog.cu ----------------------------------------");
		L_DEBUG_
			const char* log = IO::readFile(matogFile);
			printf("%s", log);
			FREE_A(log);
		L_END
		C_DEBUG(matog::log::Color::GREEN,	"-------------------------------------------------");
		C_ERROR(matog::log::Color::RED,		"nvcc log ----------------------------------------");
		L_ERROR("%s", log);
		C_ERROR(matog::log::Color::RED,		"-------------------------------------------------");

		IO::rm(logFile);
		IO::rm(cubinFile);
		IO::rm(matogFile);

		FREE_A(log);
		THROW();
	}

	return code;
}

//-------------------------------------------------------------------
void Jit::locateNVCC(void) {
	LOCK(s_jitMutex);	

	if(s_nvccFound)
		return;

	// check if nvcc is set in the path variable
	const char* cmd =
	#if WIN32
	"nvcc --version > NUL";
	#else
	"nvcc --version > /dev/null";
	#endif

	THROWIF(system(cmd) != EXIT_SUCCESS, "JIT_UNABLE_TO_LOCATE_NVCC");
	s_nvccFound = true;
}

//-------------------------------------------------------------------
void Jit::generateFileNames(char*& logFile, char*& cubinFile, char*& matogFile, const char* tmpFolder, const int len) {
	std::hash<std::thread::id> hash;
	const uint64_t uid = hash(std::this_thread::get_id());

	// +24 because: +1 for appended /, +16 for Thread HEX, +1 terminating 0, +6 for matog_
	const int logLen	= len + 24 + 4; // .log
	const int cubinLen	= len + 24 + 6; // .cubin
	const int matogLen	= len + 24 + 4; // .cuh

	logFile   = new char[logLen + cubinLen + matogLen];
	cubinFile = logFile + logLen;
	matogFile = cubinFile + cubinLen;

	#ifdef WIN32
	#pragma warning(disable: 4996)
	#endif
	sprintf(logFile,   "%s/matog_%016llX.log",   tmpFolder, (unsigned long long int)uid);
	sprintf(cubinFile, "%s/matog_%016llX.cubin", tmpFolder, (unsigned long long int)uid);
	sprintf(matogFile, "%s/matog_%016llX.cu",    tmpFolder, (unsigned long long int)uid);
}

//-------------------------------------------------------------------
	}
}