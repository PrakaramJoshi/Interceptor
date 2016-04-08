#pragma once

#include <string>
#include <Windows.h>
namespace Interceptor {
	class SymbolResolver {

		// Flag to indicate the result of symbol initialization
		bool				m_bInitResult;

		HANDLE				m_current_process;

	public:

		SymbolResolver();

		void init(void *_pAddress);

		std::string get_function_name_from_symbols_library(void *_pa);

		std::string get_function_file_from_symbols_library(void *_pa);
	};
}
