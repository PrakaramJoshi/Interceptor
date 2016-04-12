#pragma once
#include<string>
#include<vector>
#include <map>
namespace Interceptor {
	enum InterceptorMode	{	IMMEDIATE_PRINT,	CALL_DIAGRAM_FUNCTION,	CALL_DIAGRAM_FILES,
								CALL_DEPENDENCY_FUNCTION
									,TOTAL_INTERCEPTOR_MODES };

	enum RecordMode		{	REALTIME,	LAZY,	NOT_RECORDING, TOTAL_RECORD_MODES };

	enum NamesNormalization { PURE, NORMALIZED, TOTAL_FUNCTION_NAMES };

	const std::string InterceptorModeStr[InterceptorMode::TOTAL_INTERCEPTOR_MODES] = { "IMMEDIATE_PRINT", "CALL_DIAGRAM_FUNCTION" ,"CALL_DIAGRAM_FILES",
																					"CALL_DEPENDENCY_FUNCTION" };

	const std::string RecordModeStr[RecordMode::TOTAL_RECORD_MODES] = { "REALTIME", "LAZY","NOT_RECORDING" };

	const std::string NamesNormalizationStr[NamesNormalization::TOTAL_FUNCTION_NAMES] = { "PURE","NORMALIZED" };

	struct InterceptorConfiguration {

		InterceptorMode						p_mode;

		RecordMode							p_record_mode;

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
