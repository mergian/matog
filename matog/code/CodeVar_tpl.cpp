// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CodeVar.h"
#include <matog/log.h>
#include <matog/enums.h>
#include <matog/kernel/Code.h>

using ctemplate::TemplateDictionary;

using namespace matog::util;
using namespace matog::array;
using matog::kernel::Code;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
//void CodeVar::initDictionaryVars(TemplateDictionary* dict, const bool con) const {
	/*(*dict)["VAR_COUNT"] = String::toString(getFieldCount()).c_str();

	if(isStruct())
		dict->ShowSection("IF_STRUCT_TRUE");
	else
		dict->ShowSection("IF_STRUCT_FALSE");
	
	if(isSparse())
		dict->ShowSection("IF_SPARSE_TRUE");
	else
		dict->ShowSection("IF_SPARSE_FALSE");

	if(isCube())
		dict->ShowSection("IF_CUBE_TRUE");
	else
		dict->ShowSection("IF_CUBE_FALSE");

	if(isCustomIndex())
		dict->ShowSection("IF_CUSTOM_INDEX_TRUE");
	else
		dict->ShowSection("IF_CUSTOM_INDEX_FALSE");
	
	if(isTexture()) {
		auto subdict = dict->AddSectionDictionary("IF_IS_TEXTURE_TRUE");
		initDictionaryVectorType(subdict);
	} else
		dict->ShowSection("IF_IS_TEXTURE_FALSE");

	initDictionaryVectorType(dict);
	
	uint8_t id = 0;
	bool isFirst = true;

	uint32_t largestSize = 0;
	uint32_t smallestSize = UINT_MAX;
	uint32_t offset = 0;

	for(auto& item : m_fields) {
		if(item.size > largestSize)
			largestSize = item.size;

		if(item.size < smallestSize)
			smallestSize = item.size;

		TemplateDictionary* vardict = dict->AddSectionDictionary("VARS");
		
		if(isFirst) {
			(*dict)["VAR_FIRST_NAME"] = item.name.c_str();
			(*dict)["VAR_FIRST_TYPE"] = item.type.c_str();
			vardict->ShowSection("IF_VAR_ISFIRST_TRUE");
			isFirst = false;
		} else {	
			vardict->ShowSection("IF_VAR_ISFIRST_FALSE");
		}

		(*vardict)["VAR_NAME"]		= item.name.c_str();
		(*vardict)["VAR_TYPE"]		= item.type.c_str();
		(*vardict)["VAR_ID"]		= String::toString(id).c_str();
		(*vardict)["VAR_SIZE"]		= item.size * 8;

		if(isTexture()) {
			(*vardict)["VAR_TEX_FORMAT"]	= Code::texFormat(item.isFloat, item.isSigned, item.size);
			(*vardict)["VAR_TEX_FLAG"]		= Code::texFlag(item.isFloat);
			(*vardict)["VAR_TEX_OFFSET"]	= offset;
		}

		offset += item.size;
	
		if(hasVectorType())
			(*vardict)["VAR_VNAME"] = String(getVectorIndex(id)).c_str();
				
		if(isStruct())
			(*vardict).ShowSection("IF_STRUCT_TRUE");
		else
			(*vardict).ShowSection("IF_STRUCT_FALSE");

		if(isSparse())
			(*vardict).ShowSection("IF_SPARSE_TRUE");
		else
			(*vardict).ShowSection("IF_SPARSE_FALSE");

		if(item.type == "float" || item.type == "double") {
			vardict->AddSectionDictionary("IF_VAR_ISFLOAT_TRUE");
		} else {
			vardict->AddSectionDictionary("IF_VAR_ISFLOAT_FALSE");
		}

		initDictionaryVectorType(vardict);
		
		if(isTexture()) {
			auto subdict = vardict->AddSectionDictionary("IF_IS_TEXTURE_TRUE");
			initDictionaryVectorType(subdict);
		} else {
			vardict->ShowSection("IF_IS_TEXTURE_FALSE");
		}

		initDictionaryConst(vardict, con);

		id++;
	}

	(*dict)["LARGEST_VAR_SIZE"] = largestSize;
	(*dict)["SMALLEST_VAR_SIZE"] = smallestSize;*/
//}

//-------------------------------------------------------------------
void CodeVar::initDictionaryVectorType(TemplateDictionary* dict) const {
	/*if(hasVectorType()) {
		const auto& type = getVectorType();
		
		TemplateDictionary* vectordict 		= dict->AddSectionDictionary("IF_STRUCT_VECTOR_TRUE");
		(*vectordict)["STRUCT_VECTOR_TYPE"]		= type.type.c_str();
		
		if(getFieldCount() != 3) {
			TemplateDictionary* vectordict 		= dict->AddSectionDictionary("IF_VECTOR_TRUE");
			(*vectordict)["VECTOR_TYPE"]		= type.type.c_str();
			(*vectordict)["VECTOR_FLAG"]		= Code::texFlag(type.isFloat);
			(*vectordict)["VECTOR_FORMAT"]		= Code::texFormat(type.isFloat, type.isSigned, type.size);		
			TemplateDictionary* headerdict = dict->AddSectionDictionary("HEADERFILES");
			(*headerdict)["HEADER"] = "<vector_types.h>";
		} else {
			dict->ShowSection("IF_VECTOR_FALSE");
		}
	} else {
		dict->ShowSection("IF_VECTOR_FALSE");
		dict->ShowSection("IF_STRUCT_VECTOR_FALSE");
	}*/
}

//-------------------------------------------------------------------
void CodeVar::initDictionaryTranspositions(TemplateDictionary* dict) const {
	/*assert(getTranspositionCount() == m_elementIndex.size());

	// transpositions start with 0!!!
	uint32_t i = 0;

	(*dict)["COUNT"] = m_elementCount.c_str();

	for(auto& item : m_elementIndex) {
		TemplateDictionary* transposition = dict->AddSectionDictionary("TRANSPOSITIONS");
		(*transposition)["TRANSPOSITION"]				= String::toString(i).c_str();
		(*transposition)["TRANSPOSITION_INDEX"]			= item.c_str();

		i++;
	}*/
}

//-------------------------------------------------------------------
void CodeVar::initDictionaryLayouts(TemplateDictionary* dict, const String type) const {
	/*String lowerType(type);
	lowerType.toLowerCase();

	TemplateDictionary* layout = dict->AddIncludeDictionary(String("HOST_HEADER_").append(type).append("_LAYOUT").c_str());
	initDictionaryIndex(layout);

	TemplateDictionary* soa		= dict->AddIncludeDictionary(String("HOST_HEADER_").append(type).append("_SOA").c_str());

	layout	->SetFilename(getTemplateFile(String(lowerType).append("/header_layout")).c_str());
	soa		->SetFilename(getTemplateFile(String(lowerType).append("/header_soa")).c_str());
	
	initDictionaryTranspositions(layout);
	
	initDictionaryVars(layout);
	initDictionaryVars(soa);

	initDictionaryDims(layout);
	initDictionaryDims(soa);
	
	if(isStruct()) {
		TemplateDictionary* aos		= dict->AddIncludeDictionary(String("HOST_HEADER_").append(type).append("_AOS").c_str());
		TemplateDictionary* aosoa	= dict->AddIncludeDictionary(String("HOST_HEADER_").append(type).append("_AOSOA").c_str());

		aos  ->SetFilename(getTemplateFile(String(lowerType).append("/header_aos")).c_str());
		aosoa->SetFilename(getTemplateFile(String(lowerType).append("/header_aosoa")).c_str());

		initDictionaryVars(aos);
		initDictionaryVars(aosoa);

		initDictionaryDims(aos);
		initDictionaryDims(aosoa);
	}*/
}

//-------------------------------------------------------------------
void CodeVar::initDictionaryLayouts(TemplateDictionary* dict) const {
	/*dict->ShowSection("IF_SOA_TRUE");
	
	if(isStruct()) {
		dict->ShowSection("IF_AOS_TRUE");
		dict->ShowSection("IF_AOSOA_TRUE");

		TemplateDictionary* structDictHost = dict->AddIncludeDictionary("STRUCT_HOST");
		structDictHost->SetFilename(getTemplateFile("host/struct").c_str());
		initDictionaryVars		(structDictHost);
	} else {	
		dict->ShowSection("IF_AOS_FALSE");
		dict->ShowSection("IF_AOSOA_FALSE");
	}*/
}

//-------------------------------------------------------------------
void CodeVar::initDictionaryDims(TemplateDictionary* dict) const {
	/*if(getDimensionCount() > 1)
		dict->ShowSection("IF_MULTIDIM_TRUE");
	else
		dict->ShowSection("IF_MULTIDIM_FALSE");

	if(getTranspositionCount() > 1)
		dict->ShowSection("IF_MULTITRANSPOSITIONS_TRUE");
	else
		dict->ShowSection("IF_MULTITRANSPOSITIONS_FALSE");

	(*dict)["DIM_COUNT"] = String::toString(getDimensionCount()).c_str();
	for(uint32_t i = 0; i < getDimensionCount(); i++) {
		TemplateDictionary* dim = dict->AddSectionDictionary("DIMS");
		(*dim)["DIM"]			= String::toString(i).c_str();
		(*dim)["DIM_CHAR"]		= String((char)('A' + i)).c_str();
		(*dim)["DIM_CHAR_NEXT"] = String((char)('A' + i + 1)).c_str();

		if(i == 0)
			dim->ShowSection("IF_DIM_FIRST_TRUE");
		else
			dim->ShowSection("IF_DIM_FIRST_FALSE");

		if(i >= (getDimensionCount() - 1))
			dim->ShowSection("IF_DIM_ACCESSOR_FALSE");
		else
			dim->ShowSection("IF_DIM_ACCESSOR_TRUE");

		if(i == (getDimensionCount() - 2))
			dim->ShowSection("IF_DIM_ACCESSOR_LAST_TRUE");
		else
			dim->ShowSection("IF_DIM_ACCESSOR_LAST_FALSE");
	}*/
}

//-------------------------------------------------------------------
void CodeVar::initDictionaryAccessors(TemplateDictionary* dict) const {
	/*TemplateDictionary* accessors = dict->AddIncludeDictionary("ACCESSORS");

	String file; 
	
	if(isStruct()) {
		if(getDimensionCount() > 1)
			file = getTemplateFile("common/accessors_nstruct");
		else
			file = getTemplateFile("common/accessors_struct");
	} else {
		if(getDimensionCount() > 1)
			file = getTemplateFile("common/accessors_narray");
		else
			file = getTemplateFile("common/accessors_array");
	}
	
	
	accessors->SetFilename(file.c_str());
	(*accessors)["REF"] = "&";
	initDictionaryDims(accessors);
	initDictionaryVars(accessors, false);
	initDictionaryConst(accessors, false);

	TemplateDictionary* constAccessors = dict->AddIncludeDictionary("ACCESSORS_CONST");
	
	constAccessors->SetFilename(file.c_str());
	(*constAccessors)["CONST"] = "const";
	initDictionaryVars(constAccessors, true);
	initDictionaryDims(constAccessors);
	initDictionaryConst(constAccessors, true);*/
}

//-------------------------------------------------------------------
void CodeVar::initDictionaryConst(TemplateDictionary* dict, const bool con) const {
	/*if(con)
		dict->ShowSection("IF_CONST_TRUE");
	else
		dict->ShowSection("IF_CONST_FALSE");*/
}

//-------------------------------------------------------------------
void CodeVar::initDictionaryIndex(TemplateDictionary* dict) const {
	/*// init dicts
	TemplateDictionary* indexParams		= dict->AddIncludeDictionary("INDEX_PARAMS");
	TemplateDictionary* indexParams32	= dict->AddIncludeDictionary("INDEX_PARAMS32");
	TemplateDictionary* indexParams64	= dict->AddIncludeDictionary("INDEX_PARAMS64");
	TemplateDictionary* indexVars		= dict->AddIncludeDictionary("INDEX_VARS");
	
	// set files
	indexParams		->SetFilename(getTemplateFile("common/index_parameters").c_str());
	indexParams32	->SetFilename(getTemplateFile("common/index_parameters32").c_str());
	indexParams64	->SetFilename(getTemplateFile("common/index_parameters64").c_str());
	indexVars		->SetFilename(getTemplateFile("common/index_vars").c_str());

	// add dims
	initDictionaryDims(indexParams);
	initDictionaryDims(indexParams32);
	initDictionaryDims(indexParams64);
	initDictionaryDims(indexVars);*/
}

//-------------------------------------------------------------------
static void initTemplateDictionaryHelper(TemplateDictionary& dict, const char* key, const char* name, const uint32_t value, matog::util::String (*func)(const char*)) {
	/*std::ostringstream ss;
	ss << (*func)(name) << " == " << std::hex << value;
	dict.SetTemplateGlobalValue(key, ss.str().c_str());*/
} 

//-------------------------------------------------------------------
void CodeVar::dictRoot(TemplateDictionary* dict) const {
	// ids
	dict->SetTemplateGlobalValue("GLOBAL_ID",	String::toString(m_root.getGlobalId()).c_str());
	dict->SetTemplateGlobalValue("SHARED_ID",	String::toString(m_root.getSharedId()).c_str());
	dict->SetTemplateGlobalValue("DYN_ID",		String::toString(m_root.getDynId()).c_str());
	
	// names
	dict->SetTemplateGlobalValue("GLOBAL_NAME",			m_root.getName());
	dict->SetTemplateGlobalValue("SHARED_NAME",			m_root.getSharedName());
	dict->SetTemplateGlobalValue("DYN_NAME",			m_root.getDynName());

	// cnt sizes
	std::ostringstream hIndex, hValues, dIndex, dValues;

	uint32_t zeroCounts = 0;
	char hChar = 'A', dChar = 'A';

	for(const uint32_t cnt : m_root.getCounts()) {
		// count host only if cnt == zero
		if(cnt == 0) {
			if(hChar != 'A') {
				hIndex  << ", ";
				hValues << ", ";
			}

			hIndex << "const uint64_t " << hChar;
			hValues << hChar;
				
			hChar++;
		}

		if(dChar != 'A') {
			dIndex  << ", ";
			dValues << ", ";
		}

		dIndex << "const uint64_t " << dChar;
		dValues << dChar;
				
		dChar++;
	}

	dict->SetTemplateGlobalValue("HOST_DIM_CNT",		String::toString((int)(hChar - 'A')).c_str());
	dict->SetTemplateGlobalValue("HOST_INDEX",			hIndex.str());
	dict->SetTemplateGlobalValue("HOST_INDEX_VALUES",	hValues.str());

	dict->SetTemplateGlobalValue("DEVICE_DIM_CNT",		String::toString((int)(dChar - 'A')).c_str());
	dict->SetTemplateGlobalValue("DEVICE_INDEX",		dIndex.str());
	dict->SetTemplateGlobalValue("DEVICE_INDEX_VALUES",	dValues.str());

	dictInclude(dict, "HOST_HEADER_CALC_SIZE", m_root.isStruct() ? "host_header_calc_size_struct" : "host_header_calc_size");
}

//-------------------------------------------------------------------
void CodeVar::dictCNTS(Dict* dict, const Field& field) const {
	uint32_t dynamicCntIndex = 0;
	uint32_t dim = 0;
	dict->SetIntValue("CNT_SIZE", (long)field.getCounts().size());
	for(const uint32_t cnt : field.getCounts()) {
		Dict* cnts = dictSection(dict, "CNTS");
		
		cnts->SetIntValue("DIM", dim++);

		if(cnt == 0) {
			std::ostringstream ss;
			ss << "m_cnt[" << dynamicCntIndex++ << "]";
			cnts->SetValue("CNT", ss.str().c_str());
		} else {
			cnts->SetIntValue("CNT", cnt);
		}
	}	
}

//-------------------------------------------------------------------
void CodeVar::dictAccessorVars(Dict* dict, const array::Field& field) const {
	for(const Field& sub : field.getChildren()) {
		Dict* child = dictSection(dict, "CHILDREN");
		child->SetValue("NAME", sub.getName());

		if(sub.isArray())
			child->SetValue("TYPE", sub.getName());
		else
			child->SetValue("TYPE", sub.getTypeName());
	}
}

//-------------------------------------------------------------------
void CodeVar::dictAccessors(Dict* dict, const Field& field) const {
	Dict* dims = dictSection(dict, "DIMS");

	const auto& cnts = field.getCounts();

	const bool isSingle = cnts.size() == 1;
	const bool isStruct = field.isStruct();
	for(auto it = cnts.rbegin(); it != cnts.rend(); it++) {
		const bool isLast  = it == cnts.rbegin();

		Dict* accessor = 0;
		
		if(isSingle) {
			if(!isStruct)	accessor = dictInclude(dims, "ACCESSOR_CLASS", "accessor_single");
			else			accessor = dictInclude(dims, "ACCESSOR_CLASS", "accessor_single_struct");

			dictAccessorVars(accessor, field);
		} else {
			if(isLast)		{
				if(!isStruct)	accessor = dictInclude(dims, "ACCESSOR_CLASS", "accessor_multi_last");
				else			accessor = dictInclude(dims, "ACCESSOR_CLASS", "accessor_multi_last_struct");

				dictAccessorVars(accessor, field);
			} else {
				accessor = dictInclude(dims, "ACCESSOR_CLASS", "accessor_multi");
			}
		}
	}
}

//-------------------------------------------------------------------
void CodeVar::dictVars(Dict* dict, const Field& field) const {
	// init stuff
	dict->SetValue("NAME", field.getName());
	
	// add children
	if(field.isArray()) {
		// is vector
		if(field.isVector())
			dict->SetValue("VECTOR_TYPE", field.getVTypeName());

		// counts
		dictCNTS(dict, field);
		dictCNTS(dictInclude(dict, "HOST_HEADER_GETCOUNT", 
			field.getCounts().size() == 1 ? "host_header_getcount_single" : "host_header_getcount_multi")
			, field);

		// accessors
		dictAccessors(dict, field);
		
		// add children
		for(const Field& field : field.getChildren()) {
			Dict* children = dictSection(dict, "CHILDREN");
			
			if(field.isArray()) {
				dictVars(dictInclude(children, "HOST_HEADER_ARRAYS", "host_header_arrays"), field);
				dictVars(dictInclude(children, "HOST_HEADER_ARRAY_GETTER", "host_header_array_getter"), field);
			} else
				dictVars(dictInclude(children, "HOST_HEADER_VAR_GETTER", "host_header_var_getter"), field);
		}
	} else {
		dict->SetValue("TYPE", field.getTypeName());
	}

	/*TemplateDictionary* accessors = dict->AddIncludeDictionary("ACCESSORS");

	String file; 
	
	if(isStruct()) {
		if(getDimensionCount() > 1)
			file = getTemplateFile("common/accessors_nstruct");
		else
			file = getTemplateFile("common/accessors_struct");
	} else {
		if(getDimensionCount() > 1)
			file = getTemplateFile("common/accessors_narray");
		else
			file = getTemplateFile("common/accessors_array");
	}
	
	
	accessors->SetFilename(file.c_str());*/
}

//-------------------------------------------------------------------
void CodeVar::initTemplateDictionary(void) {
	TemplateDictionary* dict = &getDict();

	dictRoot(dict);
	dictVars(dict, m_root);

	// init stuff
	/*dict.SetTemplateGlobalValue("NAME", getName().c_str());
	dict.SetTemplateGlobalValue("IFNDEF_DEVICE", Code::ifndef(getName()).c_str());
	dict.SetTemplateGlobalValue("IFNDEF_SHARED", Code::ifndef(getSharedName()).c_str());
	dict.SetTemplateGlobalValue("IFNDEF_DYNSHARED", Code::ifndef(getDynSharedName()).c_str());
	
	dict.SetTemplateGlobalValue("INDEX_TYPE", uses64BitIndex() ? "uint64_t" : "uint32_t");
	dict.SetTemplateGlobalValue("INTERNAL_INDEX_TYPE", uses64BitInternalIndex() ? "uint64_t" : "uint32_t");
	
	initTemplateDictionaryHelper(dict, "AOS_DEVICE", getName(), (uint32_t)Layout::AOS, &Code::layoutDefine);
	initTemplateDictionaryHelper(dict, "SOA_DEVICE", getName(), (uint32_t)Layout::SOA, &Code::layoutDefine);
	initTemplateDictionaryHelper(dict, "AOSOA_DEVICE", getName(), (uint32_t)Layout::AOSOA, &Code::layoutDefine);
	
	initTemplateDictionaryHelper(dict, "AOS_SHARED", getSharedName(), (uint32_t)Layout::AOS, &Code::layoutDefine);
	initTemplateDictionaryHelper(dict, "SOA_SHARED", getSharedName(), (uint32_t)Layout::SOA, &Code::layoutDefine);
	initTemplateDictionaryHelper(dict, "AOSOA_SHARED", getSharedName(), (uint32_t)Layout::AOSOA, &Code::layoutDefine);
	
	initTemplateDictionaryHelper(dict, "AOS_DYNSHARED", getDynSharedName(), (uint32_t)Layout::AOS, &Code::layoutDefine);
	initTemplateDictionaryHelper(dict, "SOA_DYNSHARED", getDynSharedName(), (uint32_t)Layout::SOA, &Code::layoutDefine);
	initTemplateDictionaryHelper(dict, "AOSOA_DYNSHARED", getDynSharedName(), (uint32_t)Layout::AOSOA, &Code::layoutDefine);

	dict.SetTemplateGlobalValue("LAYOUT", Code::layoutDefine(getName()).c_str());
	dict.SetTemplateGlobalValue("LAYOUT_SHARED", Code::layoutDefine(getSharedName()).c_str());
	dict.SetTemplateGlobalValue("LAYOUT_DYNSHARED", Code::layoutDefine(getDynSharedName()).c_str());
	dict.SetTemplateGlobalValue("MEMORY", Code::memoryDefine(getName()).c_str());
	dict.SetTemplateGlobalValue("TRANSPOSITION", Code::transpositionDefine(getName()).c_str());
	dict.SetTemplateGlobalValue("TRANSPOSITION_SHARED", Code::transpositionDefine(getSharedName()).c_str());
	dict.SetTemplateGlobalValue("TRANSPOSITION_DYNSHARED", Code::transpositionDefine(getDynSharedName()).c_str());

	// init index
	initDictionaryIndex(&dict);

	// init vars
	initDictionaryVars(&dict);

	// init transpositions
	initDictionaryTranspositions(&dict);

	// init layouts
	initDictionaryLayouts(&dict);
	
	// init dims
	initDictionaryDims(&dict);

	// init accessors
	initDictionaryAccessors(&dict);*/
}

//-------------------------------------------------------------------
CodeVar::Dict* CodeVar::dictSection(Dict* dict, const char* name) const {
	return dict->AddSectionDictionary(name);
}

//-------------------------------------------------------------------
CodeVar::Dict* CodeVar::dictInclude(Dict* dict, const char* name, const char* tpl) const {
	std::ostringstream ss;
	ss << s_base << tpl << ".tpl";

	Dict* include = dict->AddIncludeDictionary(name);
	include->SetFilename(ss.str().c_str());
	return include;
}

//-------------------------------------------------------------------
	}
}
