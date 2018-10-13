// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "ArrayStore.h"
#include <matog/runtime/intern/exec/Params.h>
#include <matog/runtime/intern/exec/Profiler.h>
#include <matog/runtime/intern/DeviceArray.h>
#include <matog/runtime/intern/HostArray.h>

#include <matog/util/Mem.h>
#include <matog/Kernel.h>
#include <matog/degree/Type.h>

#include <matog/log.h>
#include <matog/macros.h>

namespace matog {
	namespace runtime {
		namespace intern {
			namespace exec {
				namespace profiler {
//-------------------------------------------------------------------
ArrayStore::ArrayStore(const DeviceArrays& deviceArrays, const Params& params, const Kernel& kernel) : m_stream(params.stream), m_kernel(kernel) {
	// iterate all arguments
	for(auto& item : deviceArrays) {
		const uint32_t index	= item.first;
		DeviceArray* devPtr		= item.second;
		
		// get VarMap
		ArrayMaps& amap = m_argMap[index];
		m_arrayMap.emplace(MAP_EMPLACE(&devPtr->getArray(), &amap));

		// store variable
		amap.device = devPtr;

		// generate host copy only if NOT WRITE_ONLY
		if(m_kernel.getRWMode(devPtr->getArray().getId(), devPtr->getParamId()) != RWMode::WRITE_ONLY) {
			// make host cop
			intern::HostArray* host = (intern::HostArray*)NEW_P(amap.device->clone_host("MATOG_PROFILER", 0));
			host->setConfig(devPtr->getConfig() == UINT_MAX ? 0 : devPtr->getConfig());

			// store to map
			amap.host[host->getConfig()] = host;

			// copy data to host
			CHECK(::cuMemcpyDtoHAsync(host->data_ptr(), amap.device->data_ptr(), host->copy_size(), m_stream));
		}
	}

	// sync
	CHECK(::cuStreamSynchronize(m_stream));
}

//-------------------------------------------------------------------
ArrayStore::~ArrayStore(void) {
	// iterate all parameters
	for(auto& map : m_argMap) {
		// iterate all HostVars
		for(auto& host : map.second.host)
			FREE(host.second);
	}
}

//-------------------------------------------------------------------
HostArray* ArrayStore::getHostArray(const uint32_t param, const variant::Hash arrayConfig) {
	// the main thread and each covertion thread is allowed to work on
	// the host maps... therefore we have to lock here.
	LOCK(m_mutex);
	
	// get map
	auto& hostMap = m_argMap[param].host;
	
	THROWIF(hostMap.size() == 0, "IMPLEMENTATION_ERROR");

	// does the item exist?
	auto it = hostMap.find(arrayConfig);

	// data should be in the store, as otherwise the Profiler isn't allowed to enqueue the Target
	THROWIF(it == hostMap.end(), "IMPLEMENTATION_ERROR");

	// return value
	return it->second;
}

//-------------------------------------------------------------------
DeviceArray* ArrayStore::getDeviceArray(const uint32_t param, const variant::Hash config) {
	// only the main thread is supposed to work on the device Maps
	// therefore we do not need to lock here

	// get device var
	DeviceArray* device = m_argMap[param].device;

	// get old layout
	variant::Hash oldConfig = device->getConfig();

	// set format
	device->setConfig(config);

	const RWMode mode = m_kernel.getRWMode(device->getArray().getId(), device->getParamId());

	// do we have to copy the data or change the format?
	if(
		// if var is read and write --> always
		mode == RWMode::READ_AND_WRITE ||
		// if var is read and layout has changed
		(mode == RWMode::READ_ONLY  && (oldConfig != config))
		// if var is write only, we never have to transfer it
	) {
		// copy data
		HostArray* host = getHostArray(param, config);

		// copy data
		CHECK(::cuMemcpyHtoDAsync(device->data_ptr(), host->data_ptr(), host->copy_size(), m_stream));
	}
	
	// return value
	return device;
}

//-------------------------------------------------------------------
bool ArrayStore::convertArray(uint32_t& notificationCount, Profiler* profiler, const Variant& variant, const variant::Hash kernelHash, const matog::array::Set& arrays) {
	LOCK(m_mutex);
	
	bool hasToBeConverted = false;

	// iterate all degrees
	for(const auto& array : arrays) {
		// only applies to global arrays
		if(array->getType() != matog::array::Type::Global)
			continue;

		// get variant
		const Variant& aVariant = array->getVariant();

		// get map
		auto range = m_arrayMap.equal_range(array);

		// iterate all arrays
		for(; range.first != range.second; range.first++) {		
			ArrayMaps& arrayMap = *range.first->second;
			// convert only READ_ONLY and READ_AND_WRITE variables!
			if(m_kernel.getRWMode(arrayMap.device->getArray().getId(), arrayMap.device->getParamId()) != RWMode::WRITE_ONLY) {
				const uint32_t paramId = arrayMap.device->getParamId();
				
				variant::Hash arrayHash = 0;
				
				for(const variant::Item& item : aVariant.subset(variant::Type::Global)) {
					arrayHash += aVariant.hash(item, variant.get(kernelHash, item.getDegree(), paramId));
				}

				auto it = arrayMap.host.find(arrayHash);
			
				if(it == arrayMap.host.end()) {
					hasToBeConverted = true;
					notificationCount++;

					// enqueue conversion
					Profiler::s_converter.push(profiler, &arrayMap.host, &m_mutex, arrayHash, kernelHash);
				}
			}
		}
	}

	return hasToBeConverted;
}

//-------------------------------------------------------------------
				}
			}
		}
	}
}