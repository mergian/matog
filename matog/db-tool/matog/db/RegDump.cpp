#include "../db.h"
#include <matog/log.h>
#include <matog/shared.h>
#include <unordered_set>

namespace matog {
	namespace db {
//-------------------------------------------------------------------
RegDump::RegDump(int argc, char** argv) {
	CHECK(MatogResult::NOT_IMPLEMENTED);
	/*
	CHECK(Static::init());
	
	DB& db = DB::getInstance();

	sqlite3_stmt* stmt;
	CHECK(db.prepare(stmt, "SELECT static_smem, const_mem, local_mem, used_registers FROM " DB_CUBIN_KERNEL_META " WHERE matog_hash = ?"));

	std::unordered_set<uint32_t> matogHashes;

	for(const auto& modulePair: Static::getModules()) {
		const Module& module = modulePair.second;

		for(uint32_t i = 0; i < module.getConfigCount(); i++) {
			if(matogHashes.emplace(hashes[i].getMatogHash()).second) {
				CHECK(sqlite3_reset(stmt));
				CHECK(sqlite3_bind_int(stmt, 1, hashes[i].getMatogHash()));
			
				sqliteResult res = sqlite3_step(stmt);
				CHECK(res);

				if(res == SQLITE_ROW) {
					LL_WARN();
					printf("%s ", module.getName().c_str());
					variants.printShort(hashes[i]);
					printf(" %u %u %u %u", 
						sqlite3_column_int(stmt, 0),
						sqlite3_column_int(stmt, 1),
						sqlite3_column_int(stmt, 2),
						sqlite3_column_int(stmt, 3));
					LL_END();
				}
			}
		}
	}

	CHECK(db.finalize(stmt));
	*/
}

//-------------------------------------------------------------------
	}
}