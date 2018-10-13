// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "CodeTemplate.h"
#include "CMakeModel.h"
#include <fstream>
#include <sstream>
#include <matog/array/Field.h>
#include <matog/array/field/Type.h>

using matog::array::Field;

namespace matog {
	namespace code {
//-------------------------------------------------------------------
CodeTemplate::CodeTemplate(const array::Field& root, const bool isRoot64Bit) : Template(root.getName()), CodeHelper(isRoot64Bit), m_root(root) {
}

//-------------------------------------------------------------------
CodeTemplate::~CodeTemplate(void) {
}

//-------------------------------------------------------------------
void CodeTemplate::generateFile(const char* tmpl, const char* nameExtension, const char* fileExtension) const {
	std::ofstream file;
	openFile(file, nameExtension, fileExtension);
	file << expandTemplate(tmpl);
	closeFile(file);
}

//-------------------------------------------------------------------
void CodeTemplate::openFile(std::ofstream& stream, const char* nameExtension, const char* fileExtension) const {
	std::ostringstream filename;
	std::ostringstream cmakeName;
	cmakeName << m_root.getName() << nameExtension << "." << fileExtension;
	filename << "matog/" << cmakeName.str();

	stream.open(filename.str().c_str());

	THROWIF(!stream, "IO_EXCEPTION");
	
	const bool isHeader = strcmp(fileExtension, "h") == 0;
	const bool isCuda	= strcmp(fileExtension, "cu") == 0;

	CMakeModel::getInstance().addSourceFile(cmakeName.str(), "Generated", isHeader || isCuda);
}

//-------------------------------------------------------------------
void CodeTemplate::closeFile(std::ofstream& stream) const {
	THROWIF(!stream, "IO_EXCEPTION");
	stream.close();
}

//-------------------------------------------------------------------
void CodeTemplate::initCounts(const Field& field, const char* label, const bool isRoot64Bit, const bool usesMembers, const bool usesHints) const {
	const char* tmpl = 0;

	if(!field.isDimensional() || field.getDimCount() == 1) tmpl = "count";
	else if(usesMembers && &field == &m_root) {
		if(usesHints)	tmpl = "count_T_root_hints";
		else			tmpl = "count_T_root";
	}
	else tmpl = "count_T";
	
	Dict* counts = inc(getDict(), label, tmpl);
	
	Name(counts, &field, Params().setLeadingSeparator());
	set(counts, "TYPE", isRoot64Bit && &field == &m_root ? "uint64_t" : "uint32_t");
	
	uint32_t dynamicCntIndex = 0;
	uint32_t dim = 0;

	set(counts, "DIM_COUNT", (uint32_t)field.getCounts().size());

	for(const uint32_t cnt : field.getCounts()) {
		Dict* tmp = counts->AddSectionDictionary("DIMS");

		if(usesMembers) {
			if(cnt == 0) {
				tmp->ShowSection("IS_DYNAMIC");
				set(tmp, "INDEX", dynamicCntIndex);

				if(!field.isCube())
					dynamicCntIndex++;
			} else {
				tmp->ShowSection("IS_STATIC");
				set(tmp, "VALUE", cnt);
			}
		} else {
			tmp->ShowSection("IS_STATIC");
			if(cnt == 0) {
				char buf[3];
				buf[0] = '_';
				buf[1] = field.isCube() ? 'A' : (char)('A' + dim);
				buf[2] = 0;

				set(tmp, "VALUE", buf);
			} else {
				set(tmp, "VALUE", cnt);
			}
		}

		tmp->SetIntValue("DIM", dim++);
	}
}

//-------------------------------------------------------------------
void CodeTemplate::initElementCounts(const Field& field, const char* label, const bool isRoot64Bit) const {
	Dict* counts = inc(getDict(), label, field.isCustomCount() ? "element_count_custom" : "element_count_default");
	Name(counts, &field, Params().setLeadingSeparator());
	set(counts, "TYPE", isRoot64Bit && &field == &m_root ? "uint64_t" : "uint32_t");

	for(uint32_t dim = 0; dim < field.getDimCount(); dim++) {
		Dict* tmp = counts->AddSectionDictionary("DIMS");
		
		set(tmp, "CHAR", (char)('A' + dim));
		if(field.getDimCount() > 1)
			set(tmp, "DIM", dim);
	}

	if(field.isCustomCount())
		set(counts, "EQUATION", field.getCustomCount());
}

//-------------------------------------------------------------------
	}
}