#include "InterceptorUtils.h"
#include <algorithm>
#include <fstream>
#include <locale>
#include <Windows.h>
using namespace Interceptor;

std::string Utils::get_file_name_from_path(const std::string &_file_path) {
	auto file_name_begins_at = _file_path.find_last_of("\\");
	if (file_name_begins_at == std::string::npos) {
		file_name_begins_at = 0;
	}
	else {
		file_name_begins_at++;
	}
	auto file_name = _file_path.substr(file_name_begins_at);
	return file_name;
}

std::string Utils::get_current_directory() {
	char buffer[MAX_PATH];
	::GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	auto directory = std::string(buffer).substr(0, pos);
	return directory;
}

void Utils::split(const std::string &s, std::vector<std::string> &_tokens, const char delimiter) {
	size_t start = 0;
	size_t end = s.find_first_of(delimiter);

	while (end <= std::string::npos) {
		_tokens.emplace_back(s.substr(start, end - start));

		if (end == std::string::npos)
			break;

		start = end + 1;
		end = s.find_first_of(delimiter, start);
	}
}

void Utils::trim_leading_white_space(std::string &_str) {
	auto iter = _str.begin();
	std::locale loc;
	while (iter != _str.end()) {
		if (std::isspace(*iter, loc)) {
			iter = _str.erase(iter);
		}
		else {
			break;
		}
	}
}

void Utils::trim_trailing_white_space(std::string &_str) {
	std::reverse(_str.begin(), _str.end());
	trim_leading_white_space(_str);
	std::reverse(_str.begin(), _str.end());
}

void Utils::trim_leading_trailing_white_space(std::string &_str) {
	trim_leading_white_space(_str);
	trim_trailing_white_space(_str);
}

bool Utils::get_key_val(const std::string &_str, std::string &_key, std::string &_val, const char _delimiter) {
	std::vector<std::string>tokens;
	split(_str, tokens, _delimiter);
	if (tokens.size() != 2)
		return false;
	trim_leading_trailing_white_space(tokens[0]);
	trim_leading_trailing_white_space(tokens[1]);
	_key = tokens[0];
	_val = tokens[1];
	return true;
}

std::string Utils::get_lower(const std::string &_str) {
	std::string str = "";
	std::locale loc;
	
	std::for_each(_str.begin(), _str.end(), [&str, &loc](const char &_c) {
		auto lower_char = std::tolower(_c, loc);
		str.append(&lower_char, 1);
	});
	return str;
}

bool Utils::read_key_val_file(const std::string &_file_path, 
							std::vector<KeyVal> &_keyVals,
							const char _delimiter,
							const char _comment) {
	std::ifstream ifs(_file_path);
	if (ifs.is_open()) {
		std::string line;
		while (std::getline(ifs, line)) {
			if (!line.empty() || line[0] != _comment) {
				std::string key = "";
				std::string val = "";
				if (get_key_val(line, key, val, _delimiter)) {
					_keyVals.push_back(KeyVal(key, val));
				}
			}
		}
		return true;
	}
	return false;
}


KeyVal::KeyVal(const std::string &_key,
	const std::string &_val) {
	m_key = _key;
	m_val = _val;
	m_key_i = Utils::get_lower(m_key);
}

std::string KeyVal::key()const {
	return m_key;
}

std::string KeyVal::val()const {
	return m_val;
}

bool KeyVal::is_key(const std::string &_key)const {
	return m_key == _key;
}

bool KeyVal::is_key_i(const std::string &_key)const {
	auto k = Utils::get_lower(_key);
	return m_key_i == k;
}