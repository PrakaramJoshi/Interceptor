#include "CallStackRecord.h"
#include <map>
using namespace Interceptor;

CallStackRecord::CallStackRecord(const STD_STRING &_function,
							CALL_STATUS _call_status) {
	m_function = _function;
	m_call_status = _call_status;
	create_normalized_name();
}

STD_STRING Interceptor::CallStackRecord::get_name() const {
	return m_function;
}

STD_STRING Interceptor::CallStackRecord::get_normalized_name() const {
	return m_normalized_name;
}

CALL_STATUS Interceptor::CallStackRecord::get_call_status() const {
	return m_call_status;
}

void CallStackRecord::create_normalized_name() {

	std::map<STD_STRING, STD_STRING> normalied_names;
#ifdef UNICODE
#ifdef _WIN64
	normalied_names["std::"]="std_lbrary";
	normalied_names["Concurrency::"]="std_library";
	normalied_names["<lambda_"]="lambda_function";
	normalied_names["boost::"] = "boost";

#else
	normalied_names[L"std::"] = L"std_library";
	normalied_names[L"Concurrency::"] = L"concurrency";
	normalied_names[L"<lambda_"] = L"lambda_function";
	normalied_names[L"boost::"] = L"boost";
#endif
#else
	normalied_names["std::"] = "std_lbrary";
	normalied_names["Concurrency::"] = "std_library";
	normalied_names["<lambda_"] = "lambda_function";
	normalied_names["boost::"] = "boost";
#endif
	for (auto &n : normalied_names) {
		if (m_function.find(n.first)!= STRING_NFOUND) {
			m_normalized_name = n.second;
			return;
		}
	}
	m_normalized_name = m_function;
}