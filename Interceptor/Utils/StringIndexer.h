#pragma once
#include <map>
#include <string>
#include <map>
#define _STRING_INDEXER_SHOW_STRING
namespace Interceptor {
#ifdef _STRING_INDEXER_SHOW_STRING
	using string_id = std::string;
#else
	using string_id = std::size_t;
#endif;

	class StringIndexer {

		std::map<std::string, string_id> m_string_to_id;

		std::map<string_id,std::string> m_id_to_string;

		string_id m_current_id;

		std::size_t m_redundancy;

		string_id get_next_id(const std::string &_str);

	public:

		StringIndexer();

		~StringIndexer();

		void print_stats()const;

		string_id record(const std::string &_str);

		std::string& get(const string_id &_id);

		std::string& operator[](const string_id &_id);
	};
	
}
