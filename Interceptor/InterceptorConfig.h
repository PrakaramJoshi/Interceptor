#pragma once
#include<string>
#include<vector>
#include <map>
namespace Interceptor {
	enum InterceptorMode	{	IMMEDIATE_PRINT,	FORCE_DIAGRAM,	DEPENDENCY_WHEEL
									, TIMELINE,
								TOTAL_INTERCEPTOR_MODES };

	enum RecordMode		{	REALTIME,	LAZY,	NOT_RECORDING,PRELOAD_FUNCTIONS, TOTAL_RECORD_MODES };

	enum NamesNormalization { PURE, NORMALIZED, TOTAL_FUNCTION_NAMES };

	enum RecordType { FUNCTION, FILE, TOTAL_RECORD_TYPES };

	const std::string InterceptorModeStr[InterceptorMode::TOTAL_INTERCEPTOR_MODES] = { "IMMEDIATE_PRINT", "FORCE_DIAGRAM" ,"DEPENDENCY_WHEEL",
																						"TIMELINE"};

	const std::string RecordModeStr[RecordMode::TOTAL_RECORD_MODES] = { "REALTIME", "LAZY","NOT_RECORDING","PRELOAD_FUNCTIONS" };

	const std::string NamesNormalizationStr[NamesNormalization::TOTAL_FUNCTION_NAMES] = { "PURE","NORMALIZED" };

	const std::string RecordTypeStr[RecordType::TOTAL_RECORD_TYPES] = { "FUNCTION", "FILE" };

	struct InterceptorConfiguration {

		InterceptorMode						p_mode;

		RecordMode							p_record_mode;

		RecordType							p_record_type;

		std::vector<std::string>			p_suppress_function_names;
		
		std::vector<std::string>			p_suppress_file_names;

		std::map<std::string, std::string>	p_function_normal;

		std::map<std::string, std::string>	p_file_normal;

		NamesNormalization					p_name_normalization;

		InterceptorConfiguration() {
				
		}

		std::string get_function_normal_name(const std::string &_function_name)const {
			if (p_name_normalization == NamesNormalization::PURE)
				return _function_name;
			for (auto &f : p_function_normal) {
				if (_function_name.find(f.first) != std::string::npos) {
					return f.second;
				}
			}
			return _function_name;
		}

		std::string get_file_normal_name(const std::string &_file_name)const {
			if (p_name_normalization == NamesNormalization::PURE)
				return _file_name;
			for (auto &f : p_file_normal) {
				if (_file_name.find(f.first) != std::string::npos) {
					return f.second;
				}
			}
			return _file_name;
		}
	};
}
