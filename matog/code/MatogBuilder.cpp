// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "MatogBuilder.h"
#include "CodeModel.h"
#include "CMakeModel.h"
#include "FileParser.h"
#include <matog/log.h>
#include <iomanip>
#include <matog/Static.h>

using namespace matog::util;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
MatogBuilder::MatogBuilder(int argc, char** argv) {
	// parse arguments
	L_INFO("Parsing command line arguments...");
	parseArguments(argc, argv);

	// parse MATOG file
	L_INFO("Parsing JSON description...");
	parseFile();
	Static::init();
	CodeModel		::getInstance().generate();
	CMakeModel		::getInstance().generate();

	L_INFO("Operation complete.");
}

//-------------------------------------------------------------------
void MatogBuilder::printHelp(void) {
	L_INFO("	usage: matog-code [debug]");
}

//-------------------------------------------------------------------
void MatogBuilder::parseFile(void) {
	// parse file and optain model
	FileParser::getInstance().parse();
}

//-------------------------------------------------------------------
void MatogBuilder::parseArguments(int argc, char** argv) {
	// iterate all arguments
	for(int i = 1; i < argc; i++) {
		// extract argument and parameter
		String str(argv[i]);

		size_t pos = str.find('=');

		String argument;
		String parameter;

		if(pos != SIZE_MAX) {
			argument = str.substr(0, (uint32_t)pos);
			parameter = str.substr((uint32_t)pos+1);
		} else {
			argument = str;
		}

		// execute argument
		// help?
		if(argument == "/?" || argument == "-h" || argument == "--help") {
			printHelp();
			exit(0);
		} else if(argument == "debug") {
			log::setLevel(log::DEBUG);
		}
		
		// unknown flag
		else {
			THROW("UNKNOWN_ARGS");
		}
	}
}

//-------------------------------------------------------------------
	}
}