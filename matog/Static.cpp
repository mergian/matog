// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/Static.h>
#include <matog/Kernel.h>
#include <matog/Degree.h>
#include <matog/degree/Set.h>
#include <matog/degree/Type.h>
#include <matog/log.h>
#include <set>
#include <unordered_set>
#include <matog/macros.h>

namespace matog {
//-------------------------------------------------------------------
std::map<uint32_t, Array>	Static::s_arrays;
std::map<uint32_t, Degree>	Static::s_degrees;
std::map<uint32_t, Module>	Static::s_modules;
std::map<uint32_t, Kernel>	Static::s_kernels;
std::map<uint32_t, GPU>		Static::s_gpus;
std::map<CUdevice, GPU*>	Static::s_gpuByDevice;
degree::Set					Static::s_functionDegrees;
degree::Set					Static::s_compilerDegrees;
degree::Set					Static::s_sharedDegrees;
degree::Set					Static::s_independentDegrees;
degree::Set					Static::s_defines;

//-------------------------------------------------------------------
float Static::getVersion(void) {
	return 5.0f;
}

//-------------------------------------------------------------------
void Static::init(void) {
	initDegrees();
	initArrays();
	initModules();
	initKernels();
	initGPUs();
}

//-------------------------------------------------------------------
void Static::initDegrees(void) {
	Degree::dbSelectAll(s_degrees);

	for(const auto& d : s_degrees) {
		if(d.second.isFunction())
			s_functionDegrees.emplace(&d.second);
		if(d.second.isCompiler())
			s_compilerDegrees.emplace(&d.second);                            
		if(d.second.isShared())
			s_sharedDegrees.emplace(&d.second);
		else
			s_independentDegrees.emplace(&d.second);
		if(d.second.getType() == degree::Type::Define)
			s_defines.emplace(&d.second);
	}

	assert(s_degrees.size());
}

//-------------------------------------------------------------------
void Static::initArrays(void) {
	Array::dbSelectAll(s_arrays);
	Array::dbSelectDegrees(s_arrays);
	assert(s_arrays.size());
}

//-------------------------------------------------------------------
void Static::initModules(void) {
	return Module::dbSelectAll(s_modules);
}

//-------------------------------------------------------------------
void Static::initKernels(void) {
	return Kernel::dbSelectAll(s_kernels);
}

//-------------------------------------------------------------------
void Static::initGPUs(void) {
	// add new GPUs if cuInit was executed
	int cnt = 0;
	CUresult result = cuDeviceGetCount(&cnt);

	if(result == CUDA_SUCCESS) {
		for(CUdevice i = 0; i < cnt; i++) {
			s_gpuByDevice.emplace(MAP_EMPLACE(i, &GPU::dbInsert(s_gpus, i)));
		}
	}

	GPU::dbSelectAll(s_gpus);
}

//-------------------------------------------------------------------
const Degree& Static::getDegree(const uint32_t id) {
	auto it = s_degrees.find(id);
	THROWIF(it == s_degrees.end(), "UNKNOWN_DEGREE");
	return it->second;
}

//-------------------------------------------------------------------
Degree& Static::getEditableDegree(const uint32_t id) {
	auto it = s_degrees.find(id);
	THROWIF(it == s_degrees.end(), "UNKNOWN_DEGREE");
	return it->second;
}

//-------------------------------------------------------------------
const Array& Static::getArray(const uint32_t id) {
	auto it = s_arrays.find(id);
	THROWIF(it == s_arrays.end(), "UNKNOWN_ARRAY");
	return it->second;
}

//-------------------------------------------------------------------
const Array& Static::getArray(const char* name) {
	for(const auto& it : s_arrays) {
		if(strcmp(it.second.getName(), name) == 0)
			return it.second;
	}

	THROW("UNKNOWN_ARRAY");
	return *(const Array*)0;
}

//-------------------------------------------------------------------
const Module& Static::getModule(const uint32_t id) {
	auto it = s_modules.find(id);
	THROWIF(it == s_modules.end(), "UNKNOWN_MODULE");
	return it->second;
}

//-------------------------------------------------------------------
const Module& Static::getModule(const char* name, const char* src) {
	uint32_t id = Module::dbSelectId(name);

	std::map<uint32_t, Module>::iterator it;

	if(id == UINT_MAX) {
		id = Module::dbInsert(name, src);
		it = s_modules.emplace(MAP_EMPLACE(id)).first;
		it->second.dbSelect(id);
	} else {
		it = s_modules.find(id);
	}

	THROWIF(it == s_modules.end(), "UNKNOWN_MODULE");
	it->second.updateFromSource();

	return it->second;
}

//-------------------------------------------------------------------
const Kernel& Static::getKernel(const char* name, const uint32_t moduleId) {
	uint32_t id = Kernel::dbSelectId(name);

	if(id == UINT_MAX) {
		THROWIF(moduleId == 0, "UNKNOWN_MODULE");
		id = Kernel::dbInsert(name, moduleId);
	}

	return getKernel(id);
}

//-------------------------------------------------------------------
const Kernel& Static::getKernel(const uint32_t id) {
	return getEditableKernel(id);
}

//-------------------------------------------------------------------
Kernel& Static::getEditableKernel(const uint32_t id) {
	// check if is in map
	auto it = s_kernels.find(id);

	if(it == s_kernels.end()) {
		it = s_kernels.emplace(MAP_EMPLACE(id)).first;
		it->second.dbSelect(id);
		THROWIF(!it->second.doesExist(), "UNKNOWN_KERNEL");
	}

	return it->second;
}

//-------------------------------------------------------------------
const GPU& Static::getGPU(const uint32_t id) {
	auto it = s_gpus.find(id);
	THROWIF(it == s_gpus.end(), "UNKNOWN_GPU");
	return it->second;
}

//-------------------------------------------------------------------
const GPU& Static::getGPUByDevice(const CUdevice id) {
	// check if is in map
	auto it = s_gpuByDevice.find(id);

	THROWIF(it == s_gpuByDevice.end(), "UNKNOWN_GPU");

	return *it->second;
}

//-------------------------------------------------------------------
const GPU& Static::getCurrentGPU(void) {
	CUdevice device = 0;
	CHECK(cuCtxGetDevice(&device));
	return getGPUByDevice(device);
}

//-------------------------------------------------------------------
}