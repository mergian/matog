// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/analyze/graph/Graph.h>
#include <matog/analyze/training/Data.h>
#include <matog/log.h>
#include <matog/db/tables.h>
#include <matog/db/Stmt.h>
#include <matog/Options.h>
#include <matog/macros.h>
#include <matog/variant/Type.h>
#include <cfloat>
#include <cmath>
#include "Training.h"

using namespace matog::db;

namespace matog {
	namespace analyze {
		namespace training {
//-------------------------------------------------------------------
Data::Data(const List& list, const Variant& variant, const bool isGlobal) : 
	m_list					(list),
	m_variant				(variant),
	m_isGlobal				(isGlobal),
	m_input					(0), 
	m_output				(0), 
	m_decisionIds			(0),
	m_dim					(0),
	m_offsets				(0),
	m_rows					((uint32_t)list.size()),
	m_cols					(0),
	m_stmt					(
		"SELECT "
		"decision_id, decision_offset, dim, value "
		"FROM "
		DB_EVENT_DATA
		" WHERE event_id = ? "
		" ORDER BY decision_id ASC, decision_offset ASC, dim ASC;"
	)
{
	// load columns and data
	calcCols();
	loadData();

	// reduce
	reduceCols();
	reduceRows(true);
	reduceRows(false);
	checkForOnlyOneCategory();

	// print
	if(Training::isLog())
		print();
}

//-------------------------------------------------------------------
void Data::checkForOnlyOneCategory(void) {
	// this is a special case. If all categories are the same, after the reduction,
	// then the SVM construction will fail.
	if(getRows() == 1)
		return;
	
	std::set<int32_t> values;
	uint32_t newCount = 0;
	for(uint32_t i = 0; i < getRows(); i++) {
		values.emplace(m_output[i]);
		newCount += m_count[i];
	}

	if(values.size() == 1) {
		m_count[0] = newCount;
		m_rows = 1;
	}
}

//-------------------------------------------------------------------
void Data::init(void) {
	m_input				= NEW_A(float, getRows() * getCols() + getRows() + 3 * getCols() + getRows());
	m_output			= (int32_t*) (m_input + getRows() * getCols());
	m_decisionIds		= (uint32_t*)(m_output + getRows());
	m_dim				= (uint32_t*)(m_decisionIds + getCols());
	m_offsets			= (uint32_t*)(m_dim + getCols());
	m_count				= (uint32_t*)(m_offsets + getCols());

	memset(m_input,				0,		sizeof(float)		* getRows() * getCols());
	memset(m_output,			0,		sizeof(int32_t)		* getRows());
	memset(m_decisionIds,		0xFF,	sizeof(uint32_t)	* getCols());
	memset(m_dim,				0xFF,	sizeof(uint32_t)	* getCols());
	memset(m_offsets,			0xFF,	sizeof(uint32_t)	* getCols());
	memset(m_count,				0,		sizeof(uint32_t)	* getRows());
}

//-------------------------------------------------------------------
Data::~Data(void) {
	if(m_input)
		FREE_A(m_input);
}

//-------------------------------------------------------------------
void Data::loadData(void) {
	uint32_t row = 0;

	for(auto& it : m_list) {
		const uint32_t eventId					= it.first;
		const variant::CompressedHash config	= it.second;

		m_stmt.reset();
		m_stmt.bind(0, eventId);

		while(m_stmt.step()) {
			const uint32_t col = m_colIndex[m_stmt];
			m_input[row * getCols() + col] = m_stmt.get<float>(3);
		}

		// store config
		m_output[row] = config;
		m_count [row] = 1;

		// increase
		row++;
	}

	// no longer needed
	m_colIndex.clear();
}

//-------------------------------------------------------------------
void Data::removeCols(const std::set<uint32_t>& keepCols) {
	uint32_t newCols = (uint32_t)keepCols.size();

	if(newCols != getCols()) {
		// reduce arrayTypeIds
		uint32_t newCol = 0;
		for(const uint32_t col : keepCols) {
			m_decisionIds	[newCol]	= m_decisionIds	[col];
			m_dim			[newCol]	= m_dim			[col];
			m_offsets		[newCol]	= m_offsets		[col];
			newCol++;
		}

		// reduce data
		for(uint32_t row = 0; row < getRows(); row++) {
			newCol = 0;
			for(const uint32_t col : keepCols) {
				m_input[row * newCols + newCol] = m_input[row * getCols() + col];
				newCol++;
			}
		}
		
		m_cols = newCols;
	}
}
//-------------------------------------------------------------------
void Data::removeRows(const std::set<uint32_t>& keepRows) {
	uint32_t newRows = (uint32_t)keepRows.size();

	if(newRows != getRows()) {
		// reduce arrayTypeIds
		uint32_t newRow = 0;
		for(const uint32_t row : keepRows) {
			m_output[newRow] = m_output[row];
			m_count [newRow] = m_count[row];
			newRow++;
		}

		// reduce data
		newRow = 0;
		for(const uint32_t row : keepRows) {
			for(uint32_t col = 0; col < getCols(); col++)
				m_input[newRow * getCols() + col] = m_input[row * getCols() + col];

			newRow++;
		}
		
		m_rows = newRows;
	}
}

//-------------------------------------------------------------------
void Data::reduceCols_InvalidMetaIDs(void) {
	std::set<uint32_t> keepCols;

	for(uint32_t col = 0; col < getCols(); col++) {
		if(m_decisionIds[col] != UINT_MAX)
			keepCols.emplace(col);		
	}

	// remove
	removeCols(keepCols);
}

//-------------------------------------------------------------------
void Data::reduceCols_EqualValues(void) {
	std::set<uint32_t> keepCols;

	for(uint32_t col = 0; col < getCols(); col++) {
		std::set<float> values;

		for(uint32_t row = 0; row < getRows(); row++)
			values.emplace(m_input[row * getCols() + col]);
		
		// remove col if only the same 
		if(values.size() > 1)
			keepCols.emplace(col);		
	}

	// remove
	removeCols(keepCols);
}

//-------------------------------------------------------------------
void Data::reduceCols_LinearDependent(void) {
	std::set<uint32_t> keepCols;

	// not necessary if there is only 1 row or less than 2 cols
	if(getRows() == 1 || getCols() < 2)
		return;

	keepCols.emplace(getCols() - 1);

	// iterate left col
	for(uint32_t left = 0; left < (getCols() - 1); left++) {
		// init
		keepCols.emplace(left);

		// iterate all cols, right of left
		for(uint32_t right = left + 1; right < getCols(); right++) {
			bool differs = false;

			// first row
			const float l0 = m_input[left  + 0 * getCols()];
			const float r0 = m_input[right + 0 * getCols()];
			const float x0 = l0 == 0 ? 0 : (r0 == 0 ? FLT_MAX : l0 / r0);
			
			// iterate all remaining rows
			for(uint32_t row = 1; row < getRows(); row++) {
				// get values
				const float ln = m_input[left  + row * getCols()];
				const float rn = m_input[right + row * getCols()];
				const float xn = ln == 0 ? 0 : (rn == 0 ? FLT_MAX : ln / rn);

				// if any of the vector values differes significantly, they are linear independent
				if(std::fabs(x0 - xn) > 0.001f) {
					differs = true;
					break;
				}
			}

			// remove col if does not differ
			if(!differs) {
				keepCols.erase(left);
				break;
			}
		}
	}

	// remove
	removeCols(keepCols);
}

//-------------------------------------------------------------------
void Data::reduceCols(void) {
	reduceCols_InvalidMetaIDs();
	reduceCols_EqualValues();
	reduceCols_LinearDependent();
}

//-------------------------------------------------------------------
void Data::reduceRows(const bool mode) {
	std::set<uint32_t> keepRows;

	for(uint32_t row = 0; row < getRows(); row++)
		keepRows.emplace(row);

	for(uint32_t rowA = 0; rowA < getRows(); rowA++) {
		for(uint32_t rowB = rowA + 1; rowB < getRows(); rowB++) {
			bool allMatch = true;
			
			for(uint32_t col = 0; col < getCols(); col++) {
				if(m_input[rowA * getCols() + col] != m_input[rowB * getCols() + col]) {
					allMatch = false;
					break;
				}
			}

			// Mode 1: combine rows
			if(mode && allMatch && m_output[rowA] == m_output[rowB]) {
				m_count[rowA] += m_count[rowB];
				keepRows.erase(rowB);
			}

			// Mode 2: use with higher count
			else if(!mode && allMatch && m_output[rowA] != m_output[rowB]) {
				if(m_count[rowA] >= m_count[rowB]) {
					m_count[rowA] += m_count[rowB];
					keepRows.erase(rowB);
				} else {
					m_count[rowB] += m_count[rowA];
					keepRows.erase(rowA);
				}
			}
		}
	}

	// remove
	removeRows(keepRows);
}

//-------------------------------------------------------------------
void Data::print(void) const {
	L_DEBUG_
	printf("Meta: ");
	for(uint32_t col = 0; col < getCols(); col++) {
		if(m_decisionIds[col] != UINT_MAX)
			printf("%u:%u:%u ", m_decisionIds[col], m_offsets[col], m_dim[col]);
		else
			printf("X:X:X:X ");
	}
	L_END

	for(uint32_t row = 0; row < getRows(); row++) {
		const variant::Hash uncompressed = m_variant.uncompress(m_output[row], m_isGlobal ? variant::Type::Global : variant::Type::Local);
		
		L_DEBUG_
		printf("%ux %i/%llu --> ", m_count[row], m_output[row], (unsigned long long)uncompressed);
		for(uint32_t col = 0; col < getCols(); col++) {
			if(m_decisionIds[col] != UINT_MAX)
				printf("%.0f ", m_input[row * getCols() + col]);
			else
				printf("_ ");
		}
		m_variant.print(uncompressed);
		L_END
	}
}

//-------------------------------------------------------------------
void Data::calcCols(void) {
	// count cols and build index
	std::set<Index> set;

	for(const auto& it : m_list) {
		m_stmt.reset();
		m_stmt.bind(0, it.first);
		
		while(m_stmt.step())
			set.emplace(m_stmt);
	}

	// set size and create index
	m_cols = (uint32_t)set.size();

	uint32_t i = 0;
	for(const Index& index : set)
		m_colIndex.emplace(MAP_EMPLACE(index, i++));

	// allocate memory
	init();

	// store headers
	for(const auto& it : m_list) {
		m_stmt.reset();
		m_stmt.bind(0, it.first);

		while(m_stmt.step()) {
			const uint32_t col = m_colIndex[m_stmt];

			m_decisionIds[col]	= m_stmt.get<uint32_t>(0);
			m_offsets[col]		= m_stmt.get<uint32_t>(1);
			m_dim[col]			= m_stmt.get<uint32_t>(2);
		}
	}
}

//-------------------------------------------------------------------
		}
	}
}