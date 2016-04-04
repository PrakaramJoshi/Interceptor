#include "stdafx.h"
#include "Interceptor_Internal.h"
using namespace Interceptor;

Interceptor_Internal::Interceptor_Internal() {
	m_bInitResult = FALSE;
	m_current_process = 0;
	m_mutex_available = true;
	m_interceptor_mode = InterceptorMode::CALL_DIAGRAM;
#ifdef UNICODE
#ifdef _WIN64
	m_DisabledStuffStr.push_back("std::");
	m_DisabledStuffStr.push_back("Concurrency::");
	m_DisabledStuffStr.push_back("<lambda_");
	m_DisabledStuffStr.push_back("operator new");
#else
	m_DisabledStuffStr.push_back(L"std::");
	m_DisabledStuffStr.push_back(L"Concurrency::");
	m_DisabledStuffStr.push_back(L"<lambda_");
	m_DisabledStuffStr.push_back(L"operator new");
#endif
#else
	m_DisabledStuffStr.push_back("std::");
	m_DisabledStuffStr.push_back("Concurrency::");
	m_DisabledStuffStr.push_back("<lambda_");
	m_DisabledStuffStr.push_back("operator new");
#endif
}

Interceptor_Internal::~Interceptor_Internal() {
	if (!m_mutex_available)
		return;
	m_called_func_mutex.lock();
	switch (m_interceptor_mode) {
		case Interceptor::InterceptorMode::IMMEDIATE_PRINT:
			break;
		case Interceptor::InterceptorMode::CALL_DIAGRAM:
			m_call_graph_recorder.create_call_chart();
			break;
		default:
			break;
	}
	m_mutex_available = false;
	m_called_func_mutex.unlock();
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

void Interceptor_Internal::on_enter_immediate_print_mode(const STD_STRING &_fn_name) {
	auto current_depth = (++m_function_call_depth);
	print_to_console(current_depth, _fn_name, true);
}

void Interceptor_Internal::on_exit_immediate_print_mode(const STD_STRING &_fn_name) {
	auto current_depth = m_function_call_depth.load();
	--m_function_call_depth;
	print_to_console(current_depth, _fn_name, false);
}

void Interceptor_Internal::on_enter_call_diagram_mode(const STD_STRING &_fn_name) {
	m_call_graph_recorder.record(_fn_name);
}

void Interceptor_Internal::on_exit_call_diagram_mode(const STD_STRING &_fn_name) {
	m_call_graph_recorder.record(_fn_name,CALL_STATUS::CALL_OUT);
}

void Interceptor_Internal::on_enter_internal(void *_pa) {
	if (!m_mutex_available)
		return;
	m_called_func_mutex.lock();
	auto iter = m_function_name_cache.find(_pa);
	STD_STRING fn;
	if (iter == m_function_name_cache.end()) {
		fn = get_function_name_internal(_pa);
		m_function_name_cache[_pa] = fn;
	}
	else {
		fn = iter->second;
	}
	switch (m_interceptor_mode) {
		case Interceptor::InterceptorMode::IMMEDIATE_PRINT:
			on_enter_immediate_print_mode(fn);
			break;
		case Interceptor::InterceptorMode::CALL_DIAGRAM:
			on_enter_call_diagram_mode(fn);
			break;
		default:
			break;
	}
	m_called_func_mutex.unlock();
}

void Interceptor_Internal::on_exit_internal(void *_pa) {
	if (!m_mutex_available)
		return;
	m_called_func_mutex.lock();
	
	STD_STRING fn;
	auto iter = m_function_name_cache.find(_pa);
	if (iter == m_function_name_cache.end()) {
		fn = get_function_name_internal(_pa);
		m_function_name_cache[_pa] = fn;
	}
	else {
		fn = iter->second;
	}
	switch (m_interceptor_mode) {
		case Interceptor::InterceptorMode::IMMEDIATE_PRINT:
			on_exit_immediate_print_mode(fn);
			break;
		case Interceptor::InterceptorMode::CALL_DIAGRAM:
			on_exit_call_diagram_mode(fn);
			break;
		default:
			break;
	}
	m_called_func_mutex.unlock();
	
}

void Interceptor_Internal::print_to_console(const std::size_t &_stack_depth,
											const STD_STRING &_function_name, 
											bool _in) {
	if (_function_name.empty())
		return;
	for (auto &_disbled_stuff : m_DisabledStuffStr) {
		if (_function_name.find(_disbled_stuff) != STRING_NFOUND) {
			return;
		}
	}
	STD_STRING out_str = INIT_STR;
	for (auto i = _stack_depth; i > 0; i--) {
		APPENDSTR(out_str);
	}
	out_str.append(_function_name);

	if (_in) {
		CONSOLE_OUT << "(in  ";
	}
	else {
		CONSOLE_OUT << "(out ";
	}
	CONSOLE_OUT << _stack_depth << 
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
	char_type moduleName[MAX_PATH];
	TCHAR modShortNameBuf[MAX_PATH];
	MEMORY_BASIC_INFORMATION mbi;
	m_current_process = GetCurrentProcess();
	//Get the module name where the address is available
	VirtualQuery((void*)_pAddress, &mbi, sizeof(mbi));
	GetModuleFileName((HMODULE)mbi.AllocationBase,
		moduleName, MAX_PATH);

	//Initialize the symbols
	m_bInitResult = SymInitializeEncoded(m_current_process, moduleName, TRUE);

	//Set the options
	SymSetOptions(SymGetOptions()   &~SYMOPT_UNDNAME);
}

STD_STRING Interceptor_Internal::get_function_name_internal(void *_pa) {
	DWORD64 symDisplacement = 0;
	STD_STRING func_name = INIT_STR;
	//Allocate the memory for PSYMBOL_INFO
	//Get the name of the symbol using the address
	enum { MAX_SYMBOL_BUF_NAME_LENGTH = MAX_SYM_NAME };
	enum {
		SIZEOF_SEGMENT = sizeof(IMAGEHLP_SYMBOL64) +
		MAX_SYMBOL_BUF_NAME_LENGTH * sizeof(TCHAR)
	};


	TCHAR  buffer[SIZEOF_SEGMENT];
	memset(&buffer, 0, sizeof(buffer));

	SYMBOL_INFO_ENCODED *pSymbolInfo = (SYMBOL_INFO_ENCODED *)buffer;
	pSymbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO_ENCODED);
#ifdef _WIN64
	pSymbolInfo->MaxNameLength = MAX_SYMBOL_BUF_NAME_LENGTH;
#else
	pSymbolInfo->MaxNameLen = MAX_SYMBOL_BUF_NAME_LENGTH;
#endif

	BOOL bResult = FALSE;
	bResult = SymFromAddrEncoded(m_current_process, (DWORD64)_pa, &symDisplacement, pSymbolInfo);
	if (bResult) {
		auto p = pSymbolInfo->Name;
		while ((*p < 32) || (*p > 127))  // skip any strange characters at the beginning of the symbol name
		{
			p++;
		}
		func_name = STD_STRING(p);
	}
	else {

	}
	return func_name;
}