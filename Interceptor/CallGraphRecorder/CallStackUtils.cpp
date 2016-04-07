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

std::string Utils::get_file_name_from_path(const std::string &_file_path) {
	auto file_name_begins_at= _file_path.find_last_of("\\");
	if (file_name_begins_at == std::string::npos) {
		file_name_begins_at = 0;
	}
	else {
		file_name_begins_at++;
	}
	auto file_name = _file_path.substr(file_name_begins_at);
	return file_name;
}