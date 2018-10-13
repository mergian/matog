// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_MATOGBUILDER_H
#define __MATOG_CODE_MATOGBUILDER_H

#include <matog/util/String.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class MatogBuilder {
private:
	// methods
	void	printHelp			(void);
	void	parseArguments		(int argc, char** argv);
	void	parseFile			(void);
	
public:
	// constructor
	MatogBuilder(int argc, char** argv);
};

//-------------------------------------------------------------------
	}
}

#endif