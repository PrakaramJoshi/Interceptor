#include "stdafx.h"
#include "Interceptor_Internal.h"
#include "ConfigurationLoader.h"
#include "InterceptorUtils.h"
#include <iostream>
#include <string>


using namespace Interceptor;
using namespace AceLogger;
Interceptor_Internal::Interceptor_Internal() {

	m_logger = new register_logger_helper(Utils::get_current_directory(), "Interceptor", "1.0");
	init();
	ConfigurationLoader configloader;
	m_configuration = configloader.get_configuration();
	if (m_configuration.p_mode == InterceptorMode::FORCE_DIAGRAM || 
		m_configuration.p_mode == InterceptorMode::DEPENDENCY_WHEEL) {
		if (!(m_configuration.p_record_mode == RecordMode::LAZY || m_configuration.p_record_mode == RecordMode::REALTIME ||
			m_configuration.p_record_mode == RecordMode::PRELOAD_FUNCTIONS)) {
			Log("Interceptor mode set to call diagram, but no recording specified, using default mode as Lazy");
			m_configuration.p_record_mode = RecordMode::LAZY;
		}
	}
	if (!(m_configuration.p_record_type == RecordType::FILE || m_configuration.p_record_type == RecordType::FUNCTION)) {
		Log("No record type specified, using FUNCTION as default record type");
		m_configuration.p_record_type = RecordType::FUNCTION;
	}
	m_call_graph_recorder.set_record_type(m_configuration.p_record_type);
	if (m_configuration.p_record_mode == RecordMode::PRELOAD_FUNCTIONS) {
		m_symboldb.cache_all_data(m_configuration);
	}
	for (auto& s : m_configuration.p_suppress_file_names) {
		m_suppressed_ids.insert(m_symboldb.get_id(s));
	}
	for (auto& s : m_configuration.p_suppress_function_names) {
		m_suppressed_ids.insert(m_symboldb.get_id(s));
	}
	m_call_graph_recorder.suppress_ids(m_suppressed_ids);
}

Interceptor_Internal::~Interceptor_Internal() {

	Log("Cleaning up Interceptor...");
	UniqueGuard guard_print(m_print_mutex);

	switch (m_configuration.p_mode) {
		case InterceptorMode::IMMEDIATE_PRINT:
			break;
		case InterceptorMode::FORCE_DIAGRAM:
		case InterceptorMode::DEPENDENCY_WHEEL:
			m_call_graph_recorder.create_call_chart(m_configuration.p_mode);
		default:
			break;
	}
	delete m_logger;
}

Interceptor_Internal& Interceptor_Internal::get() {
	static Interceptor_Internal instance;
	return instance;
}


void Interceptor_Internal::init() {
	m_symboldb.init(DLLModuleHandle());
}

void Interceptor_Internal::on_enter(void *_pa) {
	Interceptor_Internal::get().on_enter_internal(_pa);
}

void Interceptor_Internal::on_exit(void *_pa) {

	Interceptor_Internal::get().on_exit_internal(_pa);
}

void Interceptor_Internal::on_enter_immediate_print_mode(void *_pa) {
	string_id symbol_id;
	if(m_configuration.p_record_type==RecordType::FUNCTION)
		symbol_id = get_function_name_internal(_pa);
	else {
		symbol_id = get_function_file_internal(_pa);
	}
	auto current_depth = (++m_function_call_depth);
	print_to_console(current_depth, symbol_id, true);
}

void Interceptor_Internal::on_exit_immediate_print_mode(void *_pa) {
	string_id symbol_id;
	if (m_configuration.p_record_type == RecordType::FUNCTION)
		symbol_id = get_function_name_internal(_pa);
	else {
		symbol_id = get_function_file_internal(_pa);
	}
	auto current_depth = m_function_call_depth.load();
	--m_function_call_depth;
	print_to_console(current_depth, symbol_id, false);
}

void Interceptor_Internal::on_enter_call_diagram_mode(void *_pa) {
	
	if (m_configuration.p_record_mode == RecordMode::LAZY) {
		m_call_graph_recorder.record_lazy(_pa, CALL_STATUS::CALL_IN);
		return;
	}
	auto fn_id = get_function_name_internal(_pa);
	auto fn_file_id = get_function_file_internal(_pa);
	m_call_graph_recorder.record(fn_id, fn_file_id, CALL_STATUS::CALL_IN);
}

void Interceptor_Internal::on_exit_call_diagram_mode(void *_pa) {

	if (m_configuration.p_record_mode == RecordMode::LAZY) {
		m_call_graph_recorder.record_lazy(_pa, CALL_STATUS::CALL_OUT);
	}
	auto fn_id = get_function_name_internal(_pa);
	auto fn_file_id = get_function_file_internal(_pa);
	m_call_graph_recorder.record(fn_id, fn_file_id, CALL_STATUS::CALL_OUT);

}

void Interceptor_Internal::on_enter_internal(void *_pa) {

	switch (m_configuration.p_mode) {
		case Interceptor::InterceptorMode::IMMEDIATE_PRINT:
			on_enter_immediate_print_mode(_pa);
			break;

		case Interceptor::InterceptorMode::FORCE_DIAGRAM:
		case Interceptor::InterceptorMode::DEPENDENCY_WHEEL:
			on_enter_call_diagram_mode(_pa);
			break;
		default:
			break;
	}
}

void Interceptor_Internal::on_exit_internal(void *_pa) {

	switch (m_configuration.p_mode) {
		case Interceptor::InterceptorMode::IMMEDIATE_PRINT:
			on_exit_immediate_print_mode(_pa);
			break;

		case Interceptor::InterceptorMode::FORCE_DIAGRAM:
		case Interceptor::InterceptorMode::DEPENDENCY_WHEEL:
			on_exit_call_diagram_mode(_pa);
			break;
		default:
			break;
	}	
}

void Interceptor_Internal::print_to_console(const std::size_t &_stack_depth,
											const string_id &_symbol_id,
											bool _in) {
	if (m_suppressed_ids.find(_symbol_id) != m_suppressed_ids.end()) {
		return;
	}
	auto symbol_name = m_symboldb.get_string_from_id(_symbol_id);
	if (symbol_name.empty())
		return;
	std::string out_str = "";
	for (auto i = _stack_depth; i > 0; i--) {
		out_str.append("-");
	}
	out_str.append(symbol_name);

	UniqueGuard guard_print(m_print_mutex);
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


string_id Interceptor_Internal::get_function_name_internal(void *_pa) {
	if (m_configuration.p_record_type == RecordType::FUNCTION) {
		return m_symboldb.get_symbol_id(_pa, m_configuration);
	}
	std::string empty = "";
	return m_symboldb.get_id(empty);
}

string_id Interceptor_Internal::get_function_file_internal(void *_pa) {
	if (m_configuration.p_record_type == RecordType::FILE) {
		return m_symboldb.get_symbol_file_id(_pa, m_configuration);
	}
	std::string empty = "";
	return m_symboldb.get_id(empty);
}