#include "StringIndexer.h"
#include <iostream>
using namespace Interceptor;

StringIndexer::StringIndexer() {
#ifdef _STRING_INDEXER_SHOW_STRING
#else
	m_current_id = 1;
#endif
	m_redundancy = 0;
}

StringIndexer::~StringIndexer() {
	
}

void StringIndexer::print_stats()const {
	std::cout << "StringIndexer Data" << std::endl;
	std::cout << "Total redundancy\t:\t" << m_redundancy << std::endl;
	std::cout << "Total strings\t\t:\t" << m_string_to_id.size() << std::endl;
}

string_id StringIndexer::get_next_id(const std::string &_str) {
	string_id returnVal;
#ifdef _STRING_INDEXER_SHOW_STRING
	returnVal = _str;
#else
	returnVal = m_current_id;
	m_current_id++;
#endif
	return returnVal;
}

string_id StringIndexer::record(const std::string &_str) {
	auto iter = m_string_to_id.find(_str);
	auto returnval = m_current_id;
	if (iter == m_string_to_id.end()) {
		string_id id = get_next_id(_str);
		m_string_to_id[_str] = id;
		m_id_to_string[id] = _str;
		returnval = id;
	}
	else {
		returnval = iter->second;
		m_redundancy++;
	}
	return returnval;
	
}

std::string& StringIndexer::get(const string_id &_id) {
	return m_id_to_string[_id];
}

std::string& StringIndexer::operator[](const string_id &_id) {
	return m_id_to_string[_id];
}