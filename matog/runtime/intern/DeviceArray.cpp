#include <matog/runtime/intern/DeviceArray.h>
#include <algorithm>
#include <matog/array/field/Type.h>
#include <matog/cuda/CUDA.h>
#include <matog/cuda/Architecture.h>

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
uint32_t DeviceArray::getParamId(void) const {
	assert(getType() == Device);
	return m_device_0;
}

//-------------------------------------------------------------------
void DeviceArray::setParamId(const uint32_t value) {
	assert(getType() == Device);
	m_device_0 = value;
}

//-------------------------------------------------------------------
void DeviceArray::initTexRef(CUmodule module, const GPU& gpu, const uint32_t paramId) const {
	// nothing to do for CC >= 35
	if(gpu.getCC() >= 35)
		return;
	
	// init name
	std::string name(getArray().getName());
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);

	// get size
	const uint32_t size = (uint32_t)copy_size();
	const uint32_t maxTextureWidth = gpu.getMaxTextureWidth(); // 2^27
	
	// round size so a 4B aligned texture fits
	const uint32_t texSize = ((size + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);

	// calc num textures
	const uint32_t numTextures = (texSize + maxTextureWidth - 1) / maxTextureWidth;
	
	// init textures
	for(uint32_t i = 0; i < numTextures; i++) {
		// generate name
		std::ostringstream ss;
		ss << "__" << name << "_TEXREF_" << paramId << "_" << i;

		std::string texName(ss.str());

		// get reference
		size_t offset = 0;
		size_t bytes  = 0;
			
		if((i+1) < numTextures) 	bytes = maxTextureWidth * sizeof(uint32_t);
		else						bytes = texSize % (maxTextureWidth * sizeof(uint32_t));

		CUdeviceptr ptr = data_ptr() + i * maxTextureWidth * sizeof(uint32_t);

		CUtexref texRef;
		CHECK(cuModuleGetTexRef(&texRef, module, texName.c_str()));
		CHECK(cuTexRefSetAddress(&offset, texRef, ptr, bytes));

		THROWIF(offset != 0, "data ptr has to fullfill the texture alignment requirement!");

		// read as int
		CHECK(cuTexRefSetFlags(texRef, CU_TRSF_READ_AS_INTEGER));

		// set format
		CHECK(cuTexRefSetFormat(texRef, CU_AD_FORMAT_SIGNED_INT32, 1));
	}
}

//-------------------------------------------------------------------
		}
	}
}