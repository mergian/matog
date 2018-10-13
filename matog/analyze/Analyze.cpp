// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/analyze/Analyze.h>
#include <matog/analyze/graph/Graph.h>
#include <matog/analyze/algorithm/Algorithm.h>
#include <matog/analyze/training/Training.h>
#include <matog/db/PredictionModel.h>
#include <matog/Static.h>
#include <matog/Options.h>
#include <matog/cuda/CUBINStore.h>
#include <matog/util.h>
#include <matog/log.h>

using namespace matog::cuda;
using namespace matog::analyze::graph;
using namespace matog::analyze::algorithm;
using namespace matog::analyze::training;

namespace matog {
	namespace analyze {
//-------------------------------------------------------------------
Analyze::Analyze(int argc, char** argv) {
	// init db
	initDB(argc, argv);

	// clear old prediction models
	db::PredictionModel::dbDeleteAll();

	// build graphs
	Graph::exec();

	// analyze graphs
	Algorithm::exec();

	// build models
	Training::exec();

	// compile
	//Compiler::exec();

	// export dot if wanted by user
	Graph::exportDot();
}

//-------------------------------------------------------------------
void Analyze::initDB(int argc, char** argv) {
	// print usage
	if(argc > 2) {
		L_INFO("usage: matog-analyze [db-file]");
		exit(0);
	}

	// set db
	if(argc == 2)
		DB::setDBFile(argv[1]);

	// init Static
	Static::init();

	// init Options
	Options::init();

	// init CUBINStore
	CUBINStore::init();
}

//-------------------------------------------------------------------
	}
}