#include "CallStackUtils.h"
using namespace Interceptor;
std::string Utils::get_normalized_function_name(const std::string &_function_name) {
	if (_function_name.find("std::") != std::string::npos) {
		return "std_library";
	}
	else if (_function_name.find("Concurrency::") != std::string::npos) {
		return "Concurrency";
	}
	else if (_function_name.find("boost::") != std::string::npos) {
		return "boost";
	}
	else if (_function_name.find("<lambda_") != std::string::npos) {
		return  "lambda_function";
	}
	else if (_function_name.find("c:\\program files") != std::string::npos) {
		return "std_library";
	}
	else {
		return _function_name;
	}
}