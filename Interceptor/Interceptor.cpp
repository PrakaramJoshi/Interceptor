// Interceptor.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Interceptor.h"
#include "interceptor_internal.h"
#include "Hook.h"

extern "C" void _penter();
extern "C" void _pexit();

void on_enter(void* pa) {
	Interceptor::Interceptor_Internal::on_enter(pa);
}

void on_exit(void* pa) {

	Interceptor::Interceptor_Internal::on_exit(pa);
}

