#pragma once
#include <map>
#include <string>
#include <map>
namespace Interceptor {
	using string_id = std::size_t;

	class StringIndexer {

		std::map<std::string, string_id> m_string_to_id;

		std::map<string_id,std::string> m_id_to_string;

		std::size_t m_current_id;

	public:

		StringIndexer();

		string_id record(const std::string &_str);

		std::string& get(const string_id &_id);

		std::string& operator[](const string_id &_id);
	};
	
}
