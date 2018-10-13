// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_HOST
#define __MATOG_CODE_HOST

#include "CodeTemplate.h"
#include <matog/internal.h>
#include <matog/enums.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class Host : CodeTemplate {
private:
	const Array&					m_array;

	void init						(void) const;

	void initFlatItems				(void) const;
	void initVariants				(const array::Field& child) const;
	
	void initUnion					(Dict* dict, const array::Field& child, const bool printParentCount) const;
	void initOperator				(Dict* dict, const array::Field& child) const;
	void initAccessors				(void) const;
	void initAccessorsHelper		(Dict* preData, Dict* preAcc, const array::Field& child) const;
	void initTranspositions			(Dict* dict, const array::Field& child, const Degree* degree) const;
	void initPtrStruct				(Dict* dict, Dict*& soa, Dict*& aos, Dict*& aosoa, const array::Field& child) const;
	void initPtrStructItem			(Dict* dict, const array::Field& child, const Layout layout) const;
	void initRootFields				(Dict* dict) const;
	
	void method_initPtr				(void) const;
	void method_copy				(void) const;
	void method_compare				(void) const;
	void method_getCopySize			(void) const;
	void method_dyn_0				(void) const;
	void method_calcSize			(void) const;
	void method_calcSizeByElements	(void) const;

public:
	Host(const array::Field& field);
};

//-------------------------------------------------------------------
	}
}

#endif