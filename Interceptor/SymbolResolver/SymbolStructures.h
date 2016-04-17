#pragma once
#include "StringIndexer.h"
#include <Windows.h>
#include <set>
namespace Interceptor {

	struct Symbol {

		std::size_t p_id;

		string_id p_name;

		ULONG64 p_address;

		ULONG64 p_address_upper;

		bool operator<(const Symbol &_sym)const {
			return p_address < _sym.p_address;
		}
	};

	struct Module {

		std::size_t p_id;

		string_id p_module_name;

		DWORD64 p_base_address;

		bool operator<(const Module &_sym)const {
			return p_base_address < _sym.p_base_address;
		}
	};

	struct Module_native {
		std::string p_name;

		DWORD64 p_base_address;

		Module_native(const std::string &_name,
			const DWORD64 _address) {
			p_name = _name;
			p_base_address = _address;
		}
	};

	struct Symbol_native {
		std::string p_name;

		ULONG64 p_address;

		ULONG p_size;

		Symbol_native(const std::string &_name,
			const ULONG64 _address,
			const ULONG _size) {
			p_name = _name;
			p_address = _address;
			p_size = _size;
		}
	};
}
