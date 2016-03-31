#pragma once

#ifdef INTERCEPTOR_EXPORTS
#define INTERCCEPTRO_EXPORTED_API __declspec( dllexport )
#else
#define INTERCCEPTRO_EXPORTED_API __declspec( dllimport )
#endif

#ifdef _WIN64
extern"C" void INTERCCEPTRO_EXPORTED_API on_enter(void* pa);
extern"C" void INTERCCEPTRO_EXPORTED_API on_exit(void* pa);
#else
void on_enter(void*);
void on_exit(void*);
extern "C" void INTERCCEPTRO_EXPORTED_API _penter();
extern "C" void INTERCCEPTRO_EXPORTED_API _pexit();
#endif
