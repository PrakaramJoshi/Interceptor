#include "SymbolDB.h"
#include "IdGen.h"
#include "Logger.h"
#include "SymbolResolver.h"
#include <dbghelp.h>
using namespace Interceptor;
using namespace AceLogger;
SymbolDB::SymbolDB() {

}
void SymbolDB::init(SymbolResolver &_symbol_resolver,
					StringIndexer &_string_indexer,
					const InterceptorConfiguration &_config) {
	Log("Constructing SymbolDB...");
	std::vector<Symbol_native> symbols;
	std::vector<Module_native> modules;
	_symbol_resolver.load_all_symbols(symbols, modules);
	load_modules(modules, _string_indexer);
	load_symbols(symbols, _string_indexer, _config);
	std::stringstream str;
	str << m_modules.size();
	Log("Loaded total of " + str.str() + " modules");
	str.str("");
	str << m_symbols.size();
	Log("Loaded total of " + str.str() + " symbols");
}

std::string SymbolDB::get_most_relevant_module_name(const std::string &_fn_name) {
	auto index = _fn_name.find_last_of("::");
	if (index == std::string::npos)
		return _fn_name;
	auto m = _fn_name.substr(0, index - 1);
	index = m.find_last_of("::");
	if (index == std::string::npos)
		return m;
	return m.substr(index + 1);
}

void SymbolDB::load_modules(const std::vector<Module_native> &_modules,
	StringIndexer &_string_indexer) {
	for (auto& m : _modules) {
		Module module;
		module.p_id = m_module_id_gen.get_next_id();
		module.p_module_name = _string_indexer.record(m.p_name);
		module.p_base_address = m.p_base_address;
		m_modules.insert(module);
	}
}

void SymbolDB::load_symbols(const std::vector<Symbol_native> &_symbols,
	StringIndexer &_string_indexer,
	const InterceptorConfiguration &_config){
	for (auto& m : _symbols) {
		Symbol symbol;
		symbol.p_id = m_symbol_id_gen.get_next_id();
		auto fn=_config.get_function_normal_name(m.p_name);
		fn = get_most_relevant_module_name(fn);
		symbol.p_name = _string_indexer.record(fn);
		symbol.p_address = m.p_address;
		symbol.p_address_upper =m.p_address+m.p_size;
		m_symbols.insert(symbol);
	}
}

string_id SymbolDB::get_symbol_id(void *_pa, 
								SymbolResolver &_symbol_resolver,
								StringIndexer &_string_indexer,
								const InterceptorConfiguration &_config) {
	Symbol symbol;
	symbol.p_address = reinterpret_cast<ULONG64>(_pa);
	auto iter = m_symbols.upper_bound(symbol);
	if (iter == m_symbols.end()) {
		//check with the last element
		auto rIter = m_symbols.rbegin();
		if ((*rIter).p_address <= symbol.p_address && (*rIter).p_address_upper >= symbol.p_address) {
			return (*rIter).p_name;
		}
	}
	else {
		//check with the previous element
		iter--;
		if ((*iter).p_address <= symbol.p_address && (*iter).p_address_upper >= symbol.p_address) {
			return (*iter).p_name;
		}
	}
	
	UniqueGuard lock(m_mutex);
	auto fn = _symbol_resolver.get_function_name_from_symbols_library(_pa);
	symbol.p_id = m_symbol_id_gen.get_next_id();
	fn = _config.get_function_normal_name(fn);
	fn = get_most_relevant_module_name(fn);
	symbol.p_name = _string_indexer.record(fn);
	m_symbols.insert(symbol);
	Log("unable to find the symbol " + fn + " in the SymbolDB ");
	return symbol.p_name;
}
