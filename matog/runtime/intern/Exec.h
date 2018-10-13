// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_RUNTIME_INTERN_EXEC
#define __MATOG_RUNTIME_INTERN_EXEC

#include <matog/runtime.h>
#include <matog/runtime/intern/Context.h>
#include <matog/runtime/intern/exec/Params.h>
#include <matog/runtime/intern/exec/Compiler.h>
#include <matog/runtime/intern/DeviceArray.h>
#include <matog/runtime/intern/DynArray.h>
#include <tbb/task_group.h>

namespace matog {
	namespace runtime {
		namespace intern {
//-------------------------------------------------------------------
class Exec {
private:
	// STATIC
	static struct Instances {
		std::set<Exec*>				instances;
		std::mutex					mutex;
	} s_instances;

	static exec::Compiler			s_compiler;

	// INSTANCE
	const	exec::Params&			m_params;
			Context&				m_context;
	const	CUdevice				m_deviceId;
			uint32_t				m_paramCount;
	const	char*					m_userFlags;
			uint32_t				m_userHash;
			void**					m_args;
					
	const	Kernel&					m_kernel;
	const	Module&					m_module;
	const	GPU&					m_gpu;
			context::cache::Item*	m_launchItem;
			variant::Hash			m_launchHash;

			exec::DeviceArrays		m_deviceArrays;
			exec::DynSharedArrays	m_dynSharedArrays;

	void prepareArrays				(void);
	void prepareUserFlags			(void);

protected:
											Exec				(exec::Params& params);
	static	void							addInstance			(Exec* instance);
	static	void							removeInstance		(Exec* instance);
	virtual void							notify				(const cuda::jit::Target& target) = 0;
			bool							compile				(const variant::Hash hash);

	inline	void							setLaunch			(context::cache::Item& item, const variant::Hash hash)	{ setLaunchItem(item); setLaunchHash(hash); }
	inline	void							setLaunchItem		(context::cache::Item& item)							{ m_launchItem = &item; }
	inline	void							setLaunchHash		(const variant::Hash hash)								{ m_launchHash = hash; }
	inline	context::cache::Item*			getLaunchItem		(void) const	{ return m_launchItem; }
	inline	variant::Hash					getLaunchHash		(void) const	{ return m_launchHash; }

	inline const exec::Params&				getParams			(void) const	{ return m_params; }
	inline Context&							getContext			(void) const	{ return m_context; }
	inline CUdevice							getDeviceId			(void) const	{ return m_deviceId; }
	inline uint32_t							getParamCount		(void) const	{ return m_paramCount; }
	inline const char*						getUserFlags		(void) const	{ return m_userFlags; }
	inline uint32_t							getUserHash			(void) const	{ return m_userHash; }  

	inline const	Kernel&					getKernel			(void) const	{ return m_kernel; }
	inline const	Module&					getModule			(void) const	{ return m_module; }
	inline const	GPU&					getGPU				(void) const	{ return m_gpu; }
	inline			context::Cache&			getCache			(void)			{ return getContext().getCache(); }

	inline const	exec::DeviceArrays&		getDeviceArrays		(void) const	{ return m_deviceArrays; }
	inline const	exec::DynSharedArrays&	getDynSharedArrays	(void) const	{ return m_dynSharedArrays; }
	inline			uint32_t				getThreadCount		(void) const	{ return m_params.block.x * m_params.block.y * m_params.block.z; }

					bool					isTexRef			(const DeviceArray* dev, const variant::Hash hash) const;
					bool					isTexture			(const DeviceArray* dev) const;
public:
	virtual									~Exec				(void);
			CUresult						launchKernel		(uint32_t* exportSharedMemBytes = 0);
	static	void							notifyAll			(const cuda::jit::Target& target);

	friend class exec::Compiler;
};

//-------------------------------------------------------------------
		}
	}
}

#endif