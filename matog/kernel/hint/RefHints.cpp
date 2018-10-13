#include "RefHints.h"
#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/macros.h>
#include <matog/log.h>

namespace matog {
	namespace kernel {
		namespace hint {
//-------------------------------------------------------------------
void RefHints::load(const char* kernelName) {
	m_index.clear();
	m_mappings.clear();

	db::Stmt stmt("SELECT array_id, param_id, dim, src_array_id, src_param_id, src_dim FROM " DB_KERNEL_HINTS_REF " WHERE kernel_name = ?;");
	stmt.bind(0, kernelName);

	while(stmt.step())
		m_mappings.emplace(MAP_EMPLACE(
			Identifier2(stmt.get<uint32_t>(0), stmt.get<uint32_t>(1), stmt.get<uint32_t>(2)),
			stmt.get<uint32_t>(3), stmt.get<uint32_t>(4), stmt.get<uint32_t>(5)));

	stmt("SELECT array_id, param_id, idx FROM " DB_KERNEL_HINTS_REF_INDEX " WHERE kernel_name = ?;");
	stmt.bind(0, kernelName);

	while(stmt.step())
		m_index.emplace(MAP_EMPLACE(Identifier(stmt.get<uint32_t>(0), stmt.get<uint32_t>(1)), stmt.get<uint32_t>(2)));
}

//-------------------------------------------------------------------
bool RefHints::isInitialized(void) const {
	return !m_index.empty();	
}

//-------------------------------------------------------------------
void RefHints::add(const uint32_t arrayId, const uint32_t paramId, const uint32_t index) {
	m_index.emplace(MAP_EMPLACE(Identifier(arrayId, paramId), index));
}

//-------------------------------------------------------------------
void RefHints::store(const char* kernelName) {
	db::Stmt stmt("INSERT INTO " DB_KERNEL_HINTS_REF_INDEX " (kernel_name, array_id, param_id, idx) VALUES (?, ?, ?, ?);");
	stmt.bind(0, kernelName);

	for(const auto& it : m_index) {
		stmt.reset();
		stmt.bind(1, it.first.getArrayId());
		stmt.bind(2, it.first.getParamId());
		stmt.bind(3, it.second);
		stmt.step();
	}
}

//-------------------------------------------------------------------
RefHints::Mapping RefHints::getMapping(const uint32_t arrayId, const uint32_t paramId, const uint32_t dim) const {
	Mapping result = {0};

	auto it = m_mappings.find(Identifier2(arrayId, paramId, dim));
	if(it == m_mappings.end())
		return result;

	result.arrayId = it->second.getArrayId();
	result.paramId = it->second.getParamId();
	result.dim     = it->second.getDim();
	result.index   = m_index.at(Identifier(it->second.getArrayId(), it->second.getParamId()));

	return result;
}

//-------------------------------------------------------------------
		}
	}
}