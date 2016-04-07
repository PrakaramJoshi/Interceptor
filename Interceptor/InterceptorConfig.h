#pragma once
#include<string>
#include<vector>
namespace Interceptor {
	enum class InterceptorMode { IMMEDIATE_PRINT, CALL_DIAGRAM_FUNCTION,CALL_DIAGRAM_FILES };
	enum class FunctionNames { PURE, NORMALIZED };
	struct InterceptorConfiguration {

		InterceptorMode p_mode;

		std::vector<std::string> p_disabled_stuff;

		FunctionNames p_function_names;

		InterceptorConfiguration() {
			p_mode = InterceptorMode::CALL_DIAGRAM_FUNCTION;
			p_function_names = FunctionNames::NORMALIZED;
			//normalized ignores
			if (p_function_names == FunctionNames::NORMALIZED) {
				p_disabled_stuff.push_back("std_library");
				p_disabled_stuff.push_back("Concurrency");
				p_disabled_stuff.push_back("lambda_function");
				p_disabled_stuff.push_back("operator new");
				p_disabled_stuff.push_back("boost");
			}
			else {
				p_disabled_stuff.push_back("std::");
				p_disabled_stuff.push_back("Concurrency::");
				p_disabled_stuff.push_back("<lambda_");
				p_disabled_stuff.push_back("operator new");
				p_disabled_stuff.push_back("boost::");
			}

			
		}
	};
}
