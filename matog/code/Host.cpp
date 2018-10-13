// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Host.h"
#include <matog/log.h>
#include <matog/array/Field.h>
#include <matog/array/field/Type.h>
#include <matog/Static.h>
#include <matog/enums.h>

using matog::array::Field;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
Host::Host(const Field& root) : CodeTemplate(root, true), m_array(Static::getArray(m_root.getGlobalId())) {
	// init global
	init();

	// init getter / setter
	initFlatItems();

	// init unions & structs
	initUnion(getDict(), m_root, false);

	// init accessors
	Accessors acc(getDict(), &m_root, Params().setIsRoot64Bit().setRoot("Host"));

	//> Methods
	method_initPtr();
	method_copy();
	method_compare();
	method_getCopySize();
	method_calcSize();
	method_calcSizeByElements();
	method_dyn_0();

	// generate
	generateFile("host_header", "", "h");
	generateFile("host_host", "_host", "cpp");
	generateFile("host_device", "_device", "cpp");
	generateFile("host_dyn", "_dyn", "cpp");
}

//-------------------------------------------------------------------
void Host::initVariants(const Field& field) const {
	const Variant& variants = m_array.getVariant();

	if(field.isLayoutStruct()) {
		const Degree* degree = &Static::getDegree(field.getGlobalLayoutId());
		Dict* getter = inc(getDict(), "FORMAT", "host_header_get_layout");
		Name(getter, &field, Params().setLeadingSeparator());
		set(getter, "MUL", variants.mul(degree, 0));
		set(getter, "MOD", (uint32_t)degree->getValueCount());
	}

	if(field.isMultiDimensional()) {
		const Degree* degree = &Static::getDegree(field.getGlobalTranspositionId());
		Dict* getter = inc(getDict(), "FORMAT", "host_header_get_transposition");
		Name	(getter, &field, Params().setLeadingSeparator());
		Index	(getter, &field, Params().setIsRoot64Bit().setNoRecursion().setShortNames());
		Values	(getter, &field, Params().setNoRecursion().setShortNames());
		set(getter, "MUL", variants.mul(degree, 0));
		set(getter, "MOD", (uint32_t)degree->getValueCount());

		for(uint32_t i = 0; i < field.getDimCount(); i++) {
			const char letter = (char)('A' + i);

			Dict* dims = getter->AddSectionDictionary("COUNTS");
			set(dims, "CHAR", letter);
			set(dims, "DIM", i);

			Name(dims, &field, Params().setLeadingSeparator());
		}

		initTranspositions(getter, field, degree);
	}
}

//-------------------------------------------------------------------
void Host::initTranspositions(Dict* dict, const Field& field, const Degree* degree) const {
	uint32_t value = 0;
	for(const char* ptr : field.getIndex()) {
		Dict* sub = dict->AddSectionDictionary("TRANSPOSITIONS");
		set(sub, "VALUE", value++);
		set(sub, "CODE", ptr);
	}
}

//-------------------------------------------------------------------
void Host::initFlatItems(void) const {
	auto addGetterSetter = [&](const char* tmpl, const Field& field) {
		Dict* dict = inc(getDict(), "GETTER_SETTER", tmpl);
		Index	(dict, &field, Params().setIsRoot64Bit());
		Values	(dict, &field);
		Name	(dict, &field, Params().setLeadingSeparator());
		return dict;
	};

	auto addPtr = [&](const Field& field) {
		// init dictionary for pointer
		Dict* dict = inc(getDict(), "PTR", "ptr");
		set(dict, "FIELD", field.getName());

		// init basic stuff
		Index	(dict, &field, Params().setIsRoot64Bit());
		Values	(dict, &field);
		Name	(dict, &field, Params().setLeadingSeparator());
		set(dict, "TYPE", field.getTypeName());

		// init idx and layouts
		typedef std::list<const Field*> Reverse;
		Reverse reverse;

		bool isLayouts = false;
		const Field* current = &field;
		while(current != 0) {
			// dimensional
			if(current->isDimensional()) {
				Dict* incl = inc(dict, "IDX",  current->isMultiDimensional() ? "ptr_idx" : "ptr_idx2");
				Values	(incl, current, Params().setNoRecursion());
				Name	(incl, current, Params().setLeadingSeparator());
			}

			// layouts
			if(current->isLayoutStruct()) {
				Dict* incl = inc(dict, "LAYOUT", "ptr_layout");
				Name(incl, current, Params().setLeadingSeparator());
				isLayouts = true;
			}

			// ptr
			if(!current->isRoot())
				reverse.emplace_back(current);

			// next
			current = current->getParent();
		}

		// reverse
		typedef std::list< std::pair<const Field*, matog::Layout> > Pairs;
		Pairs pairs;

		std::function<void (Reverse::const_reverse_iterator, const bool)> genPtr = [&](Reverse::const_reverse_iterator it, const bool isConst) {
			if(it == reverse.rend()) {
				// init condition
				Dict* cond = inc(dict, isConst ? "CONST_PTR" : "PTR", isLayouts ? "ptr_cond" : "ptr_return");

				// is there only a root element?
				if(reverse.empty()) {
					inc(dict, isConst ? "CONST_PTR" : "PTR", !isConst ? "ptr_RW" : "ptr_RO");
				} else {
					// init ptr
					for(const auto& pair : pairs) {
						const Field* current = pair.first;
						const Field* parent = current->getParent();
						const matog::Layout layout = pair.second;

						// COND
						if(parent->isLayoutStruct()) {
							Dict* cvalue = cond->AddSectionDictionary("COND");
							Name(cvalue, parent, Params().setLeadingSeparator().setIncl("PARENT_NAME"));
							set(cvalue, "VALUE", (uint32_t)layout);
						}

						// PTR
						const char* tmpl = 0;

						if(parent->isRoot()) {
							if(!isConst)	tmpl = !parent->isLayoutStruct() ? "ptr_RW" : layout == matog::Layout::SOA ? "ptr_RW_soa" : layout == matog::Layout::AOS ? "ptr_RW_aos" : "ptr_RW_aosoa";
							else			tmpl = !parent->isLayoutStruct() ? "ptr_RO" : layout == matog::Layout::SOA ? "ptr_RO_soa" : layout == matog::Layout::AOS ? "ptr_RO_aos" : "ptr_RO_aosoa";
						} else if(parent->isLayoutStruct()) {
							tmpl = layout == matog::Layout::SOA ? "ptr_soa" : "ptr_aos";
						} else if(parent->isStruct()) {
							tmpl = parent->isDimensional() ? "ptr_struct" : "ptr_field";
						} else if(parent->isDimensional()) {
							L_WARN("TODO");
						}

						if(tmpl) {
							Dict* incl = inc(dict, isConst ? "CONST_PTR" : "PTR", tmpl);
							Name(incl, parent, Params().setLeadingSeparator().setIncl("PARENT_NAME"));
							set(incl, "FIELD", current->getName());
						}

						if(current->isDimensional() && !current->hasChildren()) {
							Dict* incl = inc(dict, isConst ? "CONST_PTR" : "PTR", "ptr_dimensional2");
							Name(incl, current, Params().setLeadingSeparator());
						}
					}
				}

				// init end
				inc(dict, isConst ? "CONST_PTR" : "PTR", "ptr_end");
			} else {
				auto pit = it;
				it++;

				const Field* current = *pit;
				const Field* parent = current->getParent();

				const uint32_t cnt = (parent->isStruct() && parent->getChildren().size() > 1) ? (parent->isAOSOA() ? 3 : 2) : 1;
				for(uint32_t layout = 0; layout < cnt; layout++) {
					pairs.emplace_back(std::make_pair(current, (matog::Layout)layout));
					genPtr(it, isConst);
					pairs.pop_back();
				}
			}
		};

		genPtr(reverse.rbegin(), false);
		genPtr(reverse.rbegin(), true);
		
		if(isLayouts)
			dict->ShowSection("IS_THROW");

		return dict;
	};

	// recurse all fields
	std::function<void (const Field&)> rFields = [&](const Field& field) {
		// init variants
		initVariants(field);

		// init counts
		if(field.isDimensional()) {
			initCounts(field, "HOST_COUNTS", true);
			initCounts(field, "DYN_COUNTS", false);
			initCounts(field, "DEVICE_COUNTS", m_root.getExternalIndexType() == array::field::Type::U64);

			initElementCounts(field, "HOST_ELEMENT_COUNTS", true);
			initElementCounts(field, "DYN_ELEMENT_COUNTS", false);
			initElementCounts(field, "DEVICE_ELEMENT_COUNTS", m_root.getInternalIndexType() == array::field::Type::U64);
		}

		// only structs
		if(field.isStruct()) {
			// vector getter
			if(field.isVector()) {
				Dict* dict = addGetterSetter("getter_vector", field);
				set(inc(dict, "TYPE", "type")->AddSectionDictionary("TYPE"), "TYPE", field.getVTypeName());

				set(dict, "AOS_FIELD", field.getChildren().front().getName());

				static const char chars[] = {'x', 'y', 'z', 'w'};

				uint32_t i = 0;
				for(const Field& sub : field.getChildren()) {
					Dict* fields = dict->AddSectionDictionary("FIELDS");
					set(fields, "FIELD", sub.getName());
					set(fields, "VFIELD", chars[i++]);
				}
			}
		} else if(!field.hasChildren()) {
			set(inc(addPtr(field), "TYPE", "type")->AddSectionDictionary("TYPE"), "TYPE", field.getTypeName());
			set(inc(addGetterSetter("getter_field", field), "TYPE", "type")->AddSectionDictionary("TYPE"), "TYPE", field.getTypeName());
		}	

		// for all children
		for(const Field& sub : field.getChildren())
			rFields(sub);
	};

	rFields(m_root);
}

//-------------------------------------------------------------------
void Host::initUnion(Dict* dict, const Field& current, const bool printParentCount) const {
	// is root?
	if(current.isRoot()) {
		// first iterate all children and then create pointer for all of them
		for(const Field& child : current.getChildren())
			initUnion(dict, child, true);

		// is struct?
		if(current.isLayoutStruct()) {
			Dict* incl = inc(dict, "STRUCT", "struct");

			for(const Field& child : current.getChildren()) {
				Dict* sub = incl->AddSectionDictionary("CHILDREN");
				set(sub, "NAME", child.getName());
				
				if(child.isStruct() || child.isDimensional()) {
					set(sub, "PRE", "__");
					set(sub, "TYPE", child.getName());
				} else {
					set(sub, "TYPE", child.getTypeName());
				}
			}
		}

		// data pointer
		set(inc(dict, "HOST_POINTER", m_root.isLayoutStruct() ? "data_pointer_struct" : "data_pointer_array"), "TYPE", m_root.getTypeName());
		initRootFields(inc(dict, "DEVICE_POINTER", m_root.isLayoutStruct() ? "host_device_pointer_struct" : "host_device_pointer_array"));
	} 
	// is child?
	else {
		const Field& parent = *current.getParent();

		// not needed if this is part of the root struct and neither dimensional nor struct
		if(parent.isRoot() && !current.isStruct() && !current.isDimensional())
			return;
		
		// begin
		if(current.isLayoutStruct()) {
			// begin
			set(inc(dict, "UNION", parent.isRoot() ? "union_union" : "union_uunion"), "NAME", current.getName());

			// aos
			inc(dict, "UNION", "union_ustruct");

			for(const Field& child : current.getChildren())
				initUnion(dict, child, false);

			set(inc(dict, "UNION", "union_ustruct_end"), "NAME", "__aos");
			set(inc(dict, "UNION", "union_count"), "COUNT", current.getElementCount());
			inc(dict, "UNION", "union_end");

			// soa
			inc(dict, "UNION", "union_ustruct");

			for(const Field& child : current.getChildren())
				initUnion(dict, child, true);

			set(inc(dict, "UNION", "union_ustruct_end"), "NAME", "__soa");
			inc(dict, "UNION", "union_end");

			// end
			set(inc(dict, "UNION", parent.isRoot() ? "union_union_end" : "union_uunion_end"), "NAME", current.getName());
		}
		else if(current.isStruct()) {
			// begin
			set(inc(dict, "UNION", parent.isRoot() ? "union_struct" : "union_ustruct"), "NAME", current.getName());

			// inner
			for(const Field& child : current.getChildren())
				initUnion(dict, child, true);

			// end
			set(inc(dict, "UNION", parent.isRoot() ? "union_struct_end" : "union_ustruct_end"), "NAME", current.getName());
		} else if(current.isDimensional()) {
			Dict* incl = inc(dict, "UNION", "union_type");
			set(incl, "NAME", current.getName());
			set(incl, "TYPE", current.getTypeName());
			set(incl, "COUNT", current.getElementCount());
		} else {
			Dict* incl = inc(dict, "UNION", "union_field");
			set(incl, "NAME", current.getName());
			set(incl, "TYPE", current.getTypeName());
		}

		// end
		if(!parent.isRoot()) {
			if(printParentCount && parent.isDimensional())
				set(inc(dict, "UNION", "union_count"), "COUNT", parent.getElementCount());

			if(current.isDimensional() && !current.hasChildren())
				set(inc(dict, "UNION", "union_count"), "COUNT", current.getElementCount());
		}

		inc(dict, "UNION", "union_end");
	}
}

//-------------------------------------------------------------------
void Host::init(void) const {
	Dict* dict = getDict();
	setGlobal(dict, "GLOBAL_ID", m_root.getGlobalId());
	setGlobal(dict, "SHARED_ID", m_root.getSharedId());
	setGlobal(dict, "DYN_ID",    m_root.getDynId());
	setGlobal(dict, "GLOBAL_NAME", m_root.getName());
	setGlobal(dict, "INDEX_TYPE", "uint64_t");
	setGlobal(dict, "INTERNAL_INDEX_TYPE", "uint64_t");
	setGlobal(dict, "DEVICE_INDEX_TYPE", m_root.getExternalIndexType() == array::field::Type::U32 ? "uint32_t" : "uint64_t");
	setGlobal(dict, "DIM_COUNT", m_root.getCounts().size());

	// variable count
	const uint32_t dynCount = m_root.getVDimCount() == 0 ? 0 : m_root.isCube() ? 1 : m_root.getVDimCount();
	if(dynCount > 0) {
		setGlobal(dict, "DIM_COUNT_DYN", dynCount);
		dict->ShowSection("IS_DYN_COUNT");
	} else {
		dict->ShowSection("IS_NO_DYN_COUNT");
	}
	
	// is struct
	if(m_root.hasChildren())
		dict->ShowSection("IS_STRUCT");

	// dyn alignment
	set(dict, "DYN_ALIGNMENT", m_root.getSize());

	// index
	Index index(dict, &m_root,		Params().setOnlyVariableCounts().setLeadingSeparator().setIsRoot64Bit().setIsCube());
	Values values(dict, &m_root,	Params().setOnlyVariableCounts().setIsCube());

	Index dynIndex(dict, &m_root,	Params().setOnlyVariableCounts().setLeadingSeparator().setIncl("DYN_INDEX").setIsCube());
	Values dynValues(dict, &m_root,	Params().setOnlyVariableCounts().setIncl("DYN_VALUES").setIsCube());

	Index devIndex(dict, &m_root,	Params().setOnlyVariableCounts().setLeadingSeparator().setIsRoot64Bit(m_root.getExternalIndexType() == array::field::Type::U64).setIncl("DEVICE_INDEX").setIsCube());
	Values devValues(dict, &m_root,	Params().setOnlyVariableCounts().setIncl("DEVICE_VALUES").setIsCube());

	// counts
	{
		uint32_t dim = 0;
		char cdim = 'A';
		for(const uint32_t value : m_root.getCounts()) {
			if(value == 0) {
				Dict* dict = getDict()->AddSectionDictionary("COUNTS");
				set(dict, "DIM", dim++);
				set(dict, "CHAR", cdim);

				if(m_root.isCube())
					break;
			}

			cdim++;
		}
	}

	// max size
	setGlobal(dict, "MAX_SIZE", m_root.getSize());
}

//-------------------------------------------------------------------
void Host::method_initPtr(void) const {
	initRootFields(inc(getDict(), "HOST_INIT_PTR", m_root.isLayoutStruct()   ? "host_host_initPtr_struct"   : "host_host_initPtr_array"));
	initRootFields(inc(getDict(), "DEVICE_INIT_PTR", m_root.isLayoutStruct() ? "host_device_initPtr_struct" : "host_device_initPtr_array"));
}

//-------------------------------------------------------------------
void Host::method_copy(void) const {
	std::function<void (Dict*, const Field*)> func = [&func, this](Dict* dict, const Field* current) {
		if(current->isDimensional()) {
			for(uint32_t dim = 0; dim < current->getDimCount(); dim++) {
				dict = inc(dict, "COPY", "copy_dim");
				
				// index_type, dim and name
				set(dict, "INDEX_TYPE", current->isRoot() && isRoot64Bit() ? "uint64_t" : "uint32_t");
				if(current->getDimCount() > 1) set(dict, "DIM", dim);
				Name(dict, current, Params().setLeadingSeparator());

				// index
				Name(dict, current, Params().setIncl("INDEX")).add((char)('A' + dim));
			}
		}

		if(current->isStruct()) {
			for(const Field& child : current->getChildren())
				func(dict, &child);
		} else {
			Dict* incl = inc(dict, "COPY", "copy_field");
			Name 	(incl, current, Params().setLeadingSeparator());
			Index 	(incl, current, Params().setIsRoot64Bit());
			Values	(incl, current);
		}
	};

	func(getDict(), &m_root);
}

//-------------------------------------------------------------------
void Host::method_compare(void) const {
	std::function<void (Dict*, const Field*)> func = [&func, this](Dict* dict, const Field* current) {
		if(current->isDimensional()) {
			for(uint32_t dim = 0; dim < current->getDimCount(); dim++) {
				dict = inc(dict, "COMPARE", "compare_dim");

				// index_type, dim and name
				set(dict, "INDEX_TYPE", current->isRoot() && isRoot64Bit() ? "uint64_t" : "uint32_t");
				if(current->getDimCount() > 1) set(dict, "DIM", dim);
				Name(dict, current, Params().setLeadingSeparator());

				// index
				Name(dict, current, Params().setIncl("INDEX")).add((char)('A' + dim));
			}
		}

		if(current->isStruct()) {
			for(const Field& child : current->getChildren())
				func(dict, &child);
		} else {
			Dict* incl = inc(dict, "COMPARE", current->isFloat() ? "compare_float" : "compare_int");
			Name 	(incl, current, Params().setLeadingSeparator());
			Index 	(incl, current, Params().setIsRoot64Bit());
			Values 	(incl, current);
		}
	};

	func(getDict(), &m_root);
}

//-------------------------------------------------------------------
void Host::initRootFields(Dict* dict) const {
	if(m_root.hasChildren()) {
		for(const Field& field : m_root.getChildren()) {
			Dict* f = dict->AddSectionDictionary("FIELDS");
			set(f, "FIELD", field.getName());

			if(field.isStruct() || field.isDimensional()) {
				f->ShowSection("IS_FIELD");
			} else {
				f->ShowSection("IS_TYPE");
				set(f, "TYPE", field.getTypeName());
			}
		}
	} else {
		set(dict, "TYPE", m_root.getTypeName());
	}
}

//-------------------------------------------------------------------
void Host::method_getCopySize(void) const {
	initRootFields(inc(getDict(), "GET_COPY_SIZE", m_root.isLayoutStruct() ? "getCopySize_struct" : "getCopySize_array"));
}

//-------------------------------------------------------------------
void Host::method_dyn_0(void) const {
	initRootFields(inc(getDict(), "DYN_0", m_root.isLayoutStruct() ? "host_dyn_0_struct" : "host_dyn_0_array"));
}

//-------------------------------------------------------------------
void Host::method_calcSize(void) const {
	Dict* dict = inc(getDict(), "CALC_SIZE", m_root.isCustomCount() ? "host_host_calcSize_custom" : "host_host_calcSize_default");

	char cdim = 'A';
	for(const uint32_t count : m_root.getCounts()) {
		Dict* sec = dict->AddSectionDictionary("DIMS");
		if(count == 0)	set(sec, "DIM", cdim);
		else			set(sec, "DIM", count);

		if(!m_root.isCube())
			cdim++;
	}
	
	if(m_root.isCustomCount())
		set(dict, "EQUATION", m_root.getCustomCount());
}

//-------------------------------------------------------------------
void Host::method_calcSizeByElements(void) const {
	Dict* dict = inc(getDict(), "CALC_SIZE_BY_ELEMENTS", m_root.isLayoutStruct() ? "host_host_calcSizeByElements_struct" : "host_host_calcSizeByElements_array");

	if(m_root.isLayoutStruct()) {
		initRootFields(dict);
	} else {
		set(dict, "TYPE", m_root.getTypeName());
	}
}

//-------------------------------------------------------------------
	}
}