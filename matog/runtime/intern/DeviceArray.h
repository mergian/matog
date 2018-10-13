#ifndef __MATOG_RUNTIME_INTERN_DEVICEARRAY
#define __MATOG_RUNTIME_INTERN_DEVICEARRAY

#include <matog/runtime/intern/MemArray.h>
#include <matog/runtime/DeviceArray.h>
#include <matog/Array.h>

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
class DeviceArray : public MemArray<runtime::DeviceArray> {
	DeviceArray(void) = delete;

public:
	uint32_t					getParamId			(void) const;
	void						setParamId			(const uint32_t value);

	void						initTexRef			(CUmodule module, const GPU& gpu, const uint32_t paramId) const;
};

//-------------------------------------------------------------------
		}
	}
}

#endif