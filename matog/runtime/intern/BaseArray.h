#ifndef __MATOG_RUNTIME_INTERN_BASEARRAY
#define __MATOG_RUNTIME_INTERN_BASEARRAY

#include <matog/runtime.h>
#include <matog/Array.h>
#include <matog/Static.h>
#include <matog/util/Hash.h>
#include <matog/array/Type.h>

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
template<class T>
class BaseArray : public T {
	BaseArray(void) = delete;

public:
	enum Type {
		Host,
		Device,
		DynamicShared
	};

	void* getKernelPtr(void) const {
		return (void*)T::m_array_0;
	}

	const Array& getArray(void) const {
		return *(Array*)T::m_array_1;
	}

	void init(const uint32_t arrayId, const bool isDevice) {
		T::m_array_1 = (void*)&Static::getArray(arrayId);
		T::m_array_2 = 0;
		T::m_array_5 = 0;
		T::m_array_6 = 0;

		if(getArray().getType() == matog::array::Type::Dyn)
			T::m_array_3 = (uint32_t)DynamicShared;
		else if(isDevice)
			T::m_array_3 = (uint32_t)Device;
		else
			T::m_array_3 = (uint32_t)Host;

		std::ostringstream ss;
		ss << getArray().getId() << T::file() << T::line();
		T::m_array_4 = util::Hash::crc32(ss.str().c_str());
	}

	uint32_t getEventId(void) const	{ 
		return T::m_array_2; 
	}

	void setEventId(const uint32_t value) { 
		T::m_array_2 = value; 
	}

	uint32_t getDecisionId(void) const	{ 
		return T::m_array_6; 
	}

	void setDecisionId(const uint32_t value) { 
		T::m_array_6 = value; 
	}

	uint32_t getHash(void) const { 
		return T::m_array_4; 
	}

	void setHash(const uint32_t value) { 
		T::m_array_4; 
	}

	Type getType(void) const { 
		return (Type)T::m_array_3;	
	}

	uint32_t getOffsetId(void) const { 
		return T::m_array_5; 
	}
	
	void setOffsetId(const uint32_t value) { 
		T::m_array_5 = value; 
	}

	uint64_t getCount(const uint32_t dim) const {
		assert(T::m_array_7);
		return T::m_array_7[dim];
	}
};

//-------------------------------------------------------------------
		}
	}
}

#endif