#include "ConfigurationLoader.h"
#include "InterceptorUtils.h"

#include <fstream>
using namespace Interceptor;

ConfigurationLoader::ConfigurationLoader() {

	m_default_file_name = "interceptor.config";
	add_default_search_path();
}

void ConfigurationLoader::add_default_search_path() {
	auto directory =Utils::get_current_directory();

	m_search_paths.push(directory + "\\" + m_default_file_name);
}

void ConfigurationLoader::add_search_path(const std::string &_file_path) {
	m_search_paths.push(_file_path);
}

InterceptorConfiguration ConfigurationLoader::create_default_configuration() {
	InterceptorConfiguration config;
	config.p_mode = InterceptorMode::IMMEDIATE_PRINT;
	config.p_name_normalization = NamesNormalization::NORMALIZED;
	//normalized ignores
	config.p_suppress_function_names.push_back("std::");
	config.p_suppress_function_names.push_back("Concurrency::");
	config.p_suppress_function_names.push_back("<lambda_");
	config.p_suppress_function_names.push_back("operator new");
	config.p_suppress_function_names.push_back("boost::");
	
	config.p_function_normal["std::"] = "std_library";
	config.p_function_normal["Concurrency::"] = "Concurrency";
	config.p_function_normal["boost::"] = "boost";
	config.p_function_normal["<lambda_"] = "lambda_function";

	config.p_file_normal["c:\\program files"] = "std_library";
	config.p_record_mode = RecordMode::REALTIME;
	update_suppressed_container(config);

	return config;
}

void ConfigurationLoader::update_suppressed_container(InterceptorConfiguration &_config) {
	for (auto iter = _config.p_suppress_function_names.begin(); iter != _config.p_suppress_function_names.end(); ++iter) {
		auto innerIter = _config.p_function_normal.find(*iter);
		if (innerIter != _config.p_function_normal.end()) {
			(*iter) = (*innerIter).second;
		}
	}

	for (auto iter = _config.p_suppress_file_names.begin(); iter != _config.p_suppress_file_names.end(); ++iter) {
		auto innerIter = _config.p_file_normal.find(*iter);
		if (innerIter != _config.p_file_normal.end()) {
			(*iter) = (*innerIter).second;
		}
	}
	
}

InterceptorConfiguration ConfigurationLoader::get_configuration() {
	InterceptorConfiguration config = create_default_configuration();
	auto search_paths = m_search_paths;
	while (!search_paths.empty()) {
		auto file_path = search_paths.top();
		search_paths.pop();
		std::vector<KeyVal> key_vals;
		if (Utils::read_key_val_file(file_path, key_vals, '=', '#')) {
			while (!search_paths.empty()) {
				search_paths.pop();
			}
			config.p_file_normal.clear();
			config.p_function_normal.clear();
			config.p_suppress_file_names.clear();
			config.p_suppress_function_names.clear();
			update_configuration(config, key_vals);
		}
		
	}
	return config;
}

void ConfigurationLoader::update_configuration(InterceptorConfiguration &_config,
	std::vector<KeyVal> &_configuration) {

	for (auto s : _configuration) {
		if (s.is_key_i("InterceptorMode")) {
			update_interceptor_mode(s.val(), _config);
		}
		else if (s.is_key_i("RecordMode")) {
			update_record_mode(s.val(), _config);
		}
		else if (s.is_key_i("NameNormalization")) {
			update_name_normalization(s.val(), _config);
		}
		else if (s.is_key_i("FunctionNormal")) {
			update_function_normalization(s.val(), _config);
		}
		else if (s.is_key_i("FileNormal")) {
			update_file_normalization(s.val(), _config);
		}
		else if (s.is_key_i("SuppressFunction")) {
			update_suppressed_function(s.val(), _config);
		}
		else if (s.is_key_i("SuppressFile")) {
			update_suppressed_file(s.val(), _config);
		}	
	}
	update_suppressed_container(_config);
}

void ConfigurationLoader::update_interceptor_mode(const std::string &_mode,
	InterceptorConfiguration &_config) {
	auto mode = Utils::get_lower(_mode);
	for (auto i = 0; i < InterceptorMode::TOTAL_INTERCEPTOR_MODES; i++) {
		if (Utils::get_lower(InterceptorModeStr[i]) == mode) {
			_config.p_mode = static_cast<InterceptorMode>(i);
			break;
		}
	}
}

void ConfigurationLoader::update_record_mode(const std::string &_mode,
	InterceptorConfiguration &_config) {
	auto mode = Utils::get_lower(_mode);
	for (auto i = 0; i < RecordMode::TOTAL_RECORD_MODES; i++) {
		if (Utils::get_lower(RecordModeStr[i]) == mode) {
			_config.p_record_mode = static_cast<RecordMode>(i);
			break;
		}
	}
}

void ConfigurationLoader::update_name_normalization(const std::string &_mode,
	InterceptorConfiguration &_config) {
	auto mode = Utils::get_lower(_mode);
	for (auto i = 0; i < NamesNormalization::TOTAL_FUNCTION_NAMES; i++) {
		if (Utils::get_lower(NamesNormalizationStr[i]) == mode) {
			_config.p_name_normalization = static_cast<NamesNormalization>(i);
			break;
		}
	}
}

void ConfigurationLoader::update_function_normalization(const std::string &_mode,
	InterceptorConfiguration &_config) {
	std::string key = "";
	std::string val = "";
	if (Utils::get_key_val(_mode, key, val, '!')) {
		_config.p_function_normal[key] = val;
	}
}

void ConfigurationLoader::update_file_normalization(const std::string &_mode,
	InterceptorConfiguration &_config) {
	std::string key = "";
	std::string val = "";
	if (Utils::get_key_val(_mode, key, val, '!')) {
		_config.p_file_normal[key] = val;
	}
}

void ConfigurationLoader::update_suppressed_function(const std::string &_mode,
	InterceptorConfiguration &_config) {
	_config.p_suppress_function_names.push_back(_mode);
}

void ConfigurationLoader::update_suppressed_file(const std::string &_mode,
	InterceptorConfiguration &_config) {
	_config.p_suppress_file_names.push_back(_mode);
}