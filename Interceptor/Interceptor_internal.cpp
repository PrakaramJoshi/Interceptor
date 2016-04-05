#include "stdafx.h"
#include "Interceptor_Internal.h"
#include "CallStackUtils.h"
#include <imagehlp.h>
#include <iostream>
#include <string>
#include <strsafe.h>

using namespace Interceptor;

Interceptor_Internal::Interceptor_Internal() {
	m_bInitResult = FALSE;
	m_current_process = 0;
	m_mutex_available = true;	
}

Interceptor_Internal::~Interceptor_Internal() {
	if (!m_mutex_available)
		return;
	SLOCK(m_called_func_mutex)
	SLOCK(m_print_mutex)
	m_mutex_available = false;
	switch (m_configuration.p_mode) {
		case Interceptor::InterceptorMode::IMMEDIATE_PRINT:
			break;
		case Interceptor::InterceptorMode::CALL_DIAGRAM:
			m_call_graph_recorder.create_call_chart();
			break;
		default:
			break;
	}
}

Interceptor_Internal& Interceptor_Internal::get() {
	static Interceptor_Internal instance;
	return instance;
}

void Interceptor_Internal::init(void *_pAddress) {
	Interceptor_Internal::get().init_internal(_pAddress);
}

void Interceptor_Internal::on_enter(void *_pa) {
	Interceptor_Internal::get().on_enter_internal(_pa);
}

void Interceptor_Internal::on_exit(void *_pa) {
	Interceptor_Internal::get().on_exit_internal(_pa);
}

void Interceptor_Internal::on_enter_immediate_print_mode(void *_pa) {
	auto fn_name = get_function_name_internal(_pa);
	auto current_depth = (++m_function_call_depth);
	print_to_console(current_depth, fn_name, true);
}

void Interceptor_Internal::on_exit_immediate_print_mode(void *_pa) {
	auto fn_name = get_function_name_internal(_pa);
	auto current_depth = m_function_call_depth.load();
	--m_function_call_depth;
	print_to_console(current_depth, fn_name, false);
}

void Interceptor_Internal::on_enter_call_diagram_mode(void *_pa) {
	auto fn_name = get_function_name_internal(_pa);
	m_call_graph_recorder.record(fn_name);
}

void Interceptor_Internal::on_exit_call_diagram_mode(void *_pa) {
	auto fn_name = get_function_name_internal(_pa);
	m_call_graph_recorder.record(fn_name,CALL_STATUS::CALL_OUT);
}

void Interceptor_Internal::on_enter_internal(void *_pa) {
	if (!m_mutex_available)
		return;
	switch (m_configuration.p_mode) {
		case Interceptor::InterceptorMode::IMMEDIATE_PRINT:
			on_enter_immediate_print_mode(_pa);
			break;
		case Interceptor::InterceptorMode::CALL_DIAGRAM:
			on_enter_call_diagram_mode(_pa);
			break;
		default:
			break;
	}
}

void Interceptor_Internal::on_exit_internal(void *_pa) {
	if (!m_mutex_available)
		return;
	switch (m_configuration.p_mode) {
		case Interceptor::InterceptorMode::IMMEDIATE_PRINT:
			on_exit_immediate_print_mode(_pa);
			break;
		case Interceptor::InterceptorMode::CALL_DIAGRAM:
			on_exit_call_diagram_mode(_pa);
			break;
		default:
			break;
	}	
}

void Interceptor_Internal::print_to_console(const std::size_t &_stack_depth,
											const std::string &_function_name, 
											bool _in) {
	if (_function_name.empty())
		return;
	for (auto &_disbled_stuff : m_configuration.p_disabled_stuff) {
		if (_function_name.find(_disbled_stuff) != std::string::npos) {
			return;
		}
	}
	std::string out_str = "";
	for (auto i = _stack_depth; i > 0; i--) {
		out_str.append("-");
	}
	out_str.append(_function_name);

	SLOCK(m_print_mutex)
	if (_in) {
		std::cout << "(in  ";
	}
	else {
		std::cout << "(out ";
	}
	std::cout << _stack_depth << 
				", Thread: " << 
				std::this_thread::get_id() << 
				")" << 
				out_str << std::endl;
}

//Function to load the symbols of the module
void Interceptor_Internal::init_internal(void *_pAddress) {
	if (m_bInitResult)
		return;
	//Query the memory
	char moduleName[MAX_PATH];
	MEMORY_BASIC_INFORMATION mbi;
	m_current_process = GetCurrentProcess();
	//Get the module name where the address is available
	VirtualQuery((void*)_pAddress, &mbi, sizeof(mbi));
	GetModuleFileNameA((HMODULE)mbi.AllocationBase,
		moduleName, MAX_PATH);

	//Initialize the symbols
	m_bInitResult = SymInitialize(m_current_process, moduleName, TRUE);

	//Set the options
	SymSetOptions(SymGetOptions()   &~SYMOPT_UNDNAME);
}
int filter(unsigned int code, struct _EXCEPTION_POINTERS *ep) {

	if (code == EXCEPTION_ACCESS_VIOLATION) {

		puts("caught AV as expected.");

		return EXCEPTION_CONTINUE_EXECUTION;

	}

	else {

		puts("didn't catch AV, unexpected.");

		return EXCEPTION_CONTINUE_EXECUTION;

	};
}
BOOL get_sym_name(HANDLE _handle,void *_pa, SYMBOL_INFO &_symbol) {
	__try {
		if (::SymFromAddr(_handle, reinterpret_cast<DWORD64>(_pa), 0, &_symbol)) {
			return TRUE;
		}
		return FALSE;
	}
	__except (filter(GetExceptionCode(), GetExceptionInformation())) {
		return FALSE;
	}
	return FALSE;
}
std::string Interceptor_Internal::get_function_name_from_symbols_library(void *_pa) {
	
	std::string func_name = "";
	BOOL bResult = FALSE;
	const size_t max_name_length = MAX_SYM_NAME;
	SYMBOL_INFO dummy;
	char buffer2[sizeof(SYMBOL_INFO) + max_name_length * sizeof(dummy.Name[0])] = { 0 };
	SYMBOL_INFO &symbol = *reinterpret_cast<SYMBOL_INFO *>(buffer2);

	symbol.SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol.MaxNameLen = max_name_length;
	bResult = get_sym_name(m_current_process, _pa, symbol);

	if (bResult) {
		auto p = symbol.Name;
		while ((*p < 32) || (*p > 127))  // skip any strange characters at the beginning of the symbol name
		{
			p++;
		}
		func_name = std::string(p);
	}
	return func_name;
}

std::string Interceptor_Internal::get_function_name_internal(void *_pa) {
	std::string fn = "";
	SLOCK(m_called_func_mutex)
	auto iter = m_function_name_cache.find(_pa);
	if (iter == m_function_name_cache.end()) {
		fn = get_function_name_from_symbols_library(_pa);
		if (m_configuration.p_function_names == FunctionNames::NORMALIZED) {
			fn = Utils::get_normalized_function_name(fn);
		}
		m_function_name_cache[_pa] = fn;
	}
	else {
		fn = iter->second;
	}
	return fn;
}