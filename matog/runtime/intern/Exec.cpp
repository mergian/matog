// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Exec.h"
#include "exec/Params.h"
#include "Context.h"
#include "BaseArray.h"
#include <matog/runtime/intern/Decisions.h>

#include <matog/array/field/Type.h>

#include <matog/cuda/CUDA.h>
#include <matog/cuda/Architecture.h>

#include <matog/util/Hash.h>
#include <matog/util/Exit.h>

#include <matog/Static.h>
#include <matog/array/Type.h>

#include <matog/log.h>

#include <vector_functions.h>
#include <matog/macros.h>
#include <matog/degree/Type.h>

using namespace matog::cuda;

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
Exec::Instances	Exec::s_instances;
exec::Compiler	Exec::s_compiler;

//-------------------------------------------------------------------
Exec::Exec(exec::Params& params) :
	m_params		(params),
	m_context		(Context::get()),
	m_deviceId		(CUDA::getCurrentDevice()),
	m_paramCount	(0),
	m_userFlags		(0),
	m_userHash		(0),
	m_args			(0),
	m_kernel		(Static::getKernel(params.kernelId)),
	m_module		(Static::getModule(m_kernel.getModuleId())),
	m_gpu			(Static::getGPUByDevice(m_deviceId)),
	m_launchItem	(0),
	m_launchHash	(UINT64_MAX)
{
	params.decisionId = Decisions::getId(0, &getKernel(), &getGPU());

	addInstance(this);

	prepareArrays();
	prepareUserFlags();
	
	m_args = NEW_A(void*, getParamCount());
	memcpy(m_args, getParams().args, sizeof(void*) * getParamCount());
}

//-------------------------------------------------------------------
Exec::~Exec(void) {
	removeInstance(this);

	if(m_userFlags)	
		FREE_A(m_userFlags);
	
	if(m_args)
		FREE_A(m_args);
}

//-------------------------------------------------------------------
CUresult Exec::launchKernel(uint32_t* exportSharedMemBytes) {
	THROWIF(getLaunchHash() == UINT64_MAX);
	THROWIF(getLaunchItem() == 0);

	const Variant& variant = getKernel().getVariant();

	// get func & Module
	CUmodule module = getLaunchItem()->getModule();
	CUfunction func = getLaunchItem()->getFunction(getKernel());

	// iterate all arguments
	uint32_t sharedMemBytes = getParams().sharedMemBytes;

	// FUNCTION OPTIMIZATIONS
	for(const auto& degree : Static::getFunctionDegrees()) {
		if(degree->getType() == degree::Type::L1Cache)
			cuFuncSetCacheConfig(func, (CUfunc_cache)(variant.get(getLaunchHash(), degree, 0) + 1));
		else
			THROW("NOT_IMPLEMENTED_FUNCTION_OPTIMIZATION");
	}

	// DYN SHARED ARRAYS
	for(auto& item : getDynSharedArrays()) {
		const uint32_t index = item.first;
		DynArray* ptr = item.second;

		sharedMemBytes = ptr->setOffset(sharedMemBytes,
			ptr->getArray().getRootLayout() ?
			variant.get(getLaunchHash(), ptr->getArray().getRootLayout(), ptr->getParamId()) :
			0
		);

		m_args[index] = ptr->getKernelPtr();
	}

	// DEVICE ARRAYS
	for(auto& item : getDeviceArrays()) {
		auto dev = item.second;
		const Array& array = dev->getArray();

		// update args
		m_args[item.first] = dev->getKernelPtr();

		// enforce initalization of array
		if(!dev->isInitialized())
			dev->setConfig(0);

		// init texture if necessary
		const uint32_t paramId = dev->getParamId();

		if(isTexRef(dev, getLaunchHash()))
			dev->initTexRef(module, getGPU(), paramId);

		// copy data to constant memory if necessary
		if(array.getRootMemory() && array.getRootMemory()->getValueCount() == 3 && (Memory)variant.get(getLaunchHash(), array.getRootMemory(), paramId) == Memory::CONSTANT) {
			std::ostringstream str;
			str << "__" << array.getName() << "_CONSTANT_" << paramId;

			CUdeviceptr ptr = 0;
			size_t size = 0;
			CHECK(cuModuleGetGlobal(&ptr, &size, getLaunchItem()->getModule(), str.str().c_str()));
			CHECK(cuMemcpyDtoDAsync(ptr, dev->data_ptr(), size, getParams().stream));
		}
	}

	if(exportSharedMemBytes)
		*exportSharedMemBytes = sharedMemBytes;

	return cuLaunchKernel(
		func, 
		getParams().grid.x, 
		getParams().grid.y, 
		getParams().grid.z, 
		getParams().block.x, 
		getParams().block.y, 
		getParams().block.z, 
		sharedMemBytes, 
		getParams().stream,
		m_args, 
		getParams().extra);
}

//-------------------------------------------------------------------
void Exec::addInstance(Exec* instance) {
	if(instance) {
		LOCK(s_instances.mutex);
		s_instances.instances.emplace(instance);
	}
}

//-------------------------------------------------------------------
void Exec::removeInstance(Exec* instance) {
	if(instance) {
		LOCK(s_instances.mutex);
		s_instances.instances.erase(instance);
	}
}

//-------------------------------------------------------------------
void Exec::notifyAll(const jit::Target& target) {
	LOCK(s_instances.mutex);

	for(auto& instance : s_instances.instances)
		instance->notify(target);
}

//-------------------------------------------------------------------
void Exec::prepareUserFlags(void) {
	// get flags
	const char* userFlags = getContext().getCompilerFlags(getKernel().getId());
	
	// create copy as we do not know if the option will be changed from another thread
	if(userFlags)
		m_userFlags = NEW_STRING(userFlags);
	else
		m_userFlags = NEW_STRING("-DNDEBUG");
	
	m_userHash = util::Hash::crc32(m_userFlags);
}

//-------------------------------------------------------------------
void Exec::prepareArrays(void) {
	// TODO:
	// We should print an error message if no array of the types in module_degrees is used, 
	// as this is an indication, that someone passed on a Array::Device** instead of Array::Device*!

	uint32_t i = 0;

	std::map<uint32_t, uint32_t> paramIds;

	Kernel* kernel = 0;
	variant::Set set;

	// INIT ALL NON PARAMETER DEGREES
	if(!getKernel().areDegreesInitialized()) {
		kernel = &Static::getEditableKernel(getKernel().getId());

		for(const auto& degree : getModule().getDegrees()) {
			const auto type = degree->isArray() ? degree->getArray()->getType() : matog::array::Type::None;

			if(type != matog::array::Type::Global && type != matog::array::Type::Dyn)
				set.emplace(degree, 0);
		}
	}

	// is REFHINT initialized
	kernel::hint::RefHints* hint = 0;
	if(!getKernel().getRefHints().isInitialized()) {
		hint = &Static::getEditableKernel(getKernel().getId()).getRefHints();
	}

	// ADD DEVICE ARRAY REFERENCES
	for(intern::BaseArray<>* ptr = 0; (ptr = ((intern::BaseArray<>*)getParams().args[i])) != 0; i++) {
		// is this a registered Variable?
		if(!getContext().isArray(ptr)) {
			continue;
		} else {
			// check if this is really a device var, or if the user has messed up!
			THROWIF(ptr->getType() == intern::BaseArray<>::Host, "Do not pass host arrays as function parameter to a kernel!");
		
			// get Array
			const Array& array = ptr->getArray();
			
			// special threadment
			switch(ptr->getType()) {
				// DynShared
				case intern::BaseArray<>::DynamicShared: {

					DynArray* dynPtr = (DynArray*)ptr;

					m_dynSharedArrays.emplace(MAP_EMPLACE(i, dynPtr));

					// set instance id
					const uint32_t paramId = paramIds[array.getId()]++;
					dynPtr->setParamId(paramId);

					// store degrees
					if(kernel)
						for(const variant::Item& item : array.getItems())
							set.emplace(item.getDegree(), paramId);

					L_TRACE("Param %u: %s (%u)", i, array.getName(), paramId);
				} break;

				// Device
				case intern::BaseArray<>::Device: {
					DeviceArray* devPtr = (DeviceArray*)ptr;
					m_deviceArrays.emplace(MAP_EMPLACE(i, devPtr));

					// set instance id
					const uint32_t paramId = paramIds[array.getId()]++;
					devPtr->setParamId(paramId);

					// store index if necessary
					if(hint)
						hint->add(array.getId(), paramId, i);

					// store degrees
					if(kernel)
						for(const variant::Item& item : array.getItems())
							set.emplace(item.getDegree(), paramId);

					L_TRACE("Param %u: %s (%u)", i, array.getName(), paramId);
				} break;

				// others
				default:
					break;
			}
		}
	}

	// store hint if necessary
	if(hint)
		hint->store(getKernel().getCompleteName());

	if(kernel)
		kernel->setDegrees(set);

	m_paramCount = i;
}

//-------------------------------------------------------------------
bool Exec::compile(const variant::Hash hash) {
	return s_compiler.push(this, hash);
}

//-------------------------------------------------------------------
bool Exec::isTexRef(const DeviceArray* dev, const variant::Hash hash) const {
	// only necessary for CC <= 30
	if(getGPU().getArchitecture() > matog::cuda::Architecture::KEPLER_30)
		return false;
	
	// is read mode?
	const RWMode mode = getKernel().getRWMode(dev->getArray().getId(), dev->getParamId());

	if(mode != RWMode::READ_ONCE && mode != RWMode::READ_ONLY)
		return false;
	
	// is this a texture anyway?
	if(!dev->getArray().getRootMemory())
		return false;
	
	// is config = texture?
	return getKernel().getVariant().get<matog::Memory>(hash, dev->getArray().getRootMemory(), dev->getParamId()) == matog::Memory::TEXTURE;
}

//-------------------------------------------------------------------
bool Exec::isTexture(const DeviceArray* dev) const {
	const RWMode mode = getKernel().getRWMode(dev->getArray().getId(), dev->getParamId());
	return dev->getArray().getRootMemory() && (mode == RWMode::READ_ONCE || mode == RWMode::READ_ONLY);
}

//-------------------------------------------------------------------
		}
	}
}