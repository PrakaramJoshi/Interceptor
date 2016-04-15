#include "CallStackRecord.h"
#include "Interceptor_Internal.h"

using namespace Interceptor;

CallStackRecord::CallStackRecord(const string_id &_function_name,
								const string_id &_function_file_path,
							CALL_STATUS _call_status) {
	m_function = _function_name;
	m_call_status = _call_status;
	m_function_file_data = _function_file_path;
}

bool CallStackRecord::operator ==(const CallStackRecord &_record)const {
	return (m_function == _record.m_function) && 
			m_call_status == _record.m_call_status && 
		m_function_file_data == _record.m_function_file_data;
}

string_id CallStackRecord::get_function_name() const {
	return m_function;
}

string_id CallStackRecord::get_file_data()const {
	return m_function_file_data;
}


CALL_STATUS CallStackRecord::get_call_status() const {
	return m_call_status;
}
