#ifndef __MATOG_RUNTIME_INTERN_HOSTARRAY
#define __MATOG_RUNTIME_INTERN_HOSTARRAY

#include <matog/runtime/HostArray.h>
#include "MemArray.h"

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
class HostArray : public MemArray<runtime::HostArray> {
};

//-------------------------------------------------------------------
		}
	}
}

#endif