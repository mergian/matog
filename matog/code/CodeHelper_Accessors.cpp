// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CodeHelper.h"
#include <matog/log.h>
#include <matog/array/Field.h>
#include <matog/array/field/Type.h>
#include <matog/Static.h>
#include <matog/enums.h>

using matog::array::Field;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
CodeHelper::Accessors::Accessors(Dict* dict, const Field* field, const Params params) : m_dict(dict), m_isRoot64Bit(params.isRoot64Bit), m_root(params.root) {
	recursive(0, 0, field);
	rootOperators(field);
}

//-------------------------------------------------------------------
void CodeHelper::Accessors::rootOperators(const array::Field* root) const {

}

//-------------------------------------------------------------------
void CodeHelper::Accessors::operatorAcc2Acc(Dict* dict, const array::Field* current) const {
	std::function<void (Dict* dict, const Field* child, const char* tmpl)> a2aHelper = [&a2aHelper, &current](Dict* dict, const Field* child, const char* tmpl) {
		if(!child->isStruct()) {
			uint32_t cnt = 1;
			std::list<const Field*> trace;

			for(const Field* tmp = child; tmp!= 0 && tmp != current; tmp = tmp->getParent()) {
				if(tmp->isDimensional()) {
					trace.emplace_back(tmp);
					cnt *= tmp->getElementCount();
				}
			}

			for(uint32_t i = 0; i < cnt; i++) {
				Dict* field = inc(dict, "FIELDS", tmpl);
				Name name(field, child, Params().setLeadingSeparator());
				set(field, "TYPE", child->getTypeName());

				Values values(field, current, Params().setTmpl("values_this"));
				Values others(field, current, Params().setIncl("OTHER_VALUES").setTmpl("values_other"));

				uint32_t tmp = i;
				uint32_t mul = 1;
				for(auto it = trace.rbegin(); it != trace.rend(); it++) { 
					const Field* item = *it;

					for(uint32_t dim = 0; dim < item->getDimCount(); dim++) {
						uint32_t value = (tmp / mul) % item->getCounts()[dim];
						mul *= item->getCounts()[dim];

						values.add(value);
						others.add(value);
					}
				}
			}	
		}

		for(const Field& sub : child->getChildren())
			a2aHelper(dict, &sub, tmpl);
	};

	Dict* a2a = inc(dict, "OPERATOR", "accessor_operator_accessor");
	Ident(a2a, current, current->getDimCount() - 1);
	a2aHelper(a2a, current, "accessor_operator_accessor_field");

	a2a = inc(dict, "OPERATOR", "accessor_operator_accessor_swap");
	Ident(a2a, current, current->getDimCount() - 1);
	a2aHelper(a2a, current, "accessor_operator_accessor_swap_field");

	if(current->isVector()) {
		a2a = inc(dict, "OPERATOR", "accessor_operator_vector");
		Values(a2a, current, Params().setTmpl("values_this"));
		Name(a2a, current, Params().setLeadingSeparator());
		set(a2a, "VTYPE", current->getVTypeName());
	}

}

//-------------------------------------------------------------------
void CodeHelper::Accessors::recursive(Dict* preData, Dict* preAcc, const array::Field* current) const {
	// for each dim
	Dict* data = 0;
	Dict* acc  = 0;

	auto initStruct = [this](Dict* acc, const Field* current) {
		for(const Field& child : current->getChildren()) {
			auto helper = [&child](Dict* dict) {
				set(dict, "FIELD", child.getName());
				if(!child.isStruct()) set(dict, "TYPE", child.getTypeName());
				Ident	(dict, &child, 0);
				Name	(dict, &child, Params().setLeadingSeparator());
				Values	(dict, &child, Params().setTmpl("values_this"));
			};

			helper(inc(acc, "VARS", child.isStruct() || child.isDimensional() ? "accessor_accessor" : "accessor_field"));
			helper(inc(acc->AddSectionDictionary("INIT"), "INIT", child.isStruct() || child.isDimensional() ? "accessor_init_accessor" : "accessor_init_field"));
			if(!child.isStruct() && !child.isDimensional())
				helper(inc(acc, "INIT_FUNCTIONS", "accessor_init_func"));
		}

		
		// operators
		operatorAcc2Acc(acc, current);
	};

	// iterate all dims
	if(current->isDimensional()) {
		if(!current->isRoot() && current->isStruct()) {
			Name(inc(m_dict, "ACCESSOR_FORWARD_DECLARATION", "accessor_forward_accessor"), current, Params().setIncl("IDENT"));

			const Field* parent = current->getParent();

			Dict* dict = inc(m_dict, "ACCESSOR", "accessor");
			Name(dict, current, Params().setIncl("IDENT"));

			// add superclass
			const uint32_t pdim = parent->getDimCount() - 1;
			Ident(inc(dict->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_parent"), parent, pdim);
			Ident(inc(dict, "SUPERCLASS", "accessor_super_accessor"), parent, pdim);
			Ident(inc(dict->AddSectionDictionary("INIT"), "INIT", "accessor_init_data2"), parent, pdim);

			Dict* incl = inc(dict, "OPERATOR", "accessor_operator_struct");
			Ident(incl, current, 0);
			set(incl, "INDEX_TYPE", current->isRoot() && m_isRoot64Bit ? "uint64_t" : "uint32_t");
		}

		for(uint32_t dim = 0; dim < current->getDimCount(); dim++) {
			// some bool stuff
			const uint32_t ndim		= dim+1;
			const bool isLast		= ndim == current->getDimCount();
			const bool isFirst		= dim == 0;
			const bool isNextToLast = (ndim+1) == current->getDimCount();
			const Field* parent		= dim == 0 && current->isRoot() ? 0 : dim == 0 ? current->getParent() : current;

			if(isLast && !current->isStruct()) {
				if(!current->isRoot()) {
					Name(inc(m_dict, "ACCESSOR_FORWARD_DECLARATION", "accessor_forward_accessor"), current, Params().setIncl("IDENT"));

					const Field* parent = current->getParent();

					Dict* dict = inc(m_dict, "ACCESSOR", "accessor");
					Name(dict, current, Params().setIncl("IDENT"));

					// add superclass
					const uint32_t pdim = parent->getDimCount() - 1;
					Ident(inc(dict->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_parent"), parent, pdim);
					Ident(inc(dict, "SUPERCLASS", "accessor_super_accessor"), parent, pdim);
					Ident(inc(dict->AddSectionDictionary("INIT"), "INIT", "accessor_init_data2"), parent, pdim);

					Dict* incl = inc(dict, "OPERATOR", "accessor_operator_value");
					Ident(incl, current, 0);
					set(incl, "INDEX_TYPE", current->isRoot() && m_isRoot64Bit ? "uint64_t" : "uint32_t");
					set(incl, "TYPE", current->getTypeName());
					Name(incl, current, Params().setLeadingSeparator());
					
					Values(incl, current->getParent(), Params().setTmpl("values_this"));
					inc(incl, "VALUES", "values_separator");// separator
					Values(incl, current, Params().setNoRecursion(true));
				}
			} else {
				// add forward
				Ident(inc(m_dict, "ACCESSOR_FORWARD_DECLARATION", "accessor_forward_data"), current, dim);
				Ident(inc(m_dict, "ACCESSOR_FORWARD_DECLARATION", "accessor_forward_accessor"), current, dim);

				// determine case
				data = inc(m_dict, "ACCESSOR_DATA", "accessor_data");
				Ident(data, current, dim);
				set(data, "INDEX_TYPE", m_isRoot64Bit && current->getParent() == 0 ? "uint64_t" : "uint32_t");

				acc = inc(m_dict, "ACCESSOR", "accessor");
				Ident(acc, current, dim);

				// add superclass
				Ident(inc(acc,  "SUPERCLASS", "accessor_super_accessor"), current, dim);

				// add root param
				if(!parent) {
					set(inc(acc ->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_root"), "ROOT", m_root);
					set(inc(data->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_root"), "ROOT", m_root);
					set(data->AddSectionDictionary("ROOT"), "ROOT", m_root);
					inc(data->AddSectionDictionary("INIT"), "INIT", "accessor_init_data3");
				} else {
					const uint32_t pdim = dim == 0 ? parent->getDimCount() - 1 : dim - 1;
					if(!isFirst) {
						Ident(inc(acc ->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_parent"), parent, pdim);
					} else if(current->isStruct()) {
						Name(inc(acc ->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_parent"), current, Params().setIncl("IDENT"));
					} else if(current->isDimensional()) {
						Name(inc(acc ->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_parent"), current, Params().setIncl("IDENT"));
					}
					Ident(inc(data->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_data"),   parent, pdim);
					Ident(inc(data, "SUPERCLASS", "accessor_super_accessor"), parent, pdim);
					Ident(inc(data->AddSectionDictionary("INIT"), "INIT", "accessor_init_data2"), parent, pdim);
				}

				// add value param
				Dict* value = inc(acc->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_value");
				Ident(value, current, dim);
				set(value, "INDEX_TYPE", current->isRoot() && m_isRoot64Bit ? "uint64_t" : "uint32_t");

				value = inc(data->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_value");
				Ident(value, current, dim);
				set(value, "INDEX_TYPE", current->isRoot() && m_isRoot64Bit ? "uint64_t" : "uint32_t");

				// init accessor_data
				Ident(inc(acc->AddSectionDictionary("INIT"), "INIT", "accessor_init_data"), current, dim);

				// init value
				Ident(inc(data->AddSectionDictionary("INIT"), "INIT", "accessor_init_value"), current, dim);

				// add operator
				if(!isLast) {
					if(!isNextToLast || current->isStruct()) {
						Dict* incl = inc(acc, "OPERATOR", "accessor_operator_struct");
						Ident(incl, current, ndim);
						set(incl, "INDEX_TYPE", current->isRoot() && m_isRoot64Bit ? "uint64_t" : "uint32_t");
					} else {
						Dict* incl = inc(acc, "OPERATOR", "accessor_operator_value");
						Ident	(incl, current, ndim);
						Name	(incl, current, Params().setLeadingSeparator());
						Values	(incl, current, Params().setTmpl("values_this").setLastNoChar());
						set(incl, "TYPE", current->getTypeName());
					}
				}/* else {
					if(current->isVector()) {
						Dict* incl = inc(acc, "OPERATOR", "accessor_operator_vector");
						Ident(incl, current, ndim);
						Values(incl, current);
						Name(incl, current, Params().setLeadingSeparator());
						set(incl, "VTYPE", current->getVTypeName());
					}
				}*/
			}
		}
	} else if(current->isStruct()) {
		// add forward
		Ident(inc(m_dict, "ACCESSOR_FORWARD_DECLARATION", "accessor_forward_accessor"), current, 0);

		// determine case
		acc = inc(m_dict, "ACCESSOR", "accessor");
		Ident(acc, current, 0);

		// add superclass
		const Field* parent = current->getParent();

		while(parent != 0 && !parent->isDimensional())
			parent = parent->getParent();
		
		// add root param
		if(!parent) {
			set(inc(acc->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_root"), "ROOT", m_root);
		} else {
			const uint32_t pdim = parent->getDimCount() - 1;
			Ident(inc(acc->AddSectionDictionary("PARAMS"), "PARAM", "accessor_params_parent"), parent, pdim);

			// add superclass
			Ident(inc(acc, "SUPERCLASS", "accessor_super_accessor"), parent, pdim);

			// init accessor_data
			Ident(inc(acc->AddSectionDictionary("INIT"), "INIT", "accessor_init_data2"), parent, pdim);
		}
	}
	
	// add children to structs
	if(current->isStruct())
		initStruct(acc, current);

	// for all children
	for(const Field& child : current->getChildren())
		recursive(data, acc, &child);

	// add operator if this is root
	if(current->isRoot()) {
		if(current->isStruct() || current->isMultiDimensional())
			Ident(inc(m_dict, "ACCESSOR", "accessor_operator_root"), current, 0);
		else
			set(inc(m_dict, "ACCESSOR", "accessor_operator_root_field"), "TYPE", current->getTypeName());
	}
}

//-------------------------------------------------------------------
	}
}