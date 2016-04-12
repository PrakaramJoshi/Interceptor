#pragma once
#include "CallGraphRecorder.h"
#include "CallStackLazyRecord.h"
#include "FunctionDepth.h"
#include "InterceptorConfig.h"
#include "SymbolResolver.h"

#include <map>
#include <mutex>
#include <vector>
#include <atomic>

namespace Interceptor {
	
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

		std::string get_function_name_internal(void *_pa);

		std::string get_function_file_internal(void *_pa);

		void on_enter_immediate_print_mode(void *_pa);

		void on_exit_immediate_print_mode(void *_pa);

		void on_enter_call_diagram_mode(void *_pa);

		void on_exit_call_diagram_mode(void *_pa);

		void on_enter_internal(void *_pa);

		void on_exit_internal(void *_pa);

		void print_to_console(const std::size_t &_stack_depth, const std::string &_function_name, bool _in);

		static Interceptor_Internal& get();

	public:

		friend class CallGraphRecorder;

		~Interceptor_Internal();

		static void init(void *_pAddress);

		static void on_enter(void *_pa);

		static void on_exit(void *_pa);

	};
}
