#pragma once
// Unicode ,multi byte , 32-bit, 64 bit configuration
#include <windows.h>
#include <imagehlp.h>
#include <strsafe.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#ifdef  UNICODE
	typedef wchar_t char_type;
	#define SymInitializeEncoded SymInitializeW
	#ifdef _WIN64
		#define UnDecorateSymbolNameEncoded UnDecorateSymbolName
		typedef IMAGEHLP_SYMBOL64 SYMBOL_INFO_ENCODED;
		#define SymFromAddrEncoded SymGetSymFromAddr
	#else
		#define USING_WSTRING
		#define UnDecorateSymbolNameEncoded UnDecorateSymbolNameW
		typedef SYMBOL_INFOW SYMBOL_INFO_ENCODED;
		#define SymFromAddrEncoded SymFromAddrW
		
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
	
	#define UnDecorateSymbolNameEncoded UnDecorateSymbolName

#endif

#ifdef USING_WSTRING
#define STD_STRING std::wstring
#define INIT_STR L""
#define CONSOLE_OUT std::wcout
#define STRSTREAM std::wstringstream
#define STSTR wcsstr
#define STRLEN wcslen
#define STRCOPY_S wcscpy_s
#define APPENDSTR(STR) STR.append(L"-");
#define FILE_OSTREAM std::wofstream
#define STRING_NFOUND std::wstring::npos
#else
#define STD_STRING std::string
#define INIT_STR ""
#define CONSOLE_OUT std::cout
#define STRSTREAM std::stringstream
#define STSTR strstr
#define STRLEN strlen
#define STRCOPY_S strcpy_s
#define APPENDSTR(STR) STR.append("-");
#define FILE_OSTREAM std::ofstream
#define STRING_NFOUND std::string::npos
#endif
