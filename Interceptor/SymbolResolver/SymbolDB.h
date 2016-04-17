#pragma once
#include "IdGen.h"
#include "InterceptorConfig.h"
#include "NonRecursiveLock.h"
#include "SymbolStructures.h"
#include <set>
#include <vector>
namespace Interceptor {

	class SymbolResolver;

	class SymbolDB {

		IDGen m_module_id_gen;

		IDGen m_symbol_id_gen;

		NonRecursiveLock m_mutex;

		std::set<Symbol> m_symbols;

		std::set<Module> m_modules;

		void load_modules(const std::vector<Module_native> &_modules,
			StringIndexer &_string_indexer);

		void load_symbols(const std::vector<Symbol_native> &_symbols,
			StringIndexer &_string_indexer,
			const InterceptorConfiguration &_config);

		std::string get_most_relevant_module_name(const std::string &_fn_name);

	public:

		SymbolDB();

		void init(SymbolResolver &_symbol_resolver,
				StringIndexer &_string_indexer,
			const InterceptorConfiguration &_config);

		string_id get_symbol_id(void *_pa,
								SymbolResolver &_symbol_resolver,
								StringIndexer &_string_indexer,
								const InterceptorConfiguration &_config);
	};
}
