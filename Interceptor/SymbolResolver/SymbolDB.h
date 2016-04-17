#pragma once
#include "IdGen.h"
#include "InterceptorConfig.h"
#include "NonRecursiveLock.h"
#include "SymbolResolver.h"
#include "SymbolStructures.h"
#include <set>
#include <vector>
namespace Interceptor {

	class SymbolDB {

		NonRecursiveLock								m_mutex;

		StringIndexer									m_string_indexer;

		SymbolResolver									m_symbol_resolver;

		std::set<Symbol>								m_symbols;

		std::set<Module>								m_modules;

		std::set<SymbolFile*, SymbolFileComparator >	m_symbol_files;

		void load_modules(const std::vector<Module_native> &_modules);

		void load_symbols(const std::vector<Symbol_native> &_symbols,
			const InterceptorConfiguration &_config);

		void load_symbol_files(const InterceptorConfiguration &_config);

		std::set<Symbol>::iterator load_symbol(void *_pa,
			const InterceptorConfiguration &_config);

		std::string get_most_relevant_module_name(const std::string &_fn_name);

	public:

		SymbolDB();

		~SymbolDB();

		void init(HANDLE _handle);

		void cache_all_data(const InterceptorConfiguration &_config);

		string_id get_symbol_id(void *_pa,
								const InterceptorConfiguration &_config);

		string_id get_symbol_file_id(void *_pa,
			const InterceptorConfiguration &_config);

		string_id get_id(const std::string &_str);

		std::string get_string_from_id(const string_id &_id);
	};
}
