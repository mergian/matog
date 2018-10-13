// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYSE_ANALYSE_H
#define __MATOG_ANALYSE_ANALYSE_H

namespace matog {
	namespace analyze {
//-------------------------------------------------------------------
class Analyze {
private:
	void initDB(int argc, char** argv);

public:
	Analyze(int argc, char** argv);
};

//-------------------------------------------------------------------
	}
}

#endif