// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Data.h"
#include <matog/db/Stmt.h>
#include <matog/log.h>
#include <matog/analyze.h>
#include <matog/macros.h>

using namespace matog::db;

namespace matog {
	namespace analyze {
		namespace training {
//-------------------------------------------------------------------
Data::Index::Index(const Stmt& stmt) :
	decisionId		(stmt.get<uint32_t>(0)),
	offset			 (stmt.get<uint32_t>(1)),
	dim				(stmt.get<uint32_t>(2))
{}

//-------------------------------------------------------------------
bool Data::Index::operator<(const Index& other) const {
	if(decisionId < other.decisionId)	return true;
	if(decisionId > other.decisionId)	return false;
	
	if(offset < other.offset)			return true;
	if(offset > other.offset)			return false;

	return dim < other.dim;
}
//-------------------------------------------------------------------
		}
	}
}