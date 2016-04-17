#include "SymbolDB.h"
#include "InterceptorUtils.h"
#include "Logger.h"
#include "SymbolResolver.h"
#include <dbghelp.h>
using namespace Interceptor;
using namespace AceLogger;

SymbolDB::SymbolDB() {

}

SymbolDB::~SymbolDB() {
	for (auto f : m_symbol_files) {
		delete f;
	}
}

void SymbolDB::init(HANDLE _handle) {
	m_symbol_resolver.init(static_cast<void*>(_handle));
}

void SymbolDB::cache_all_data(const InterceptorConfiguration &_config) {

	Log("Constructing SymbolDB...");
	std::vector<Symbol_native> symbols;
	std::vector<Module_native> modules;
	m_symbol_resolver.load_all_symbols(symbols, modules);
	load_modules(modules);
	load_symbols(symbols, _config);
	load_symbol_files(_config);
	std::stringstream str;
	str << m_modules.size();
	Log("Loaded total of " + str.str() + " modules");
	str.str("");
	str << m_symbols.size();
	Log("Loaded total of " + str.str() + " symbols");
	str.str("");
	str << m_symbol_files.size();
	Log("Loaded total of " + str.str() + " files");

}

std::string SymbolDB::get_string_from_id(const string_id &_id) {
	return m_string_indexer[_id];
}

string_id SymbolDB::get_id(const std::string &_str) {
	return m_string_indexer.record(_str);
}

std::string SymbolDB::get_most_relevant_module_name(const std::string &_fn_name) {
	auto index = _fn_name.find_last_of("::");
	if (index == std::string::npos)
		return _fn_name;
	auto m = _fn_name.substr(0, index - 1);
	index = m.find_last_of("::");
	if (index == std::string::npos)
		return m;
	m = m.substr(index + 1);
	index = m.find("`");
	if(index!=std::string::npos)
		m.erase(index, 1);
	index = m.find("'");
	if (index != std::string::npos)
		m.erase(index, 1);
	return m;
}

void SymbolDB::load_modules(const std::vector<Module_native> &_modules) {
	for (auto& m : _modules) {
		Module module;
		module.p_module_name = m_string_indexer.record(m.p_name);
		module.p_base_address = m.p_base_address;
		m_modules.insert(module);
	}
}

void SymbolDB::load_symbols(const std::vector<Symbol_native> &_symbols,
	const InterceptorConfiguration &_config){
	for (auto& m : _symbols) {
		Symbol symbol;
		auto fn=_config.get_function_normal_name(m.p_name);
		fn = get_most_relevant_module_name(fn);
		symbol.p_name = m_string_indexer.record(fn);
		symbol.p_address = m.p_address;
		symbol.p_address_upper =m.p_address+m.p_size;
		m_symbols.insert(symbol);
	}
}

void SymbolDB::load_symbol_files(const InterceptorConfiguration &_config) {
	
	for (auto& m : m_symbols) {
		SymbolFile symbol_file;
		auto fn_file = m_symbol_resolver.get_function_file_from_symbols_library(reinterpret_cast<void*>(m.p_address));
		fn_file = Utils::get_file_name_from_path(fn_file);
		fn_file = _config.get_file_normal_name(fn_file);
		auto iter = m_symbol_files.find(&symbol_file);
		if (iter == m_symbol_files.end()) {
			auto sym_file = new SymbolFile;
			sym_file->p_file_name = m_string_indexer.record(fn_file);
			m_symbol_files.insert(sym_file);
			m.p_symbol_file = sym_file;
		}
		else {
			m.p_symbol_file = (*iter);
		}
	}
}

string_id SymbolDB::get_symbol_id(void *_pa,
								const InterceptorConfiguration &_config) {
	Symbol symbol;
	symbol.p_address = reinterpret_cast<ULONG64>(_pa);
	auto iter = m_symbols.upper_bound(symbol);
	if (iter == m_symbols.end()) {
		//check with the last element
		auto rIter = m_symbols.rbegin();
		if (rIter!=m_symbols.rend() && (*rIter).p_address <= symbol.p_address && (*rIter).p_address_upper >= symbol.p_address) {
			return (*rIter).p_name;
		}
	}
	else {
		//check with the previous element
		if (iter != m_symbols.begin()) {
			iter--;
			if ((*iter).p_address <= symbol.p_address && (*iter).p_address_upper >= symbol.p_address) {
				return (*iter).p_name;
			}
		}
		
	}
	return load_symbol(_pa, _config)->p_name;
}

std::set<Symbol>::iterator SymbolDB::load_symbol(void *_pa,
	const InterceptorConfiguration &_config) {
	UniqueGuard lock(m_mutex);
	Symbol symbol;
	symbol.p_address = reinterpret_cast<ULONG64>(_pa);
	auto fn = m_symbol_resolver.get_function_name_from_symbols_library(_pa);
	fn = _config.get_function_normal_name(fn);
	fn = get_most_relevant_module_name(fn);
	symbol.p_name = m_string_indexer.record(fn);
	m_symbols.insert(symbol);
	auto iter = m_symbols.find(symbol);
	Log("unable to find the symbol " + fn + " in the SymbolDB ");
	return iter;
}

string_id SymbolDB::get_symbol_file_id(void *_pa,
	const InterceptorConfiguration &_config) {

	Symbol symbol;
	symbol.p_address = reinterpret_cast<ULONG64>(_pa);
	auto iter = m_symbols.upper_bound(symbol);
	if (iter == m_symbols.end()) {
		//check with the last element
		auto rIter = m_symbols.rbegin();
		if (rIter != m_symbols.rend() && (*rIter).p_address <= symbol.p_address && (*rIter).p_address_upper >= symbol.p_address) {
			return (*rIter).p_symbol_file->p_file_name;
		}
	}
	else {
		//check with the previous element
		if (iter != m_symbols.begin()) {
			iter--;
			if ((*iter).p_address <= symbol.p_address && (*iter).p_address_upper >= symbol.p_address) {
				return (*iter).p_symbol_file->p_file_name;
			}
		}
	}
	return load_symbol(_pa, _config)->p_symbol_file->p_file_name;
}
