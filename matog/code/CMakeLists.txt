# Copyright (c) 2016 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
# Use of this source code is governed by the BSD 3-Clause license that can be
# found in the LICENSE file.

#------------------------------------------------------------------------------
# TEMPLATES
#------------------------------------------------------------------------------
SET(CODE_TPLS
	code/tpl/lorem.tpl
	code/tpl/empty.tpl
	code/tpl/name.tpl
	code/tpl/type.tpl
	code/tpl/index.tpl
	code/tpl/index_template.tpl
	code/tpl/values.tpl
	code/tpl/values_this.tpl
	code/tpl/values_other.tpl
	code/tpl/values_separator.tpl
	
	code/tpl/field.tpl
	code/tpl/field_recursive.tpl
	
	code/tpl/union_count.tpl
	code/tpl/union_end.tpl
	code/tpl/union_field.tpl
	code/tpl/union_type.tpl
	code/tpl/union_struct.tpl
	code/tpl/union_struct_end.tpl
	code/tpl/union_ustruct.tpl
	code/tpl/union_ustruct_end.tpl
	code/tpl/union_union.tpl
	code/tpl/union_union_end.tpl
	code/tpl/union_uunion.tpl
	code/tpl/union_uunion_end.tpl
	
	code/tpl/struct.tpl
	
	code/tpl/getter_field.tpl
	code/tpl/getter_vector.tpl
	code/tpl/soa_offset.tpl
	
	code/tpl/data_pointer_array.tpl
	code/tpl/data_pointer_struct.tpl
		
	code/tpl/ptr.tpl
	code/tpl/ptr_RW.tpl
	code/tpl/ptr_RW_soa.tpl
	code/tpl/ptr_RW_aos.tpl
	code/tpl/ptr_RW_aosoa.tpl
	code/tpl/ptr_RO.tpl
	code/tpl/ptr_RO_soa.tpl
	code/tpl/ptr_RO_aos.tpl
	code/tpl/ptr_RO_aosoa.tpl
	code/tpl/ptr_aos.tpl
	code/tpl/ptr_soa.tpl
	code/tpl/ptr_field.tpl
	code/tpl/ptr_dimensional.tpl
	code/tpl/ptr_dimensional2.tpl
	code/tpl/ptr_struct.tpl
	code/tpl/ptr_return.tpl
	
	code/tpl/ptr_idx.tpl
	code/tpl/ptr_idx2.tpl
	code/tpl/ptr_layout.tpl
	code/tpl/ptr_cond.tpl
	code/tpl/ptr_end.tpl
	
	code/tpl/accessor.tpl
	code/tpl/accessor_accessor.tpl
	code/tpl/accessor_field.tpl
	code/tpl/accessor_data.tpl
	code/tpl/accessor_init_accessor.tpl
	code/tpl/accessor_init_field.tpl
	code/tpl/accessor_init_func.tpl
	code/tpl/accessor_init_data.tpl
	code/tpl/accessor_init_data2.tpl
	code/tpl/accessor_init_data3.tpl
	code/tpl/accessor_init_value.tpl
	code/tpl/accessor_forward_accessor.tpl
	code/tpl/accessor_forward_data.tpl
	code/tpl/accessor_params_root.tpl
	code/tpl/accessor_params_parent.tpl
	code/tpl/accessor_params_value.tpl
	code/tpl/accessor_params_data.tpl
	code/tpl/accessor_operator_struct.tpl
	code/tpl/accessor_operator_value.tpl
	code/tpl/accessor_super_accessor.tpl
	code/tpl/accessor_operator_accessor.tpl
	code/tpl/accessor_operator_accessor_field.tpl
	code/tpl/accessor_operator_accessor_swap.tpl
	code/tpl/accessor_operator_accessor_swap_field.tpl
	code/tpl/accessor_operator_vector.tpl
	code/tpl/accessor_operator_root.tpl
	code/tpl/accessor_operator_root_field.tpl
	
	code/tpl/soa_pointer_struct.tpl
	code/tpl/aosoa.tpl
	code/tpl/aosoa_recursive.tpl
	code/tpl/aosoa_pointer_struct.tpl
	
	code/tpl/operator.tpl
	code/tpl/operator_L.tpl
	code/tpl/operator_T.tpl
	code/tpl/operator_single_set.tpl
	code/tpl/operator_single_set_struct.tpl

	code/tpl/device_global.tpl
	code/tpl/device_global_getter_field.tpl
	code/tpl/device_global_struct.tpl
	code/tpl/device_global_array.tpl
	code/tpl/device_global_union_pointer.tpl
	code/tpl/device_global_pointer_struct.tpl
	code/tpl/device_global_pointer_array.tpl
	code/tpl/device_global_constant_struct.tpl
	code/tpl/device_global_constant_array.tpl
	
	code/tpl/device_dyn.tpl
	code/tpl/device_dyn_ptr_R.tpl
	code/tpl/device_dyn_ptr_R_aos.tpl
	code/tpl/device_dyn_ptr_R_aosoa.tpl
	code/tpl/device_dyn_ptr_R_soa.tpl
	
	code/tpl/device_shared.tpl
	code/tpl/device_shared_count_default.tpl
	code/tpl/device_shared_count_custom.tpl
	code/tpl/device_shared_pointer_array.tpl
	code/tpl/device_shared_pointer_struct.tpl
	code/tpl/device_shared_ptr_R.tpl
	code/tpl/device_shared_ptr_R_aos.tpl
	code/tpl/device_shared_ptr_R_aosoa.tpl
	code/tpl/device_shared_ptr_R_soa.tpl
	code/tpl/device_shared_ptr_cond.tpl
	code/tpl/device_shared_ptr_end.tpl
	code/tpl/device_shared_ptr.tpl
	code/tpl/device_shared_getter_vector.tpl
	
	code/tpl/device_get_layout.tpl
	code/tpl/device_get_transposition.tpl
	code/tpl/device_get_memory.tpl
	
	code/tpl/host_header.tpl
	
	code/tpl/host_host.tpl
	code/tpl/host_host_calcSize_default.tpl
	code/tpl/host_host_calcSize_custom.tpl
	code/tpl/host_host_calcSizeByElements_array.tpl
	code/tpl/host_host_calcSizeByElements_struct.tpl
	code/tpl/host_host_initPtr_array.tpl
	code/tpl/host_host_initPtr_struct.tpl
	
	code/tpl/host_device.tpl
	code/tpl/host_device_initPtr_array.tpl
	code/tpl/host_device_initPtr_struct.tpl
	code/tpl/host_device_pointer_struct.tpl
	code/tpl/host_device_pointer_array.tpl
	
	code/tpl/host_dyn.tpl
	code/tpl/host_dyn_0_array.tpl
	code/tpl/host_dyn_0_struct.tpl
	
	code/tpl/getCopySize_array.tpl
	code/tpl/getCopySize_struct.tpl
		
	code/tpl/count.tpl
	code/tpl/count_T.tpl
	code/tpl/count_T_root.tpl
	code/tpl/count_T_root_hints.tpl
	code/tpl/element_count_default.tpl
	code/tpl/element_count_custom.tpl
	
	code/tpl/host_header_get_layout.tpl
	code/tpl/host_header_get_transposition.tpl
		
	code/tpl/copy_dim.tpl
	code/tpl/copy_field.tpl
	
	code/tpl/compare_dim.tpl
	code/tpl/compare_int.tpl
	code/tpl/compare_float.tpl
	
	code/tpl/CMakeLists.tpl
)

SET(FINDMATOG
	code/tpl/FindMATOG.cmake
)

SOURCE_GROUP("Template Files" FILES ${CODE_TPLS})
SOURCE_GROUP("FindMATOG" FILES ${FINDMATOG})

EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/tpl")

#------------------------------------------------------------------------------
# EXECUTABLE
#------------------------------------------------------------------------------
SET(SRC
	code/main.cpp
	code/MatogBuilder.cpp
	code/FileParser.cpp
	code/CodeModel.cpp
	code/CMakeModel.cpp
	code/CodeVar.cpp
	code/CodeRange.cpp
	code/CodeDefine.cpp
	code/CodeHint.cpp
	code/CodeHint.h
	code/Header.cpp
	code/Template.cpp
	code/MatogBuilder.h
	code/FileParser.h
	code/CodeModel.h
	code/CodeDefine.h
	code/CodeRange.h
	code/CMakeModel.h
	code/CodeVar.h
	code/Header.h
	code/Template.h
	code/CodeTemplate.h
	code/CodeTemplate.cpp
	code/Global.h
	code/Global.cpp
	code/Shared.h
	code/Shared.cpp
	code/Dyn.h
	code/Dyn.cpp
	code/Host.h
	code/Host.cpp
	code/CodeHelper.h
	code/CodeHelper.cpp
	code/CodeHelper_Accessors.cpp
	code/CodeHelper_Ident.cpp
	code/CodeHelper_Name.cpp
	code/CodeHelper_Index.cpp
	code/CodeHelper_Type.cpp
	code/CodeHelper_Values.cpp
	code/CodeHelper_FieldType.cpp
)

SOURCE_GROUP("code" FILES ${SRC})

LIST(APPEND CODE_SRC ${SRC})