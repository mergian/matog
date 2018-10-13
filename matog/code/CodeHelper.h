// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_CODE_CODEHELPER
#define __MATOG_CODE_CODEHELPER

#include "Template.h"
#include <matog/internal.h>
#include <matog/enums.h>

namespace matog {
	namespace code {
//-------------------------------------------------------------------
class CodeHelper {
public:
	struct Params {
		bool isRoot64Bit;
		bool withRoot;
		bool leadingSeparator;
		bool noRecursion;
		bool onlyVariableCounts;
		bool shortNames;
		bool isCube;
		bool lastNoChar;
		const char* tmpl;
		const char* root;
		const char* incl;

		inline Params(void) :
			isRoot64Bit			(false),
			withRoot			(false),
			leadingSeparator	(false),
			noRecursion			(false),
			onlyVariableCounts	(false),
			shortNames			(false),
			isCube				(false),
			lastNoChar			(false),
			tmpl				(0),
			root				(0),
			incl				(0)
		{}

		inline Params& setIsRoot64Bit		(const bool value = true)	{ isRoot64Bit		= value; return *this; }
		inline Params& setWithRoot			(const bool value = true)	{ withRoot			= value; return *this; }
		inline Params& setLeadingSeparator	(const bool value = true)	{ leadingSeparator	= value; return *this; }
		inline Params& setNoRecursion		(const bool value = true)	{ noRecursion		= value; return *this; }
		inline Params& setOnlyVariableCounts(const bool value = true)	{ onlyVariableCounts= value; return *this; }
		inline Params& setShortNames		(const bool value = true)	{ shortNames		= value; return *this; }
		inline Params& setIsCube			(const bool value = true)	{ isCube			= value; return *this; }
		inline Params& setLastNoChar		(const bool value = true)	{ lastNoChar		= value; return *this; }
		inline Params& setTmpl				(const char* value)			{ tmpl = value; return *this; }
		inline Params& setRoot				(const char* value)			{ root = value; return *this; }
		inline Params& setIncl				(const char* value)			{ incl = value; return *this; }
	};

	//---------------------------------------------------------------
	class Accessors {
	private:
		Dict* m_dict;
		bool m_isRoot64Bit;
		const char* m_root;

		void recursive		(Dict* preData, Dict* preAcc, const array::Field* current) const;
		void operatorAcc2Acc(Dict* dict, const array::Field* current) const;
		void rootOperators	(const array::Field* root) const;

	public:
		Accessors(Dict* dict, const array::Field* field, const Params params = Params());
	};

	//---------------------------------------------------------------
	class Name {
		Dict* m_dict;

	public:
		Name(Dict* dict, const array::Field* p, const Params params = Params());
		void add(const char* value);
		void add(const char value);
		void add(const uint32_t value);
		void add(const uint64_t value);
	};
	
	//---------------------------------------------------------------
	class Ident {
		Dict* m_dict;

	public:
		Ident(Dict* dict, const array::Field* p, const uint32_t dim, const Params params = Params());
		void add(const char* value);
		void add(const char value);
		void add(const uint32_t value);
		void add(const uint64_t value);
	};
	
	//---------------------------------------------------------------
	class FieldType {
		Dict* m_dict;

	public:
		FieldType(Dict* dict, const array::Field* field, const Params params = Params());
	};
	
	//---------------------------------------------------------------
	class Index {
		const bool m_isRoot64Bit;
		Dict* m_dict;

	public:
		Index(Dict* dict, const array::Field* p, const Params params = Params());
		void add(const char* value);
		void add(const char value);
		void add(const uint32_t value);
		void add(const uint64_t value);
	};
	
	//---------------------------------------------------------------
	class Values {
		Dict* m_dict;

	public:
		Values(Dict* dict, const array::Field* p, const Params params = Params());
		void add(const char* value);
		void add(const char value);
		void add(const uint32_t value);
		void add(const uint64_t value);
	};
	
	//---------------------------------------------------------------
	class Type {
		Dict* m_dict;

	public:
		Type(Dict* dict, const array::Field* p, const Params params = Params());
	};

	
	//---------------------------------------------------------------
private:
	const bool m_isRoot64Bit;

	static Dict*			incHelper(Dict* dict, const array::Field* p, const bool withRoot, const bool leadingSeparator, const char* tmpl, const char* incl, const char* sec, const char* field);

protected:
	static Dict*			inc						(Dict* dict, const char* name, const char* file);

	static void				set						(Dict* dict, const char* name, const uint64_t value);
	static void				set						(Dict* dict, const char* name, const uint32_t value);
	static void				set						(Dict* dict, const char* name, const char* value);
	static void				set						(Dict* dict, const char* name, const char value);
	           						              			                         
	static void				setGlobal				(Dict* dict, const char* name, const uint64_t value);
	static void				setGlobal				(Dict* dict, const char* name, const uint32_t value);
	static void				setGlobal				(Dict* dict, const char* name, const char* value);
	static void				setGlobal				(Dict* dict, const char* name, const char value);

public:
	inline bool isRoot64Bit(void) const { return m_isRoot64Bit; }
	CodeHelper(const bool isRoot64Bit);
};

//-------------------------------------------------------------------
	}
}

#endif