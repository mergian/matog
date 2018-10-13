#include "CountHints.h"
#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/macros.h>
#include <matog/Array.h>
#include <matog/Static.h>

namespace matog {
	namespace kernel {
		namespace hint {
//-------------------------------------------------------------------
void CountHints::load(const char* kernelName) {
	m_counts.clear();
	
	db::Stmt stmt("SELECT array_id, param_id, dim, value FROM " DB_KERNEL_HINTS_COUNTS " WHERE kernel_name = ?;");
	stmt.bind(0, kernelName);

	while(stmt.step()) {
		const uint32_t arrayId = stmt.get<uint32_t>(0);
		const uint32_t paramId = stmt.get<uint32_t>(1);
		const uint32_t dim     = stmt.get<uint32_t>(2);
		const uint64_t value   = stmt.get<uint64_t>(3);
		const Array& array     = Static::getArray(arrayId);

		const Identifier ident(arrayId, paramId);

		const uint32_t cnt = array.isCube() ? 1 : array.getRootDimCount();
		THROWIF(dim >= cnt);

		auto it = m_counts.find(ident);
		if(it == m_counts.end())
			it = m_counts.emplace(MAP_EMPLACE(ident, cnt, 0)).first;
		
		it->second[dim] = value;
	}
}

//-------------------------------------------------------------------
const uint64_t* CountHints::getCounts(const uint32_t arrayId, const uint32_t paramId) const {
	auto it = m_counts.find(Identifier(arrayId, paramId));

	if(it == m_counts.end())
		return 0;

	return &it->second[0];
}

//-------------------------------------------------------------------
		}
	}
}