// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file. 
#ifndef __MATOG__{{GLOBAL_NAME:x-uppercase}}
#define __MATOG__{{GLOBAL_NAME:x-uppercase}}

#include <type_traits>
#include <matog/matog.h>
#include <matog/log.h>
#include <matog/runtime/Malloc.h>
#include <matog/runtime/BaseArray.h>
#include <matog/runtime/DynArray.h>
#include <matog/runtime/HostArray.h>
#include <matog/runtime/DeviceArray.h>

// prevent stupid GCC diagnostic messages
#ifdef __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#ifdef __matog_inline__
#undef __matog_inline__
#endif

#define __matog_inline__ inline

class {{GLOBAL_NAME}} {
public:
// FORWARD DECLARATIONS ---------------------------------------------
	class Device;
	class Dyn;
	class Host;
// FORWARD DECLARATIONS ---------------------------------------------
	
// STATIC METHODS ---------------------------------------------------
	/// calculates the size for given element count
	static uint64_t calc_size_by_elements(const uint64_t cnt);

	/// calculates the size for given array sizes
	static uint64_t calc_size({{>INDEX}});
// STATIC METHODS ---------------------------------------------------

// HOST -------------------------------------------------------------
	class Host : public matog::runtime::HostArray {
	private:
		// UNION ----------------------------------------------------
		{{>UNION}}
		{{>STRUCT}}
		// UNION ----------------------------------------------------
		
		// POINTER --------------------------------------------------
		{{>HOST_POINTER}}
		{{#IS_DYN_COUNT}}uint64_t m_cnt[{{DIM_COUNT_DYN}}];{{/IS_DYN_COUNT}}
		// POINTER --------------------------------------------------
				
		// INTERNAL METHODS -----------------------------------------
		/// allocates the memory
		void __init_allocation(void);
		
		/// initializes the array sizes
		void* __init_counts({{>INDEX}});
		
		/// initializes sub array pointer
		void __init_ptr(void);
		
		/// format getter
		{{>FORMAT}}
						
	public:
		/// no direct init
		Host(void) = delete;
		
		/// no copying allowed
		Host(const Device&) = delete;
		
		/// default constructor
		Host({{#IS_DYN_COUNT}}{{>INDEX}}, {{/IS_DYN_COUNT}}const char* file, const int line, matog::runtime::HostMalloc* allocator = &matog::runtime::DefaultHostMalloc::getInstance());
		
		/// creates a clone of the array, with the same sizes, but does not copy the data
		virtual matog::runtime::HostArray* clone(const char* file, const int line, matog::runtime::HostMalloc* malloc = &matog::runtime::DefaultHostMalloc::getInstance()) const;
		
		/// creates a device-clone of the array, with the same sizes, but does not copy the data
		virtual matog::runtime::DeviceArray* clone_device(const char* file, const int line, matog::runtime::DeviceMalloc* malloc = &matog::runtime::DefaultDeviceMalloc::getInstance()) const;
		
		/// destructor
		virtual ~Host(void);
		
		/// copies data from source into this array
		virtual	void copy(const matog::runtime::HostArray* other);
		
		/// compares the data in this and the other array
		virtual bool compare(const matog::runtime::HostArray* other);
		
		/// gets the valid data size
		virtual uint64_t copy_size(void) const;
		
		/// element counts
		{{>HOST_ELEMENT_COUNTS}}
		
		/// counts
		{{>HOST_COUNTS}}
		
		/// ptr
		{{>PTR}}
		
		/// getter/setter
		{{>GETTER_SETTER}}
		
		/// accessor
		{{>ACCESSOR_FORWARD_DECLARATION}}
		{{>ACCESSOR_DATA}}
		{{>ACCESSOR}}
				
		/// Device has to be a friend
		friend class Device;
		friend class {{GLOBAL_NAME}};
	};
// HOST -------------------------------------------------------------
	
// DYN --------------------------------------------------------------
	class Dyn : public matog::runtime::DynArray {
	private:
		{{#IS_DYN_COUNT}}uint32_t m_cnt[{{DIM_COUNT_DYN}}]; ///< stores the array sizes
		{{/IS_DYN_COUNT}}
		
		/// inits the array sizes
		void* __init_counts({{>DYN_INDEX}});
	
	protected:
		/// internal use only
		virtual	uint32_t __dyn_0(const uint8_t) const;
		virtual	uint32_t __dyn_1(void) const;
		
	public:
		/// default constructor
		Dyn({{#IS_DYN_COUNT}}{{>DYN_INDEX}}, {{/IS_DYN_COUNT}}const char* file, const int line);
		
		/// element counts
		{{>DYN_ELEMENT_COUNTS}}
		
		/// counts
		{{>DYN_COUNTS}}
		
		/// destructor
		virtual ~Dyn(void);
	};
	
	// Device memory implementation
	class Device : public matog::runtime::DeviceArray {
	private:
		{{>DEVICE_POINTER}}
		{{#IS_DYN_COUNT}}{{DEVICE_INDEX_TYPE}} m_cnt[{{DIM_COUNT_DYN}}];{{/IS_DYN_COUNT}}
		
		/// inits array sizes
		void* __init_counts({{>DEVICE_INDEX}});

		/// allocates memory
		void __init_allocation(void);
		
		/// inits pointers
		void __init_ptr(void);
		
		/// format getter
		{{>FORMAT}}
		
	public:
		/// no default init
		Device(void) = delete;
		
		/// no copying
		Device(const Device&) = delete;
		
		/// constructor
		Device({{#IS_DYN_COUNT}}{{>DEVICE_INDEX}}, {{/IS_DYN_COUNT}}const char* file, const int line, matog::runtime::DeviceMalloc* allocator = &matog::runtime::DefaultDeviceMalloc::getInstance());
		
		/// element counts
		{{>DEVICE_ELEMENT_COUNTS}}
		
		/// counts
		{{>DEVICE_COUNTS}}
		
		/// creates a clone of the array, with the same sizes, but does not copy the data
		virtual matog::runtime::DeviceArray* clone(const char* file, const int line, matog::runtime::DeviceMalloc* malloc = &matog::runtime::DefaultDeviceMalloc::getInstance()) const;
		
		/// create a host-clone of the array, with the same sizez, but does not copy the data
		virtual matog::runtime::HostArray* clone_host(const char* file, const int line, matog::runtime::HostMalloc* malloc = &matog::runtime::DefaultHostMalloc::getInstance()) const;
		
		/// destructor
		virtual ~Device(void);
		
		/// gets the valid data size
		virtual uint64_t copy_size(void) const;
		
		/// Host has to be a friend
		friend class Host;
	};
};

#ifdef __GNUG__
#pragma GCC diagnostic pop
#endif

#endif