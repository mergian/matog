// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/GPU.h>
#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/cuda/Architecture.h>
#include <matog/cuda/NVDevice.h>
#include <matog/log.h>
#include <matog/macros.h>

using matog::db::Stmt;
using namespace matog::cuda;

namespace matog {
//-------------------------------------------------------------------
GPU::GPU(void) :
	m_id				(0),
	m_name				(0),
	m_cc				(0),
	m_sharedMem			(0),
	m_constMem			(0),
	m_smCount			(0),
	m_globalMem			(0),
	m_maxThreadsPerBlock(0),
	m_maxThreadsPerSM	(0),
	m_maxTexWidth		(0),
	m_texAlignment		(0),
	m_architecture		(Architecture::UNKNOWN)	
{
	// not intended to be called
	THROW("IMPLEMENTATION_ERROR");
}

//-------------------------------------------------------------------
GPU::GPU(const Stmt& stmt) :
	m_id				(stmt.get<uint32_t>(0)),
	m_name				(0),
	m_cc				(stmt.get<uint32_t>(5)),
	m_sharedMem			(stmt.get<uint32_t>(3)),
	m_constMem			(stmt.get<uint32_t>(4)),
	m_smCount			(stmt.get<uint32_t>(6)),
	m_globalMem			(stmt.get<uint64_t>(2)),
	m_maxThreadsPerBlock(stmt.get<uint32_t>(7)),
	m_maxThreadsPerSM	(stmt.get<uint32_t>(8)),
	m_maxTexWidth		(stmt.get<uint32_t>(10)),
	m_texAlignment		(stmt.get<uint32_t>(11)),
	m_architecture		(stmt.get<Architecture>(9))
{
	m_name = NEW_STRING(stmt.get<const char*>(1));

	assert(m_id);
	assert(m_cc);
	assert(m_sharedMem);
	assert(m_constMem);
	assert(m_smCount);
	assert(m_globalMem);
	assert((int)m_architecture);
	assert(m_maxTexWidth);
	assert(m_texAlignment);
}

//-------------------------------------------------------------------
GPU::~GPU(void) {
	if(m_name)
		FREE_A(m_name);
}

//-------------------------------------------------------------------
GPU& GPU::dbInsert(std::map<uint32_t, GPU>& map, const CUdevice deviceId) {
	const char* name = NVDevice::getName(deviceId);
	Stmt stmt("INSERT OR IGNORE INTO " DB_GPUS " (name, global_mem, shared_mem, const_mem, cc, sm_count, max_threads_block, max_threads_sm, architecture, max_tex_width, tex_alignment) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
	stmt.bind( 0,	name);
	stmt.bind( 1,	NVDevice::getGlobalMem(deviceId));
	stmt.bind( 2,	NVDevice::getSharedMem(deviceId));
	stmt.bind( 3,	NVDevice::getConstMem(deviceId));
	stmt.bind( 4,	NVDevice::getCC(deviceId));
	stmt.bind( 5,	NVDevice::getSMCount(deviceId));
	stmt.bind( 6,	NVDevice::getMaxThreadsPerBlock(deviceId));
	stmt.bind( 7,	NVDevice::getMaxThreadsPerSM(deviceId));
	stmt.bind( 8,	(int)NVDevice::getArchitecture(deviceId));
	stmt.bind( 9,	NVDevice::getMaxTextureWidth(deviceId));
	stmt.bind(10,	NVDevice::getTextureAlignment(deviceId));
	stmt.step();

	uint32_t id = 0;

	stmt("SELECT id, name, global_mem, shared_mem, const_mem, cc, sm_count, max_threads_block, max_threads_sm, architecture, max_tex_width, tex_alignment FROM " DB_GPUS " WHERE name = ? LIMIT 1;");
	stmt.bind(0, name);

	while(stmt.step()) {
		map.emplace(MAP_EMPLACE(stmt.get<uint32_t>(0), stmt));
		id = stmt.get<uint32_t>(0);
	}

	FREE_A(name);

	return map[id];		
}

//-------------------------------------------------------------------
void GPU::dbSelectAll(std::map<uint32_t, GPU>& map) {
	Stmt stmt("SELECT id, name, global_mem, shared_mem, const_mem, cc, sm_count, max_threads_block, max_threads_sm, architecture, max_tex_width, tex_alignment FROM " DB_GPUS ";");
	
	while(stmt.step())
		map.emplace(MAP_EMPLACE(stmt.get<uint32_t>(0), stmt));
}

//-------------------------------------------------------------------
void GPU::dbDelete(const uint32_t id) {
	Stmt("DELETE FROM " DB_GPUS " WHERE run_id = ?;").bind(0, id).step();
}

//-------------------------------------------------------------------
void GPU::dbDeleteAll(void) {
	Stmt("DELETE FROM " DB_GPUS ";").step();
	Stmt("DELETE FROM sqlite_sequence WHERE NAME = '" DB_GPUS "';").step();
}

//-------------------------------------------------------------------
}