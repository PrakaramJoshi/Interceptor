#pragma once
#include "CallGraphRecorder.h"
#include "CallStackLazyRecord.h"
#include "CallGraphTimeline.h"
#include "FunctionDepth.h"
#include "InterceptorConfig.h"
#include "Logger.h"
#include "SymbolDB.h"

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

		FuntionDepth						m_function_call_depth;

		CallGraphRecorder					m_call_graph_recorder;

		NonRecursiveLock					m_print_mutex;

		InterceptorConfiguration			m_configuration;

		SymbolDB							m_symboldb;

		std::set<string_id>					m_suppressed_ids;

		AceLogger::register_logger_helper *	m_logger;

		string_id get_function_name_internal(void *_pa);

		string_id get_function_file_internal(void *_pa);

		void on_enter_immediate_print_mode(void *_pa);

		void on_exit_immediate_print_mode(void *_pa);

		void on_enter_call_diagram_mode(void *_pa);

		void on_exit_call_diagram_mode(void *_pa);

		void on_enter_internal(void *_pa);

		void on_exit_internal(void *_pa);

		void print_to_console(const std::size_t &_stack_depth, 
							const string_id &symbol_id, 
							bool _in);

		std::string get_string_from_id(const string_id &_id) {
			return m_symboldb.get_string_from_id(_id);
		}

		InterceptorConfiguration& interceptor_configuration() { return m_configuration; };

		static Interceptor_Internal& get();

		void init();

	public:

		friend class CallGraphRecorder;

		friend class TimelineNode;

		~Interceptor_Internal();

		static void on_enter(void *_pa);

		static void on_exit(void *_pa);

	};
}
