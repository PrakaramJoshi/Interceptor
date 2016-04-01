#pragma once
#ifdef _WIN64
//extern "C" void _penter();
//extern "C" void _pexit();
#else
extern "C" void __declspec(naked) _cdecl _penter(void) {
	_asm
	{
		//Prolog instructions
		pushad
		//calculate the pointer to the return address by adding 4*8 bytes 
		//(8 register values are pushed onto stack which must be removed)
			mov  eax, esp
			add  eax, 32
			// retrieve return address from stack
			mov  eax, dword ptr[eax]
			// subtract 5 bytes as instruction for call _penter
			// is 5 bytes long on 32-bit machines, e.g. E8 <00 00 00 00>
			sub  eax, 5
			// provide return address to recordFunctionCall
			push eax
			call on_enter
			pop eax

			//Epilog instructions
			popad
			ret
	}
}
extern "C" void __declspec(naked) _cdecl _pexit(void) {
	_asm
	{
		//Prolog instructions
		pushad
		//calculate the pointer to the return address by adding 4*8 bytes 
		//(8 register values are pushed onto stack which must be removed)
			mov  eax, esp
			add  eax, 32
			// retrieve return address from stack
			mov  eax, dword ptr[eax]
			// subtract 5 bytes as instruction for call _penter
			// is 5 bytes long on 32-bit machines, e.g. E8 <00 00 00 00>
			sub  eax, 5
			// provide return address to recordFunctionCall
			push eax
			call on_exit
			pop eax

			//Epilog instructions
			popad
			ret
	}
}
#endif
