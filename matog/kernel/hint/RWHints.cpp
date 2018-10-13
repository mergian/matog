#include "RWHints.h"
#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/macros.h>

namespace matog {
	namespace kernel {
		namespace hint {
//-------------------------------------------------------------------
void RWHints::load(const char* kernelName) {
	m_modes.clear();

	db::Stmt stmt("SELECT array_id, param_id, mode FROM " DB_KERNEL_HINTS_RW " WHERE kernel_name = ?;");
	stmt.bind(0, kernelName);

	while(stmt.step())
		m_modes.emplace(MAP_EMPLACE(Identifier(stmt.get<uint32_t>(0), stmt.get<uint32_t>(1)), stmt.get<RWMode>(2)));
}

//-------------------------------------------------------------------
RWMode RWHints::getMode(const uint32_t arrayId, const uint32_t paramId) const {
	auto it = m_modes.find(Identifier(arrayId, paramId));

	if(it == m_modes.end())
		return RWMode::READ_AND_WRITE;
	return it->second;
}

//-------------------------------------------------------------------
		}
	}
}