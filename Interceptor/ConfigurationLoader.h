#pragma once
#include "InterceptorConfig.h"
#include "InterceptorUtils.h"
#include <stack>

namespace Interceptor {
	class ConfigurationLoader {
		std::stack<std::string> m_search_paths;

		std::string m_default_file_name;

		void add_default_search_path();

		InterceptorConfiguration create_default_configuration();

		void update_configuration(InterceptorConfiguration &_config,
			std::vector<KeyVal> &_configuration);

		void update_interceptor_mode(const std::string &_mode,
			InterceptorConfiguration &_config);

		void update_record_mode(const std::string &_mode,
			InterceptorConfiguration &_config);

		void update_record_type(const std::string &_mode,
			InterceptorConfiguration &_config);

		void update_name_normalization(const std::string &_mode,
			InterceptorConfiguration &_config);

		void update_function_normalization(const std::string &_mode,
			InterceptorConfiguration &_config);

		void update_file_normalization(const std::string &_mode,
			InterceptorConfiguration &_config);

		void update_suppressed_function(const std::string &_mode,
			InterceptorConfiguration &_config);

		void update_suppressed_file(const std::string &_mode,
			InterceptorConfiguration &_config);

		void update_suppressed_container(InterceptorConfiguration &_config);

	public:
		ConfigurationLoader();

		void add_search_path(const std::string &_file_path);

		InterceptorConfiguration get_configuration();
	};
};
