// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/Kernel.h>
#include <matog/Static.h>
#include <matog/db/Stmt.h>
#include <matog/db/tables.h>
#include <matog/kernel/Code.h>
#include <matog/degree/Type.h>
#include <matog/array/Type.h>
#include <matog/log.h>
#include <matog/macros.h>
#include <matog/cuda/Architecture.h>
#include <matog/variant/Type.h>
#include <matog/array/field/Type.h>
#include <algorithm>
#include <matog/enums.h>

using matog::db::Stmt;

namespace matog {
//-------------------------------------------------------------------
Kernel::Kernel(void) :
	m_id			(0),
	m_moduleId		(0),
	m_name			(0),
	m_completeName	(0)
{}

//-------------------------------------------------------------------
void Kernel::set(const Stmt& stmt) {
	m_id		= stmt.get<uint32_t>(0);
	m_moduleId	= stmt.get<uint32_t>(2);
	m_name		= NEW_STRING(stmt.get<const char*>(1));

	std::ostringstream ss;
	ss << Static::getModule(m_moduleId).getName() << "/" << getName();
	const std::string tmp = ss.str();
	m_completeName = NEW_STRING(tmp.c_str());

	m_rwHints.load(getCompleteName());
	m_countHints.load(getCompleteName());
	m_refHints.load(getCompleteName());

	dbSelectDegrees();
}

//-------------------------------------------------------------------
Kernel::~Kernel(void) {
	if(m_name)
		FREE_A(m_name);

	if(m_completeName)
		FREE_A(m_completeName);
}

//-------------------------------------------------------------------
void Kernel::dbSelectDegrees(void) {
	Stmt stmt("SELECT degree_id, instance_id FROM " DB_KERNEL_DEGREES " WHERE kernel_id = ?;");
	stmt.bind(0, getId());

	// we need to use the set here, to ensure the correct ordering!
	m_degrees.clear();
	while(stmt.step())
		m_degrees.emplace(&Static::getDegree(stmt.get<uint32_t>(0)), stmt.get<uint32_t>(1));
	
	// update index
	updateIndex();
}

//-------------------------------------------------------------------
void Kernel::setDegrees(const variant::Set& set) {
	// reset degrees
	m_degrees.clear();
	m_degrees.insert(set.begin(), set.end());

	// update index
	updateIndex();

	// store to db
	dbSetDegrees();
}

//-------------------------------------------------------------------
void Kernel::updateIndex(void) {
	m_variant.init(m_degrees);
}

//-------------------------------------------------------------------
void Kernel::dbSetDegrees(void) {
	Stmt stmt("INSERT INTO " DB_KERNEL_DEGREES " (kernel_id, degree_id, instance_id) VALUES (?, ?, ?);");
	stmt.bind(0, getId());

	for(const auto& item : m_degrees) {
		stmt.reset();
		stmt.bind(1, item.getDegree()->getId());
		stmt.bind(2, item.getParamId());
		stmt.step();
	}
}

//-------------------------------------------------------------------
const char* Kernel::generateMatogCode(variant::Hash hash, const cuda::Architecture architecture) const  {
	//---------------------------------------------------------------
	/*
		This method is kind of "the heart" of MATOG. It generates
		a file that is included into each compiled kernel, containing
		the information which layout, transposition, ... is used.

		There are multiple techniques used for this. DEFINES, RANGES
		and the values for StaticShared arrays are set as #DEFINE. 
		The others are set using look up functions that return a 
		specific value for each instance (paramId). 

		Further, for texture memory, a memory lookup function is 
		provided. For CC>=3.5 it just calls __ldg(ptr). For 64bit 
		values, we read an uint2 and pointer cast it to the correct
		value.

		For CC<3.5 we do some fancy shit (unfortunately this is 
		necessary). First of all, texture loads only support 32bit
		values so we always load 32bit values and then extract the
		necessary 8, 16 or 32 bit. For 64bit, we load 2x 32bit 
		values.

		As textures can only store 2^27 elements, we have put 
		multiple textures after each other. This requires to change
		the texRef if the item is in the second texture. Using an IF
		is extremly slow at this location, so we trick the GPU. We
		grab the 64bit ptr of the texRef and pass this one to the
		texture lookup. On Fermi cards, the ptr of the adjoining 
		texRef is ptr = prePtr + 0x80000. Since Kepler it is ptr
		= prePtr - 1 (hopefully this will never change again...).

		Unfortunately the compiler removes texRefs if they are not 
		explicitly referenced in the code. So we have to add a 
		section that references all texRefs. We add a method that 
		loads the address of all texRefs but which is never executed
		as we skip it using a goto. Not calling the function does not
		work, as the compiler then just removes it. However the goto
		trick seems to work as for now.
	*/
	//---------------------------------------------------------------
	// init stream
	std::ostringstream stream;
	stream << "#include <cassert>" << std::endl;
	stream << "#include <cstdint>" << std::endl;
	stream << "#include <cstdio>" << std::endl;
	stream << "#include <cstdlib>" << std::endl;
	stream << "#include <vector_types.h>" << std::endl << std::endl;
	
	// init vars
	const bool usesTexRef = architecture < cuda::Architecture::KEPLER_35;

	std::list<variant::Item> defines;
	std::map<const Degree*, std::list<uint32_t> > functions;
	std::map<const Degree*, std::list<uint32_t> > textures;
	std::map<const Array*, std::set<uint32_t> > globalArrays;

	// --------------------------------------------------------------
	// sort degrees to their categories
	for(const auto& item : getVariant().subset(variant::Type::Compiler)) {
		// init vars
		const Degree* degree = item.getDegree();
		const bool isShared = degree->isArray() ? degree->getArray()->getType() == array::Type::Shared : false;

		if(degree->isArray() && degree->getArray()->getType() == array::Type::Global)
			globalArrays[degree->getArray()].emplace(item.getParamId());

		switch(degree->getType()) {
		case degree::Type::ArrayLayout:
			if(isShared)	defines.emplace_back(item);
			else			functions[degree].emplace_back(item.getParamId());
			break;

		case degree::Type::ArrayMemory:
			functions[degree].emplace_back(item.getParamId());
			textures[degree].emplace_back(item.getParamId());
			break;

		case degree::Type::ArrayTransposition:
			functions[degree].emplace_back(item.getParamId());
			break;
		
		case degree::Type::Define:
		case degree::Type::Range:
			defines.emplace_back(item);
			break;

		default:
			THROW();
		}
	}

	//---------------------------------------------------------------
	// iterate all defines
	for(const Item& item : defines)
		stream << "#define " << item.getDegree()->getName() << " " << (uint32_t)getVariant().get(hash, item) << std::endl;

	stream << std::endl;

	//---------------------------------------------------------------
	// functions
	for(const auto& it : functions) {
		const Degree* degree = it.first;
		
		// open function
		stream << "static __device__ __forceinline__ int __" << degree->getName() << "(const int PID = 0) {" << std::endl;

		// iterate all combinations
		for(const uint32_t paramId : it.second) {
			const variant::Value value = getVariant().get(hash, degree, paramId);
			stream << "\tif(PID == " << paramId << ") return " << (int)value << ";" << std::endl;
		}

		// finish
		stream << "\tassert(0);" << std::endl << "\t__trap();" << std::endl << "\treturn 0;" << std::endl << "}" << std::endl << std::endl;
	}

	//---------------------------------------------------------------
	// TEXREF TEXTURES
	if(usesTexRef) {
		// max memory for CC3.0 cards is 2GB! So we need max 4 textures
		const uint32_t texNum = 4;

		// tex value union
		stream << "union __TexValue {" << std::endl;
		stream << "\tuint64_t l;" << std::endl;
		stream << "\tuint32_t i[2];" << std::endl;
		stream << "\tuint16_t s[2];" << std::endl;
		stream << "\tuint8_t  c[4];" << std::endl;
		stream << "};" << std::endl;

		//-----------------------------------------------------------
		// create texRefs
		std::map<const Degree*, std::set<uint32_t> > used;
		for(const auto& it : textures) {
			const Degree* degree = it.first;
			const Array* array   = degree->getArray();

			// generate name dummy
			std::string name(array->getName());
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);

			// init texrefs
			for(const uint32_t paramId : it.second)
				if(getVariant().get<matog::Memory>(hash, degree, paramId) == matog::Memory::TEXTURE)
					for(uint32_t i = 0; i < texNum; i++)
						stream << "texture<int32_t, cudaTextureType1D, cudaReadModeElementType> __"
							<< name << "_TEXREF_" << paramId << "_" << i << ";" << std::endl;
		}
		
		// iterate all texture functions
		for(const auto& it : textures) {
			const Degree* degree = it.first;
			const Array* array   = degree->getArray();

			// generate name dummy
			std::string name(array->getName());
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);

			// iterate all combinations
			stream << "static __device__ __forceinline__ void __" << name << "_INIT(void) {" << std::endl;
			stream << "\tgoto __SKIP_" << name << ";" << std::endl;
			stream << "\tasm(" << std::endl;
			stream << "\t\t\"{\\n\"" << std::endl;
			stream << "\t\t\t\".reg .u64 %tmp;\\n\"" << std::endl;
			for(const uint32_t paramId : it.second) {
				if(getVariant().get<matog::Memory>(hash, degree, paramId) == matog::Memory::TEXTURE) {
					for(uint32_t i = 0; i < texNum; i++) {
						// init root texref
						std::ostringstream texRefSS;
						texRefSS << "__" << name << "_TEXREF_" << paramId << "_" << i;
					
						const std::string texRef(texRefSS.str());

						stream << "\t\t\t\"mov.u64 %tmp, " << texRef << ";\\n\"" << std::endl;
					}
				}
			}
			stream << "\t\t\"}\\n\"" << std::endl;
			stream << "\t);" << std::endl;
			stream << "\t__SKIP_" << name << ":" << std::endl;
			stream << "}" << std::endl << std::endl;

			stream << "template<typename T> __device__ __forceinline__ T __" << name << "_TEX(const uint32_t PID, const T* ptr, const T* root) {" << std::endl;
			stream << "\t__" << name << "_INIT();" << std::endl;

			for(const uint32_t paramId : it.second) {
				if(getVariant().get<matog::Memory>(hash, degree, paramId) == matog::Memory::TEXTURE) {
					// init root texref
					std::ostringstream texRefSS;
					texRefSS << "__" << name << "_TEXREF_" << paramId << "_0";
					
					const std::string texRef(texRefSS.str());

					// check PID
					stream << "\tif(PID == " << paramId << ") {" << std::endl;
						
					// calc index, texOffset and texIndex
					stream << "\t\tconst uint32_t byteIndex = (uint32_t)((((uint64_t)ptr) - ((uint64_t)root)));" << std::endl;
					stream << "\t\tconst uint32_t intIndex  = byteIndex / sizeof(uint32_t);" << std::endl;
					// textures only can access 2^27 elements.
					stream << "\t\tconst int32_t  texIndex  = intIndex & 0x7FFFFFF;" << std::endl;
					stream << "\t\tconst uint64_t texOffset = intIndex >> 27;" << std::endl;
									
					// get texref
					stream << "\t\tuint64_t tex;\n"<< std::endl;
					stream << "\t\tasm(\"mov.u64 %0, " << texRef << ";\\n\" : \"=l\"(tex));" << std::endl;
						
					if(architecture == cuda::Architecture::FERMI)
						stream << "\t\ttex += texOffset * 0x800000;" << std::endl;
					else
						stream << "\t\ttex -= texOffset;" << std::endl;

					// get value
					stream << "\t\t__TexValue value;" << std::endl;
					stream << "\t\tasm(" << :: std::endl;
					stream << "\t\t\t\"{\\n\"" << std::endl;
					stream << "\t\t\t\".reg .u32 %%dummy<2>;\\n\"" << std::endl;
					stream << "\t\t\t\"tex.1d.v4.s32.s32 {%0, %1, %%dummy0, %%dummy1}, [%2, {%3}];\\n\"" << std::endl;
					stream << "\t\t\t\"}\\n\"" << std::endl;
					stream << "\t\t\t: \"=r\"(value.i[0]), \"=r\"(value.i[1]) : \"l\"(tex), \"r\"(texIndex)" << std::endl;
					stream << "\t\t);" << std::endl;

					stream << "\t\tif(sizeof(T) == 1) {" << std::endl;
					stream << "\t\t\treturn value.c[byteIndex % 4];" << std::endl;
					stream << "\t\t} else if(sizeof(T) == 2) {" << std::endl;
					stream << "\t\t\treturn value.s[(byteIndex % 4) / 2];" << std::endl;
					stream << "\t\t} else if(sizeof(T) == 4) {" << std::endl;
					stream << "\t\t\treturn *((T*)&value.i[0]);" << std::endl;
					stream << "\t\t} else if(sizeof(T) == 8) {" << std::endl;
					stream << "\t\t\treturn *((T*)&value.l);" << std::endl;
					stream << "\t\t}" << std::endl;
					stream << "\t}" << std::endl;
				}
			}

			stream << "\tassert(0);" << std::endl << "\t__trap();" << std::endl << "\treturn 0;" << std::endl << "}" << std::endl << std::endl;
		}
	} 
	// LDG TEXTURES
	else {
		for(const auto& it : textures) {
			const Degree* degree = it.first;
			const Array* array   = degree->getArray();

			// generate name dummy
			std::string name(array->getName());
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);

			stream << "template<typename T> __device__ __forceinline__ T __" << name << "_TEX(const uint32_t PID, const T* ptr, const T* root) {" << std::endl;
			stream << "\treturn __ldg(ptr);" << std::endl;
			stream << "}" << std::endl;

			stream << "template<> __device__ __forceinline__ double __" << name << "_TEX(const uint32_t PID, const double* ptr, const double* root) {" << std::endl;
			stream << "\tconst uint2 value = __ldg((const uint2*)ptr);" << std::endl;
			stream << "\treturn *(double*)&value;" << std::endl;
			stream << "}" << std::endl;

			stream << "template<> __device__ __forceinline__ int64_t __" << name << "_TEX(const uint32_t PID, const int64_t* ptr, const int64_t* root) {" << std::endl;
			stream << "\tconst uint2 value = __ldg((const uint2*)ptr);" << std::endl;
			stream << "\treturn *(int64_t*)&value;" << std::endl;
			stream << "}" << std::endl;

			stream << "template<> __device__ __forceinline__ uint64_t __" << name << "_TEX(const uint32_t PID, const uint64_t* ptr, const uint64_t* root) {" << std::endl;
			stream << "\tconst uint2 value = __ldg((const uint2*)ptr);" << std::endl;
			stream << "\treturn *(uint64_t*)&value;" << std::endl;
			stream << "}" << std::endl;
		}
	}

	//---------------------------------------------------------------
	// CONSTANT MEMORY
	//---------------------------------------------------------------
	{
		// init streams
		std::ostringstream includes;
		std::ostringstream variables;
		std::ostringstream functions;

		// pointer functions
		for(const auto& it : textures) {
			const Degree* degree = it.first;
			const Array* array = degree->getArray();

			// only if 3, then it is constant!
			if(degree->getValueCount() != 3)
				continue;

			// includes
			includes << "#include \"matog/" << array->getName() << "Type.cu\"" << std::endl;
			
			// init function
			functions << "__device__ __forceinline__ const " << array->getName() << "Type" << (array->isStruct() ? "::ptr&" : "*") << " __" << array->getName() << "_CONSTANT(const uint32_t PID) {" << std::endl;

			// for each paramId
			for(const uint32_t paramId : it.second) {
				Memory memory = (Memory)getVariant().get(hash, array->getRootMemory(), paramId);
				memory = Memory::CONSTANT;

				// variable
				if(memory == Memory::CONSTANT) {
					variables << "__constant__ " << array->getName() << "Type";
				
					if(array->isStruct()) {
						Layout layout = (Layout)getVariant().get(hash, array->getRootLayout(), paramId);
						if(layout == Layout::SOA)
							variables << "::__soa";
						else if(layout == Layout::AOS)
							variables << "::__aos";
						else if(layout == Layout::AOSOA)
							variables << "::__aosoa";
					}
				
					variables << " __" << array->getName() << "_CONSTANT_" << paramId;

					for (uint32_t i = 0; i < array->getRootDimCount(); i++)
						variables << "[" << array->getRootSizes()[i] << "]";

					variables << ";" << std::endl;
				}
				
				// functions
				functions << "\tif(PID == " << paramId << ")" << std::endl;
				if(memory == Memory::CONSTANT)
					if(array->isStruct()) {
						functions << "\t\treturn *(" << array->getName() << "Type::ptr*)__" << array->getName() << "_CONSTANT_" << paramId << ";";
					} else {
						functions << "\t\treturn (const " << array->getName() << "Type*)__" << array->getName() << "_CONSTANT_" << paramId << ";";
					}
				else
					functions << "\t\t{ assert(0); __trap(); return 0; }";

				functions << std::endl;
			}

			// end function
			functions << "\tassert(0);" << std::endl;
			functions << "\t__trap();" << std::endl;
			functions << "\treturn 0;" << std::endl;
			functions << "}" << std::endl << std::endl;
		}

		stream << includes.str() << std::endl;
		stream << variables.str() << std::endl;
		stream << functions.str() << std::endl;
	}

	//---------------------------------------------------------------
	// COUNTS
	//---------------------------------------------------------------
	for(const auto& it : globalArrays) {
		const Array* array = it.first;
		const auto& set = it.second;

		stream << "__device__ __forceinline__ uint32_t __" << array->getName() << "_COUNT(const uint32_t PID, const uint32_t dim) {" << std::endl;
		
		const uint32_t dimCnt = array->isCube() ? 1 : array->getRootDimCount();

		for(const uint32_t pid : set) {
			stream << "\tif(PID == " << pid << ") {" << std::endl;

			const uint64_t* ptr = m_countHints.getCounts(array->getId(), pid);

			for(uint32_t dim = 0; dim < dimCnt; dim++) {
				stream << "\t\tif(dim == " << dim << ") return " << (ptr == 0 ? 0 : ptr[dim]) << ";" << std::endl;
			}

			stream << "\t}" << std::endl;
		}

		stream << "\tassert(0);" << std::endl;
		stream << "\t__trap();" << std::endl;
		stream << "\treturn 0;" << std::endl;
		stream << "}" << std::endl << std::endl;
	}

	//---------------------------------------------------------------
	// REFS
	//---------------------------------------------------------------
	std::ostringstream tmp;

	for(const auto& it : globalArrays) {
		const Array* array = it.first;
		const auto& set = it.second;
		const char* indexType = array->is32BitIndex() ? "uint32_t" : "uint64_t";

		std::ostringstream s1;
		std::ostringstream s2;

		s1 << "__device__ __forceinline__ bool __" << array->getName() << "_COUNT_REF(const uint32_t PID, const uint32_t dim) {" << std::endl;
		s2 << "__device__ __forceinline__ " << indexType << " __" << array->getName() << "_COUNT_VALUE(const uint32_t PID, const uint32_t dim) {" << std::endl;

		const uint32_t dimCnt = array->isCube() ? 1 : array->getRootDimCount();

		for(const uint32_t pid : set) {
			s1 << "\tif(PID == " << pid << ") {" << std::endl;
			s2 << "\tif(PID == " << pid << ") {" << std::endl;

			for(uint32_t dim = 0; dim < dimCnt; dim++) {
				s1 << "\t\tif(dim == " << dim << ") { " << std::endl;
								
				const kernel::hint::RefHints::Mapping mapping = getRefHints().getMapping(array->getId(), pid, dim);

				if(mapping.arrayId != 0) {
					const Array* otherArray = &Static::getArray(mapping.arrayId);
					const uint32_t offset = otherArray->getRootPtrCount() * sizeof(CUdeviceptr) + mapping.dim * (otherArray->is32BitIndex() ? sizeof(uint32_t) : sizeof(uint64_t));

					s2 << "\t\tif(dim == " << dim << ") { " << std::endl;
					s2 << "\t\t\t" << indexType << " value;" << std::endl;
					s2 << "\t\t\tasm(\"ld.param.u" << (array->is32BitIndex() ? 32 : 64) << " %0, [" << getName() << "_param_" << mapping.index << "+" << offset << "];\" : \"=" <<
						(array->is32BitIndex() ? 'r' : 'l') << "\"(value));" << std::endl;
					s2 << "\t\t\treturn value;" << std::endl;
					s2 << "\t\t}" << std::endl;

					s1 << "\t\t\treturn true;" << std::endl;
				} else {
					s1 << "\t\t\treturn false;" << std::endl;
				}

				s1 << "\t\t}" << std::endl; 
			}

			s1 << "\t}" << std::endl;
			s2 << "\t}" << std::endl;
		}

		s1 << "\tassert(0);" << std::endl;
		s1 << "\t__trap();" << std::endl;
		s1 << "\treturn false;" << std::endl;
		s1 << "}" << std::endl << std::endl;

		s2 << "\tassert(0);" << std::endl;
		s2 << "\t__trap();" << std::endl;
		s2 << "\treturn 0;" << std::endl;
		s2 << "}" << std::endl << std::endl;

		tmp << s1.str() << s2.str();
	}

	// complete
	stream << std::endl << tmp.str();

	//---------------------------------------------------------------
	const std::string str = stream.str();
	//L_WARN("%s", str.c_str());
	return NEW_STRING(str.c_str());
}

//-------------------------------------------------------------------
bool Kernel::areDegreesInitialized(void) const {
	return !m_degrees.empty();
}

//-------------------------------------------------------------------
}