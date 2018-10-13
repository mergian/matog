#include <matog/runtime/intern/DynArray.h>
#include <matog/degree/Type.h>
#include <cassert>

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
uint32_t DynArray::getParamId(void) const {
	return m_dyn_0;
}

//-------------------------------------------------------------------
void DynArray::setParamId(const uint32_t id) {
	m_dyn_0 = id;
}
	
//-------------------------------------------------------------------
uint32_t DynArray::getOffset(void) const {
	assert(getType() == DynamicShared);
	return m_dyn_1;
}

//-------------------------------------------------------------------
uint32_t DynArray::setOffset(const uint32_t offset, const variant::Value layout) {
	assert(getType() == DynamicShared);

	// set offset for this element
	m_dyn_1 = align(offset, getAlignment());

	// return new offset
	return m_dyn_1 + getSize(layout);
}

//-------------------------------------------------------------------
uint32_t DynArray::getSize(const variant::Value layout) const {
	return __dyn_0(layout);
}

//-------------------------------------------------------------------
uint32_t DynArray::getAlignment(void) const {
	return __dyn_1();
}

//-------------------------------------------------------------------
		}
	}
}