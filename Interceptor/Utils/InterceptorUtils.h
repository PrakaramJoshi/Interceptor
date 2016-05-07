#pragma once
#include <sstream>
#include <string>
#include <vector>

namespace Interceptor {

	

	class KeyVal {
		std::string m_key;

		std::string m_val;

		std::string m_key_i;

		KeyVal() = delete;

	public:

		KeyVal(const std::string &_key,
			const std::string &_val);

		std::string key()const;

		std::string val()const;

		bool is_key(const std::string &_key)const;

		bool is_key_i(const std::string &_key)const;

	};

	class Utils {
	public:

		static std::string get_file_name_from_path(const std::string &_file_path);

		static std::string get_current_directory();

		static void split(const std::string &_str,
						std::vector<std::string> &_tokens,
						const char delimiter);

		static bool read_key_val_file(const std::string &_file_path,
									std::vector<KeyVal> &_keyVals,
									const char _delimiter,
									const char _comment);

		static bool get_key_val(const std::string &_str, 
								std::string &_key, 
								std::string &_val,	
								const char delimiter);

		static std::string get_lower(const std::string &_str);

		static void trim_leading_white_space(std::string &_str);

		static void trim_trailing_white_space(std::string &_str);

		static void trim_leading_trailing_white_space(std::string &_str);

		static void replace_all(std::string& _str, 
								const std::string& _oldStr, 
								const std::string& _newStr);

		template<typename T>
		static std::string get_string(const T& _obj) {
			std::stringstream str;
			str << _obj;
			return str.str();
		}
	};
}
#pragma once
