// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Clear.h"
#include <matog/cuda/CUBINStore.h>
#include <matog/Kernel.h>
#include <matog/kernel/Meta.h>
#include <matog/Options.h>
#include <matog/GPU.h>
#include <matog/Module.h>
#include <matog/db/PredictionModel.h>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
void Clear::cubins(void) {
	cuda::CUBINStore::dbDeleteAll();
}

//-------------------------------------------------------------------
void Clear::all(void) {
	// CUBINS
	cubins();

	// options
	Options				::dbDeleteAll();

	// profilings
	GPU					::dbDeleteAll();
	event				::dbDeleteAll();
	
	// modules
	Kernel				::dbDeleteAll();
	kernel::Meta		::dbDeleteAll();
	Module				::dbDeleteAll();
	Module				::dbDeleteAllDeps();

	// Prediction Models
	PredictionModel		::dbDeleteAll();
}

//-------------------------------------------------------------------
void Clear::profilings(void) {
	// config
	Options				::dbDeleteAll();

	// profilings
	event				::dbDeleteAll();
	
	// Prediction Models
	PredictionModel		::dbDeleteAll();
}

//-------------------------------------------------------------------
	}
}