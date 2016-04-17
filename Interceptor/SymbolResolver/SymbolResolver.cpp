#include "SymbolResolver.h"
#include "Logger.h"
#include <imagehlp.h>
#include <strsafe.h>

using namespace Interceptor;
using namespace AceLogger;

SymbolResolver::SymbolResolver() {
	Log("Symbol resolver created.");
	m_bInitResult = false;
	m_current_process = 0;

}

//Function to load the symbols of the module
void SymbolResolver::init(void *_pAddress) {
	if (m_bInitResult)
		return;
	Log("initializing Symbol resolver...");
	//Query the memory
	char moduleName[MAX_PATH];
	MEMORY_BASIC_INFORMATION mbi;
	m_current_process = GetCurrentProcess();
	//Get the module name where the address is available
	VirtualQuery((void*)_pAddress, &mbi, sizeof(mbi));
	GetModuleFileNameA((HMODULE)mbi.AllocationBase,
		moduleName, MAX_PATH);
	Log("Module name : " + std::string(moduleName));
	//Initialize the symbols
	if (SymInitialize(m_current_process, moduleName, TRUE) == TRUE) {
		m_bInitResult = true;
	}

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

bool get_file_name(HANDLE _handle, void *_pa, std::string &_file_name) {
	static bool bHasFailed = false;  // for debugging purposes (so we only output the first time symbol lookup fails)
	int LineNumber = 0;
	if (reinterpret_cast<DWORD64>(_pa) == 0) {
		return false;
	}
	const size_t max_name_length = MAX_SYM_NAME;
#pragma warning(disable:4101)
	PIMAGEHLP_SYMBOL64 dummy;
#pragma warning(default:4101)
	char buffer2[sizeof(SYMBOL_INFO) + max_name_length * sizeof(dummy->Name[0])] = { 0 };
	PIMAGEHLP_SYMBOL64 pSymbol;
	pSymbol = (PIMAGEHLP_SYMBOL64)buffer2;
	pSymbol->SizeOfStruct = sizeof(buffer2);
	pSymbol->MaxNameLength = MAX_SYM_NAME;

	DWORD SymbolDisplacement = 0;

	IMAGEHLP_LINE64 Line;
	memset(&Line, 0, sizeof(Line));

	Line.SizeOfStruct = sizeof(Line);

	if (SymGetLineFromAddr64(_handle, reinterpret_cast<DWORD64>(_pa), &SymbolDisplacement, &Line)) {
		LineNumber = Line.LineNumber;
		_file_name = std::string(Line.FileName);
		return true;
	}
	return false;
}

bool get_sym_name_eha(HANDLE _handle, void *_pa, SYMBOL_INFO &_symbol) {
	__try {
		if (::SymFromAddr(_handle, reinterpret_cast<DWORD64>(_pa), 0, &_symbol)) {
			return true;
		}
		return false;
	}
	__except (filter(GetExceptionCode(), GetExceptionInformation())) {
		return false;
	}
	return false;
}

bool get_sym_name(HANDLE _handle, void *_pa, std::string &_func_name) {
	const size_t max_name_length = MAX_SYM_NAME;
#pragma warning(disable:4101)
	SYMBOL_INFO dummy;
#pragma warning(default:4101)
	char buffer2[sizeof(SYMBOL_INFO) + max_name_length * sizeof(dummy.Name[0])] = { 0 };
	SYMBOL_INFO &symbol = *reinterpret_cast<SYMBOL_INFO *>(buffer2);

	symbol.SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol.MaxNameLen = max_name_length;
	if (get_sym_name_eha(_handle, _pa, symbol)) {
		auto p = symbol.Name;
		while ((*p < 32) || (*p > 127))  // skip any strange characters at the beginning of the symbol name
		{
			p++;
		}
		_func_name = std::string(p);
		return true;
	}
	return false;
}
std::vector<Module_native> modules;
std::vector<Symbol_native> symbols_data;
BOOL CALLBACK EnumSymProc(
	PSYMBOL_INFO pSymInfo,
	ULONG SymbolSize,
	PVOID UserContext) {
	UNREFERENCED_PARAMETER(UserContext);
	auto symbol_name = std::string(pSymInfo->Name);
	
	symbols_data.emplace_back(Symbol_native(symbol_name,pSymInfo->Address,pSymInfo->Size ));

	return TRUE;
}

BOOL CALLBACK EnumModules(
	PCSTR  ModuleName,
	DWORD64 BaseOfDll,
	PVOID   UserContext) {
	UNREFERENCED_PARAMETER(UserContext);
	modules.emplace_back(Module_native(std::string(ModuleName), BaseOfDll));
	return TRUE;
}

BOOL CALLBACK EnumModules(
	PCTSTR  ModuleName,
	DWORD64 BaseOfDll,
	PVOID   UserContext) {
	UNREFERENCED_PARAMETER(UserContext);
	modules.emplace_back(Module_native(std::string((LPCSTR)ModuleName), BaseOfDll));
	return TRUE;
}

void SymbolResolver::load_modules() {
	modules.clear();
	Log("Loading all module names...");
	if (SymEnumerateModules64(m_current_process, EnumModules, NULL)) {
		// SymEnumerateModules64 returned success
	}
	else {
		// SymEnumerateModules64 failed
		auto error = GetLastError();
	}
}

void SymbolResolver::load_all_symbols(std::vector<Symbol_native> &_symbols,
	std::vector<Module_native> &_modules) {
	load_modules();
	_modules.swap(modules);
	char *Mask = "*";
	symbols_data.clear();
	Log("Loading all symbol names...");
	for (auto &m : _modules) {
		auto BaseOfDll = m.p_base_address;
		if (SymEnumSymbols(m_current_process,     // Process handle from SymInitialize.
			BaseOfDll,   // Base address of module.
			Mask,        // Name of symbols to match.
			EnumSymProc, // Symbol handler procedure.
			NULL))       // User context.
		{
			// SymEnumSymbols succeeded
		}
	}
	_symbols.swap(symbols_data);
}

std::string SymbolResolver::get_function_file_from_symbols_library(void *_pa) {
	std::string file_name = "";
	bool bResult = get_file_name(m_current_process, _pa, file_name);
	return file_name;
}

std::string SymbolResolver::get_function_name_from_symbols_library(void *_pa) {

	std::string func_name = "";
	bool bResult = get_sym_name(m_current_process, _pa, func_name);
	return func_name;
}