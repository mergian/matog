// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DB_CLEAR
#define __MATOG_DB_CLEAR

#include <matog/dll.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------

/// Helper Class to clear db entries
class MATOG_INTERNAL_DLL Clear {
public:
	/// Remove all cubins
	static void	cubins			(void);

	/// Removes all data from the db. Equal to factory reset.
	static void	all				(void);

	/// Removes all data from the db related to profilings
	static void profilings		(void);
};

//-------------------------------------------------------------------
	}
}

#endif