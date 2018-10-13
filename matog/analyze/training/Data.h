// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYZE_TRAINING_DATA_H
#define __MATOG_ANALYZE_TRAINING_DATA_H

#include <cstdint>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <matog/db/Stmt.h>
#include <matog/Variant.h>

namespace matog {
	namespace analyze {
		namespace training {
//-------------------------------------------------------------------
class Data {
public:
	typedef std::list< std::pair<uint32_t, uint32_t> > List;

private:
	struct Index {
		uint32_t decisionId;
		uint32_t offset;
		uint32_t dim;

		Index(const db::Stmt& stmt);
		bool operator<(const Index& other) const;
	};

	const List&					m_list;
	const Variant&				m_variant;
	const bool					m_isGlobal;
	std::map<Index, uint32_t>	m_colIndex;
	float*						m_input;
	int32_t*					m_output;
	uint32_t*					m_decisionIds;
	uint32_t*					m_dim;
	uint32_t*					m_offsets;
	uint32_t*					m_count;

	uint32_t					m_rows;
	uint32_t					m_cols;

	db::Stmt			m_stmt;

	void init						(void);
	void removeCols					(const std::set<uint32_t>& keepCols);
	void removeRows					(const std::set<uint32_t>& keepRows);
	void loadData					(void);
	void reduceCols					(void);
	void reduceCols_InvalidMetaIDs	(void);
	void reduceCols_EqualValues		(void);
	void reduceCols_LinearDependent	(void);
	void reduceRows					(const bool mode);
	void checkForOnlyOneCategory	(void);
	void calcCols					(void);

public:
			Data							(const List& list, const Variant& variant, const bool isGlobal);
			~Data							(void);
			void		print				(void) const;

	inline	uint32_t	getRows				(void) const { return m_rows; }
	inline	uint32_t	getCols				(void) const { return m_cols; }

	inline	float*		getInput			(void) const { return m_input; }
	inline	int32_t*	getOutput			(void) const { return m_output; }
	inline	uint32_t*	getDecisionIds		(void) const { return m_decisionIds; }
	inline	uint32_t*	getDims				(void) const { return m_dim; }
	inline	uint32_t*	getOffsets			(void) const { return m_offsets; }
	inline	uint32_t*	getCounts			(void) const { return m_count; }
};

//-------------------------------------------------------------------
		}
	}
}

#endif