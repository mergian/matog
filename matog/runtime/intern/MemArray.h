#ifndef __MATOG_RUNTIME_INTERN_MEMARRAY
#define __MATOG_RUNTIME_INTERN_MEMARRAY

#include <matog/runtime/intern/BaseArray.h>

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
template<class T>
class MemArray : public BaseArray<T> {
	MemArray(void) = delete;

public:
	variant::Hash getConfig(void) const {
		return T::m_mem_0;
	}

	void setConfig(const variant::Hash value) {
		T::m_mem_0 = value;
	}

	bool isInitialized(void) const {
		return T::m_mem_0 != UINT64_MAX;
	}
};

//-------------------------------------------------------------------
		}
	}
}

#endif