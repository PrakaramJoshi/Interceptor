#include "CallStackRecord.h"
#include "Interceptor_Internal.h"

using namespace Interceptor;

CallStackRecord::CallStackRecord(const string_id &_function_name,
								const string_id &_function_file_path,
							CALL_STATUS _call_status) {
	m_function = _function_name;
	m_call_status = _call_status;
	m_function_file_path = _function_file_path;
}

string_id Interceptor::CallStackRecord::get_name() const {
	return m_function_file_path;
}


CALL_STATUS Interceptor::CallStackRecord::get_call_status() const {
	return m_call_status;
}
