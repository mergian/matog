// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_DEGREE
#define __MATOG_DEGREE

#include <matog/util.h>
#include <matog/internal.h>
#include <set>
#include <list>
#include <map>

namespace matog {
//-------------------------------------------------------------------
class MATOG_INTERNAL_DLL Degree {
	struct MATOG_INTERNAL_DLL DefineData {
		const char** strings;
		uint32_t* values;
	};

	struct MATOG_INTERNAL_DLL DefineRangeData {
		double min;
		double max;
		double step;
	};

	uint32_t							m_id;
	uint32_t							m_arrayId;
	variant::Value						m_valueCount;
	const char*							m_name;
	degree::Type						m_type;
	const Array*						m_array;
	union {
		DefineData define;
		DefineRangeData range;
	}									m_data;
	bool								m_isShared;
	bool								m_isCompiler;
	bool								m_isFunction;
	bool								m_isRoot;

			void								free						(const bool all);
public:		
			void								set							(const db::Stmt& stmt);
												Degree						(void);
	virtual										~Degree						(void);
			bool								isLocal						(void) const;
			bool								isGlobal					(void) const;
	inline	bool								doesExist					(void) const { return m_id != 0; }
	inline	bool								operator==					(const Degree& other) const	{ return getId() == other.getId(); }
	inline	bool								operator<					(const Degree& other) const	{ return getId() < other.getId(); }
	inline	degree::Type						getType						(void) const { return m_type; }
	inline	uint32_t							getId						(void) const { return m_id; }
	inline	const char*							getName						(void) const { return m_name; }
			void								invalidate					(void);
	inline	bool								isArray						(void) const { return m_arrayId != 0; }
	inline	const Array*						getArray					(void) const { return m_array; }
	inline	bool								isRoot						(void) const { return m_isRoot; }

			bool								isGPULimited				(void) const;
			cuda::Architecture					getMinArch					(const variant::Value value) const;
			cuda::Architecture					getMaxArch					(const variant::Value value) const;
	inline	variant::Value						getValueCount				(void) const { return m_valueCount; };
			variant::Value						getValueCount				(const GPU& gpu) const;
			bool								isFunction					(void) const;
			bool								isCompiler					(void) const;
			bool								isShared					(void) const;
			bool								isIndependent				(void) const;
			const char*							printLong					(const variant::Value value) const;
			char								printShort					(const variant::Value value) const;
			uint32_t							getDefineValue				(const variant::Value value) const;
			const char*							getDefineString				(const variant::Value value) const;
			double								getRangeValue				(const variant::Value value) const;
			bool								isConfigType				(const variant::Type type) const;

	static	bool								dbDoesExist					(const uint32_t id);
			void								dbSelect					(const uint32_t id);
			void								dbSelect					(const char* name);
	static	void								dbSelectAll					(std::map<uint32_t, Degree>& map);
	static	void								dbDelete					(const uint32_t id);
	static	void								dbDelete					(const char* name);
	static	void								dbDeleteAll					(void);
	static	void								dbInsertDefine				(const char* name, const std::list< std::pair<util::String, uint32_t> >& options, const bool isShared);
	static	void								dbInsertRange				(const char* name, const double min, const double max, const double step, const bool isShared);

	friend class Array;
};

//-------------------------------------------------------------------
}

#endif
