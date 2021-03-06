// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_MISC_MISC_H
#define __MATOG_MISC_MISC_H

namespace matog {
	namespace misc {
//-------------------------------------------------------------------
class Misc {
private:
	void initDB(int argc, char** argv);
	void predict(void);
	void appendixA(void);

public:
	Misc(int argc, char** argv);
};

//-------------------------------------------------------------------
	}
}

#endif