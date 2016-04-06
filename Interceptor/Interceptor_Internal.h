#pragma once
#include "CallGraphRecorder.h"
#include "FunctionDepth.h"
#include "InterceptorConfig.h"

#include <map>
#include <mutex>
#include <vector>
#include <atomic>
#include <Windows.h>
namespace Interceptor {
	
	class Interceptor_Internal {

		Interceptor_Internal(const Interceptor_Internal&)			= delete;
		Interceptor_Internal(Interceptor_Internal &&)				= delete;
		Interceptor_Internal& operator=(const Interceptor_Internal&)= delete;
		Interceptor_Internal& operator=(Interceptor_Internal&&)		= delete;
		Interceptor_Internal();

		// Flag to indicate the result of symbol initialization
		BOOL	m_bInitResult;

		HANDLE m_current_process;

		FuntionDepth m_function_call_depth;

		CallGraphRecorder m_call_graph_recorder;

		std::mutex m_called_func_mutex;

		std::mutex m_print_mutex;

		bool m_main_found;

		std::map<void*, std::string> m_function_name_cache;

		std::map<void*, std::string> m_function_file_cache;

		std::atomic<bool> m_mutex_available;

		const InterceptorConfiguration m_configuration;

		void init_internal(void *_pAddress);

		std::string get_function_name_from_symbols_library(void *_pa);

		std::string get_function_name_internal(void *_pa);

		std::string get_function_file_from_symbols_library(void *_pa);

		std::string get_function_file_internal(void *_pa);

		void on_enter_immediate_print_mode(void *_pa);

		void on_exit_immediate_print_mode(void *_pa);

		void on_enter_call_diagram_mode(void *_pa);

		void on_exit_call_diagram_mode(void *_pa);

		void on_enter_internal(void *_pa);

		void on_exit_internal(void *_pa);

		bool is_main(const std::string &_func);

		void print_to_console(const std::size_t &_stack_depth, const std::string &_function_name, bool _in);

		static Interceptor_Internal& get();

	public:

		~Interceptor_Internal();

		static void init(void *_pAddress);

		static void on_enter(void *_pa);

		static void on_exit(void *_pa);

	};
}
