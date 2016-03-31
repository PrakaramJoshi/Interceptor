#pragma once
// Unicode ,multi byte , 32-bit, 64 bit configuration
#include <windows.h>
#include <imagehlp.h>
#include <strsafe.h>
#include <string>
#include <iostream>
#ifdef  UNICODE
	typedef wchar_t char_type;
	#define SymInitializeEncoded SymInitializeW
	#ifdef _WIN64
		#define STSTR strstr
		#define STRLEN strlen
		#define STRCOPY_S strcpy_s
		#define APPENDSTR(STR) STR.append("-");
		#define UnDecorateSymbolNameEncoded UnDecorateSymbolName
		typedef IMAGEHLP_SYMBOL64 SYMBOL_INFO_ENCODED;
		#define SymFromAddrEncoded SymGetSymFromAddr
		#define STD_STRING std::string
		#define INIT_STR ""
		#define CONSOLE_OUT std::cout
	#else
		#define STSTR wcsstr
		#define STRLEN wcslen
		#define STRCOPY_S wcscpy_s
		#define APPENDSTR(STR) STR.append(L"-");
		#define UnDecorateSymbolNameEncoded UnDecorateSymbolNameW
		typedef SYMBOL_INFOW SYMBOL_INFO_ENCODED;
		#define SymFromAddrEncoded SymFromAddrW
		#define STD_STRING std::wstring
		#define INIT_STR L""
		#define CONSOLE_OUT std::wcout
	#endif
#else
	typedef char char_type;
	#define SymInitializeEncoded SymInitialize
	#ifdef _WIN64
		typedef IMAGEHLP_SYMBOL64 SYMBOL_INFO_ENCODED;
		#define SymFromAddrEncoded SymGetSymFromAddr
	#else
		typedef SYMBOL_INFO SYMBOL_INFO_ENCODED;
		#define SymFromAddrEncoded SymFromAddr
	#endif
	#define STSTR strstr
	#define STRLEN strlen
	#define STRCOPY_S strcpy_s
	#define APPENDSTR(STR) STR.append("-");
	#define UnDecorateSymbolNameEncoded UnDecorateSymbolName
	#define STD_STRING std::string
	#define INIT_STR ""
	#define CONSOLE_OUT std::cout
#endif
