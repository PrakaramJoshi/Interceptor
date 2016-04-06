#include "StringIndexer.h"
using namespace Interceptor;

StringIndexer::StringIndexer() {
	m_current_id = 1;
}

string_id StringIndexer::record(const std::string &_str) {
	auto iter = m_string_to_id.find(_str);
	auto returnval = m_current_id;
	if (iter == m_string_to_id.end()) {
		m_string_to_id[_str] = m_current_id;
		m_id_to_string[m_current_id] = _str;
		m_current_id++;
	}
	else {
		returnval = iter->second;
	}
	return returnval;
	
}

std::string& StringIndexer::get(const string_id &_id) {
	return m_id_to_string[_id];
}

std::string& StringIndexer::operator[](const string_id &_id) {
	return m_id_to_string[_id];
}