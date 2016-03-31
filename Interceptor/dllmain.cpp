// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "interceptor_internal.h"
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			//Loads the symbols of the module
			Interceptor::Interceptor_Internal::init(hModule);
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

