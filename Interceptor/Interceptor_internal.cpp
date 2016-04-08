#include "stdafx.h"
#include "Interceptor_Internal.h"
#include "CallStackUtils.h"

#include <iostream>
#include <string>


using namespace Interceptor;

Interceptor_Internal::Interceptor_Internal() {
	
	m_mutex_available = true;	
}

Interceptor_Internal::~Interceptor_Internal() {
	if (!m_mutex_available)
		return;
	SLOCK(m_print_mutex)

	switch (m_configuration.p_mode) {
		case InterceptorMode::IMMEDIATE_PRINT:
			break;
		case InterceptorMode::CALL_DIAGRAM_FILES:
		case InterceptorMode::CALL_DIAGRAM_FUNCTION:
			m_call_graph_recorder.create_call_chart(m_configuration.p_mode);
		default:
			break;
	}
	
	m_mutex_available = false;
	SLOCK(m_called_func_mutex)
	
}

Interceptor_Internal& Interceptor_Internal::get() {
	static Interceptor_Internal instance;
	return instance;
}

void Interceptor_Internal::init(void *_pAddress) {
	Interceptor_Internal::get().m_symbol_resolver.init(_pAddress);
}

void Interceptor_Internal::on_enter(void *_pa) {
	Interceptor_Internal::get().on_enter_internal(_pa);
}

void Interceptor_Internal::on_exit(void *_pa) {
	Interceptor_Internal::get().on_exit_internal(_pa);
}

void Interceptor_Internal::on_enter_immediate_print_mode(void *_pa) {
	auto fn_name = get_function_name_internal(_pa);
	auto current_depth = (++m_function_call_depth);
	print_to_console(current_depth, fn_name, true);
}

void Interceptor_Internal::on_exit_immediate_print_mode(void *_pa) {
	auto fn_name = get_function_name_internal(_pa);
	auto current_depth = m_function_call_depth.load();
	--m_function_call_depth;
	print_to_console(current_depth, fn_name, false);
}

void Interceptor_Internal::on_enter_call_diagram_mode(void *_pa) {
	
	if (m_configuration.p_record_mode == RecordMode::LAZY) {
		m_call_graph_recorder.record_lazy(_pa, CALL_STATUS::CALL_IN);
	}
	else if (m_configuration.p_record_mode == RecordMode::REALTIME) {
		auto fn_name = get_function_name_internal(_pa);
		auto fn_file_name = get_function_file_internal(_pa);
		m_call_graph_recorder.record(fn_name, fn_file_name, CALL_STATUS::CALL_IN);
	}
}

void Interceptor_Internal::on_exit_call_diagram_mode(void *_pa) {

	if (m_configuration.p_record_mode == RecordMode::LAZY) {
		m_call_graph_recorder.record_lazy(_pa, CALL_STATUS::CALL_OUT);
	}
	else if (m_configuration.p_record_mode == RecordMode::REALTIME) {
		auto fn_name = get_function_name_internal(_pa);
		auto fn_file_name = get_function_file_internal(_pa);
		m_call_graph_recorder.record(fn_name, fn_file_name, CALL_STATUS::CALL_OUT);
	}

}

void Interceptor_Internal::on_enter_internal(void *_pa) {
	if (!m_mutex_available)
		return;
	switch (m_configuration.p_mode) {
		case Interceptor::InterceptorMode::IMMEDIATE_PRINT:
			on_enter_immediate_print_mode(_pa);
			break;

		case Interceptor::InterceptorMode::CALL_DIAGRAM_FUNCTION:
		case Interceptor::InterceptorMode::CALL_DIAGRAM_FILES:
			on_enter_call_diagram_mode(_pa);
			break;
		default:
			break;
	}
}

void Interceptor_Internal::on_exit_internal(void *_pa) {
	if (!m_mutex_available)
		return;
	switch (m_configuration.p_mode) {
		case Interceptor::InterceptorMode::IMMEDIATE_PRINT:
			on_exit_immediate_print_mode(_pa);
			break;

		case Interceptor::InterceptorMode::CALL_DIAGRAM_FUNCTION:
		case Interceptor::InterceptorMode::CALL_DIAGRAM_FILES:
			on_exit_call_diagram_mode(_pa);
			break;
		default:
			break;
	}	
}

void Interceptor_Internal::print_to_console(const std::size_t &_stack_depth,
											const std::string &_function_name, 
											bool _in) {
	if (_function_name.empty())
		return;
	for (auto &_disbled_stuff : m_configuration.p_disabled_stuff) {
		if (_function_name.find(_disbled_stuff) != std::string::npos) {
			return;
		}
	}
	std::string out_str = "";
	for (auto i = _stack_depth; i > 0; i--) {
		out_str.append("-");
	}
	out_str.append(_function_name);

	SLOCK(m_print_mutex)
	if (_in) {
		std::cout << "(in  ";
	}
	else {
		std::cout << "(out ";
	}
	std::cout << _stack_depth << 
				", Thread: " << 
				std::this_thread::get_id() << 
				")" << 
				out_str << std::endl;
}


std::string Interceptor_Internal::get_function_name_internal(void *_pa) {
	std::string fn = "";
	SLOCK(m_called_func_mutex)
	auto iter = m_function_name_cache.find(_pa);
	if (iter == m_function_name_cache.end()) {
		fn = m_symbol_resolver.get_function_name_from_symbols_library(_pa);
		if (m_configuration.p_function_names == FunctionNames::NORMALIZED) {
			fn = Utils::get_normalized_function_name(fn);
		}
		m_function_name_cache[_pa] = fn;
	}
	else {
		fn = iter->second;
	}
	return fn;
}


std::string Interceptor_Internal::get_function_file_internal(void *_pa) {
	std::string fn = "";
	SLOCK(m_called_func_mutex)
	auto iter = m_function_file_cache.find(_pa);
	if (iter == m_function_file_cache.end()) {
		fn =m_symbol_resolver.get_function_file_from_symbols_library(_pa);
		if (m_configuration.p_function_names == FunctionNames::NORMALIZED) {
			fn = Utils::get_file_name_from_path(fn);
		}
		m_function_file_cache[_pa] = fn;
	}
	else {
		fn = iter->second;
	}
	return fn;
}