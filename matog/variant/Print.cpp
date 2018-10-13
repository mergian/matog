// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/variant/Print.h>
#include <matog/variant/Item.h>
#include <matog/Degree.h>

namespace matog {
	namespace variant {
//-------------------------------------------------------------------
bool PrintComparator::operator()(const Item& a, const Item& b) const {
	if(a.getDegree()->getId() < b.getDegree()->getId())
		return true;

	if(a.getDegree()->getId() > b.getDegree()->getId())
		return false;

	return a.getParamId() < b.getParamId();
}

//-------------------------------------------------------------------
	}
}