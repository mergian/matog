#ifndef __MATOG_RUNTIME_INTERN_DYNARRAY
#define __MATOG_RUNTIME_INTERN_DYNARRAY

#include <matog/runtime/DynArray.h>
#include "BaseArray.h"

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
class DynArray : public BaseArray<runtime::DynArray> {
	DynArray(void) = delete;

public:
	uint32_t getParamId		(void) const;
	void	 setParamId		(const uint32_t id);
	uint32_t getOffset		(void) const;
	uint32_t setOffset		(const uint32_t offset, const variant::Value layout);
	uint32_t getSize		(const variant::Value layout) const;
	uint32_t getAlignment	(void) const;
};

//-------------------------------------------------------------------
		}
	}
}

#endif