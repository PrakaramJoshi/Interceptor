#pragma once
#include "InterceptorConfig.h"
#include "FunctionDepth.h"
#include "CallGraphRecorder.h"
#include <map>
#include <mutex>
#include <vector>
#include <atomic>
namespace Interceptor {
	enum class InterceptorMode {IMMEDIATE_PRINT,CALL_DIAGRAM};

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

		std::map<void*, STD_STRING> m_function_name_cache;

		std::vector<STD_STRING> m_DisabledStuffStr;

		std::atomic<bool> m_mutex_available;

		InterceptorMode m_interceptor_mode;

		void init_internal(void *_pAddress);

		STD_STRING get_function_name_internal(void *_pa);

		void on_enter_immediate_print_mode(const STD_STRING &_fn_name);

		void on_exit_immediate_print_mode(const STD_STRING &_fn_name);

		void on_enter_call_diagram_mode(const STD_STRING &_fn_name);

		void on_exit_call_diagram_mode(const STD_STRING &_fn_name);

		void on_enter_internal(void *_pa);

		void on_exit_internal(void *_pa);

		void print_to_console(const std::size_t &_stack_depth, const STD_STRING &_function_name, bool _in);

		static Interceptor_Internal& get();

	public:

		~Interceptor_Internal();

		static void init(void *_pAddress);

		static void on_enter(void *_pa);

		static void on_exit(void *_pa);

	};
}
