#pragma once
#include "CallGraphRecorder.h"
#include "CallStackLazyRecord.h"
#include "FunctionDepth.h"
#include "InterceptorConfig.h"
#include "Logger.h"
#include "SymbolResolver.h"

#include <map>
#include <mutex>
#include <vector>
#include <atomic>

namespace Interceptor {

	static HANDLE& DLLModuleHandle() {
		static HANDLE handle;
		return handle;
	};
	
	class Interceptor_Internal {

		Interceptor_Internal(const Interceptor_Internal&)			= delete;
		Interceptor_Internal(Interceptor_Internal &&)				= delete;
		Interceptor_Internal& operator=(const Interceptor_Internal&)= delete;
		Interceptor_Internal& operator=(Interceptor_Internal&&)		= delete;
		Interceptor_Internal();

		FuntionDepth					m_function_call_depth;

		CallGraphRecorder				m_call_graph_recorder;

		SymbolResolver					m_symbol_resolver;

		NonRecursiveLock				m_called_func_mutex;

		NonRecursiveLock				m_print_mutex;

		std::map<void*, std::string>	m_function_name_cache;

		std::map<void*, std::string>	m_function_file_cache;

		InterceptorConfiguration		m_configuration;

		AceLogger::register_logger_helper *m_logger;

		std::string get_most_relevant_module_name(const std::string &_fn_name);

		std::string get_function_name_internal(void *_pa);

		std::string get_function_file_internal(void *_pa);

		void on_enter_immediate_print_mode(void *_pa);

		void on_exit_immediate_print_mode(void *_pa);

		void on_enter_call_diagram_mode(void *_pa);

		void on_exit_call_diagram_mode(void *_pa);

		void on_enter_internal(void *_pa);

		void on_exit_internal(void *_pa);

		void print_to_console(const std::size_t &_stack_depth, const std::string &_function_name, bool _in);

		SymbolResolver& symbol_resolver() { return m_symbol_resolver; };

		InterceptorConfiguration& interceptor_configuration() { return m_configuration; };

		static Interceptor_Internal& get();

		void init();

	public:

		friend class CallGraphRecorder;

		~Interceptor_Internal();

		static void on_enter(void *_pa);

		static void on_exit(void *_pa);

	};
}
