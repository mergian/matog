// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/log.h>
#include <sqlite3/sqlite3.h>

namespace matog {
	namespace log {
//-------------------------------------------------------------------
Result::Result(void) : m_errorNumber(0), m_error("UNKNOWN_ERROR"), m_message(0) {
}

//-------------------------------------------------------------------
Result::Result(CUresult cuda) : m_errorNumber((int)cuda), m_error(0), m_message(0) {
	if(cuda != CUDA_SUCCESS) {
		cuGetErrorName(cuda, &m_error);
		cuGetErrorString(cuda, &m_message);
	}
}

//-------------------------------------------------------------------
Result::Result(CUptiResult cupti) : m_errorNumber((int)cupti), m_error(0), m_message(0) {
	if(cupti != CUPTI_SUCCESS) {
		#if CUPTI_API_VERSION >= 6 // requires cuda 6.5 
		cuptiGetResultString(cupti, &m_message);
		#endif

		m_error = getCUPTIErrorString(cupti);
	}
}

//-------------------------------------------------------------------
Result::Result(sqliteResult sql, void* db) : m_errorNumber(sql), m_error(0), m_message(0) {
	if(sql != SQLITE_OK && sql != SQLITE_DONE && sql != SQLITE_ROW) {
		m_error = sqlite3_errstr(sql);
		m_message = sqlite3_errmsg((sqlite3*)db);
	}
}

//-------------------------------------------------------------------
Result::Result(const char* error) : m_errorNumber(0), m_error(error), m_message(0) {
}

//-------------------------------------------------------------------
Result::Result(const char* error, const char* message) : m_errorNumber(0), m_error(error), m_message(message) {
}

//-------------------------------------------------------------------
void Result::print(void) const {
	C_ERROR(RED, "%u: %s @ %s (%u) %s", m_errorNumber, m_error, m_file, m_line, m_message ? m_message : "");
}

//-------------------------------------------------------------------
const char* Result::getCUPTIErrorString(const CUptiResult error) {
	switch(error) {
	case CUPTI_SUCCESS:														return "No error.";
	case CUPTI_ERROR_INVALID_PARAMETER:										return "One or more of the parameters is invalid.";
	case CUPTI_ERROR_INVALID_DEVICE:										return "The device does not correspond to a valid CUDA device.";
	case CUPTI_ERROR_INVALID_CONTEXT:										return "The context is NULL or not valid.";
	case CUPTI_ERROR_INVALID_EVENT_DOMAIN_ID:								return "The event domain id is invalid.";
	case CUPTI_ERROR_INVALID_EVENT_ID:										return "The event id is invalid.";
	case CUPTI_ERROR_INVALID_EVENT_NAME:									return "The event name is invalid.";
	case CUPTI_ERROR_INVALID_OPERATION:										return "The current operation cannot be performed due to dependency on other factors.";
	case CUPTI_ERROR_OUT_OF_MEMORY:											return "Unable to allocate enough memory to perform the requested operation.";
	case CUPTI_ERROR_HARDWARE:												return "An error occurred on the performance monitoring hardware.";
	case CUPTI_ERROR_PARAMETER_SIZE_NOT_SUFFICIENT:							return "The output buffer size is not sufficient to return all requested data.";
	case CUPTI_ERROR_API_NOT_IMPLEMENTED:									return "API is not implemented.";
	case CUPTI_ERROR_MAX_LIMIT_REACHED:										return "The maximum limit is reached.";
	case CUPTI_ERROR_NOT_READY:												return "The object is not yet ready to perform the requested operation.";
	case CUPTI_ERROR_NOT_COMPATIBLE:										return "The current operation is not compatible with the current state of the object.;";
	case CUPTI_ERROR_NOT_INITIALIZED:										return "CUPTI is unable to initialize its connection to the CUDA driver.";
	case CUPTI_ERROR_INVALID_METRIC_ID:										return "The metric id is invalid.";
	case CUPTI_ERROR_INVALID_METRIC_NAME:									return "The metric name is invalid.";
	case CUPTI_ERROR_QUEUE_EMPTY:											return "The queue is empty.";
	case CUPTI_ERROR_INVALID_HANDLE:										return "Invalid handle (internal?).";
	case CUPTI_ERROR_INVALID_STREAM:										return "Invalid stream.";
	case CUPTI_ERROR_INVALID_KIND:											return "Invalid kind.";
	case CUPTI_ERROR_INVALID_EVENT_VALUE:									return "Invalid event value.";
	case CUPTI_ERROR_DISABLED:												return "CUPTI is disabled due to conflicts with other enabled profilers";
	case CUPTI_ERROR_INVALID_MODULE:										return "Invalid module.";
	case CUPTI_ERROR_INVALID_METRIC_VALUE:									return "Invalid metric value.";
	case CUPTI_ERROR_HARDWARE_BUSY:											return "The performance monitoring hardware is in use by other client.";
	case CUPTI_ERROR_NOT_SUPPORTED:											return "The attempted operation is not supported on the current system or device.";
	#if CUPTI_API_VERSION >= 8
	case CUPTI_ERROR_UM_PROFILING_NOT_SUPPORTED:							return "Unified memory profiling is not supported on the system. Potential reason could be unsupported OS or architecture.";
	case CUPTI_ERROR_UM_PROFILING_NOT_SUPPORTED_ON_DEVICE:					return "Unified memory profiling is not supported on the device";
	case CUPTI_ERROR_UM_PROFILING_NOT_SUPPORTED_ON_NON_P2P_DEVICES:			return "Unified memory profiling is not supported on a multi-GPU configuration without P2P support between any pair of devices";
	case CUPTI_ERROR_UM_PROFILING_NOT_SUPPORTED_WITH_MPS:					return "Unified memory profiling is not supported under the Multi-Process Service (MPS) environment";
	#endif
	case CUPTI_ERROR_UNKNOWN:												return "An unknown internal error has occurred.";
	case CUPTI_ERROR_FORCE_INT:												return "An unknown internal error has occurred.";
	default:																return "unknown error";
	}
}

//-------------------------------------------------------------------
	}
}