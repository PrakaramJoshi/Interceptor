#include "CallStackRecord.h"
#include "Interceptor_Internal.h"
#include <map>
using namespace Interceptor;

CallStackRecord::CallStackRecord(const std::string &_function_name,
							CALL_STATUS _call_status) {
	m_function = _function_name;
	m_call_status = _call_status;
}

std::string Interceptor::CallStackRecord::get_name() const {
	return m_function;
}


CALL_STATUS Interceptor::CallStackRecord::get_call_status() const {
	return m_call_status;
}
