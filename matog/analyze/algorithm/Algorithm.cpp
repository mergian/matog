#include <matog/analyze/algorithm/Algorithm.h>
#include <matog/analyze/algorithm/prediction/Prediction.h>
#include <matog/analyze/algorithm/prediction/Helper.h>
#include <matog/analyze/algorithm/brute/Brute.h>
#include <matog/analyze/graph/Graph.h>
#include <matog/event/ProfilingAlgorithm.h>
#include <matog/Options.h>
#include <matog/Static.h>
#include "Type.h"

#define CONFIG(VALUE, DEFAULT) {"analyze", "algorithm", VALUE}, DEFAULT

namespace matog {
	namespace analyze {
		namespace algorithm {
//-------------------------------------------------------------------
void Algorithm::exec(void) {
	C_INFO(log::GREEN, "-- solving graphs");

	prediction::Helper::initDomains();

	Type algorithm	= Options::get<Type>({"analyze", "algorithm"}, Type::Prediction, [](const char* algorithm) {
		if(strcmp(algorithm, "prediction") == 0)	return Type::Prediction;
		if(strcmp(algorithm, "brute") == 0)			return Type::Brute;
		if(strcmp(algorithm, "iterative") == 0)		return Type::Iterative;

		THROW("Unknown analyze::algorithm. Values are: prediction, brute or iterative");
		return (Type)0;
	});

	for(graph::Graph& graph : graph::Graph::getAllGraphs()) {
		switch(algorithm) {
			case Type::Prediction: {
				prediction::Prediction p(graph);
			} break;

			case Type::Brute: {
				brute::Brute b(graph);
			} break;

			case Type::Iterative: {
				THROW("TODO");
			}

			default:
				THROW();
		}
	}
}

//-------------------------------------------------------------------
		}
	}
}
