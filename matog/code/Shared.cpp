// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Shared.h"
#include <matog/log.h>
#include <matog/array/Field.h>
#include <matog/array/field/Type.h>
#include <matog/Static.h>
#include <matog/enums.h>

using matog::array::Field;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
Shared::Shared(const Field& root) : CodeTemplate(root, true), m_array(Static::getArray(m_root.getSharedId())) {
	// init global
	init();

	// init getter / setter
	initFlatItems();

	// init unions & structs
	initUnion(getDict(), m_root, false);

	// init accessors
	Accessors acc(getDict(), &m_root, Params().setRoot(root.getSharedName()));

	// generate
	generateFile("device_shared", "Shared", "cu");
}

//-------------------------------------------------------------------
void Shared::initVariants(const Field& field) const {
	const Variant& variants = m_array.getVariant();

	if(field.isMultiDimensional()) {
		const Degree* degree = &Static::getDegree(field.getSharedTranspositionId());
		Dict* getter = inc(getDict(), "FORMAT", "device_get_transposition");
		Name	(getter, &field, Params().setLeadingSeparator());
		set(getter, "DEGREE", degree->getName());
		Index	(getter, &field, Params().setNoRecursion().setShortNames());
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
void Shared::initTranspositions(Dict* dict, const Field& field, const Degree* degree) const {
	uint32_t value = 0;
	for(const char* ptr : field.getIndex()) {
		Dict* sub = dict->AddSectionDictionary("TRANSPOSITIONS");
		set(sub, "VALUE", value++);
		set(sub, "CODE", ptr);
	}
}

//-------------------------------------------------------------------
void Shared::initFlatItems(void) const {
	auto addGetterSetter = [&](const char* tmpl, const Field& field) {
		Dict* dict = inc(getDict(), "GETTER_SETTER", tmpl);
		Index	(dict, &field);
		Values	(dict, &field);
		Name	(dict, &field, Params().setLeadingSeparator());
		return dict;
	};

	auto addPtr = [&](const Field& field) {
		// init dictionary for pointer
		Dict* dict = inc(getDict(), "PTR", "device_shared_ptr");
		set(dict, "FIELD", field.getName());

		// init basic stuff
		Index	(dict, &field);
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
			if(!current->isRoot() && current->isLayoutStruct()) {
				Dict* incl = inc(dict, "LAYOUT", "ptr_layout");
				Name(incl, current, Params().setLeadingSeparator());
			}

			if(current->isLayoutStruct())
				isLayouts = true;

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
				Dict* cond = inc(dict, isConst ? "CONST_PTR" : "PTR", isLayouts ? "device_shared_ptr_cond" : "ptr_return");

				// is there only a root element?
				if(reverse.empty()) {
					inc(dict, isConst ? "CONST_PTR" : "PTR", "device_shared_ptr_R");
				} else {
					variant::Hash mul	= 1;
					variant::Hash hash	= 0;

					// init ptr
					for(const auto& pair : pairs) {
						const Field* current = pair.first;
						const Field* parent = current->getParent();
						const matog::Layout layout = pair.second;

						// COND
						if(parent->isLayoutStruct()) {
							hash += (variant::Hash)layout * mul;
							mul *= parent->isAOSOA() ? 3 : 2;
						}

						// PTR
						const char* tmpl = 0;

						if(parent->isRoot()) {
							tmpl = !parent->isLayoutStruct() ? "ptr_RW" : layout == matog::Layout::SOA ? "device_shared_ptr_R_soa" : layout == matog::Layout::AOS ? "device_shared_ptr_R_aos" : "device_shared_ptr_R_aosoa";

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

					// set value
					set(cond, "VALUE", hash);
				}

				// init end
				inc(dict, isConst ? "CONST_PTR" : "PTR", isLayouts ? "device_shared_ptr_end" : "ptr_end");
			} else {
				const Field* current = *it;
				const Field* parent = current->getParent();
				it++;

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
			initCounts(field, "COUNTS", false, false);
			initElementCounts(field, "ELEMENT_COUNTS", false);
		}

		// only structs
		if(field.isStruct()) {
			// vector getter
			if(field.isVector()) {
				Dict* dict = addGetterSetter("device_shared_getter_vector", field);
				//Name(dict, &field, Params().setLeadingSeparator());
				set(inc(dict, "TYPE", "type")->AddSectionDictionary("TYPE"), "TYPE", field.getVTypeName());

				set(dict, "AOS_FIELD", field.getChildren().front().getName());

				static const char chars[] = {'x', 'y', 'z', 'w'};

				uint32_t i = 0;
				for(const Field& sub : field.getChildren()) {
					Dict* fields = dict->AddSectionDictionary("FIELDS");
					set(fields, "FIELD", sub.getName());
					set(fields, "VFIELD", chars[i++]);
				}

				uint32_t value = 1;
				const Field* tmp = field.getParent();
				while(tmp) {
					if(tmp->isLayoutStruct())
						value *= tmp->isAOSOA() ? 3 : 2;
					tmp = tmp->getParent();
				}
				
				set(dict, "VALUE", value);
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
void Shared::initUnion(Dict* dict, const Field& current, const bool printParentCount) const {
	// is root?
	if(current.isRoot()) {
		// first iterate all children and then create pointer for all of them
		for(const Field& child : current.getChildren())
			initUnion(dict, child, true);

		// data pointer
		Dict* elem = inc(dict, "POINTER", m_root.isStruct() ? "device_shared_pointer_struct" : "device_shared_pointer_array");
		
		if(!m_root.hasChildren())
			set(elem, "TYPE", m_root.getTypeName());
		
		Dict* counts = inc(elem, "ELEMENT_COUNTS", m_root.isCustomCount() ? "device_shared_count_custom" : "device_shared_count_default");
		uint32_t dim = 0;
		char cdim = 'A';
		for(const uint32_t value : current.getCounts()) {
			Dict* tmp = counts->AddSectionDictionary("DIMS");
		
			if(value == 0) {
				char buf[3]; buf[0] = '_'; buf[1] = cdim; buf[2] = 0;
				set(tmp, "VALUE", buf);
			} else {
				set(tmp, "VALUE", value);
			}

			if(m_root.getDimCount() > 1)
				set(tmp, "DIM", dim);

			if(!current.isCube()) {
				cdim++;
				dim++;
			}
		}

		for(const Field& child : current.getChildren()) {
			Dict* sub = elem->AddSectionDictionary("CHILDREN");
			set(sub, "NAME", child.getName());
			set(sub, "TYPE", child.getTypeName());

			if(child.isStruct())
				set(sub, "PRE", "__");
		}

		if(m_root.isCustomCount())
			set(counts, "EQUATION", m_root.getCustomCount());
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
void Shared::init(void) const {
	Dict* dict = getDict();

	std::string gname(m_root.getName());
	gname.append("Shared");
	setGlobal(dict, "GLOBAL_NAME",			gname.c_str());
	setGlobal(dict, "INTERNAL_INDEX_TYPE",	"uint32_t");
	setGlobal(dict, "INDEX_TYPE",			"uint32_t");
	setGlobal(dict, "DIM_COUNT",			m_root.getCounts().size());

	// index
	Index  (dict, &m_root, Params().setNoRecursion().setOnlyVariableCounts().setTmpl("index_template").setIsCube());
	Values (dict, &m_root,	Params().setNoRecursion().setOnlyVariableCounts().setIsCube());

	// variable count
	const uint32_t dynCount = m_root.getVDimCount() == 0 ? 0 : m_root.isCube() ? 1 : m_root.getVDimCount();
	setGlobal(dict, "DIM_COUNT_DYN", dynCount);
	if(dynCount > 0)
		dict->ShowSection("IS_TEMPLATE");

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
}

//-------------------------------------------------------------------
void Shared::initRootFields(Dict* dict) const {
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
	}
}