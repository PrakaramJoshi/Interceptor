#include "CallGraphRecorder.h"
#include "CallGraphHTML.h"
#include "InterceptorUtils.h"
#include "Interceptor_Internal.h"
#include "Logger.h"
#include <fstream>
#include<iostream>
#include <set>
#include <stack>
#include <string>
#include<sstream>
#include <Windows.h>
using namespace Interceptor;
using namespace AceLogger;

Interceptor::CallGraphRecorder::CallGraphRecorder() {
	m_mode = RecordType::NONE;
}

Interceptor::CallGraphRecorder::~CallGraphRecorder() {

	UniqueGuard guard_lock(m_lock);
	UniqueGuard guard_lazy_record_lock(m_lazy_record_lock);
}

void CallGraphRecorder::set_record_type(RecordType _mode) {
	m_mode = _mode;
}

void CallGraphRecorder::record(const std::string &_function_name,
	const std::string &_function_file_path,
	CALL_STATUS _call_status) {

	auto thread_id = std::this_thread::get_id();
	record(_function_name, _function_file_path, thread_id, _call_status);
}

void CallGraphRecorder::record(const std::string &_function_name,
	const std::string &_function_file_path,
	const std::thread::id &_thread_id,
	CALL_STATUS _call_status) {

	UniqueGuard guard_lock(m_lock);

	auto fn_id = m_string_indexer.record(_function_name);
	auto fn_file_id = m_string_indexer.record(_function_file_path);
	auto iter = m_call_stack_records.find(_thread_id);
	if (iter != m_call_stack_records.end()) {
		if (!(*iter).second.empty()) {
			CallStackRecord call_record(fn_id, fn_file_id, _call_status);
			auto & last_record = (*iter).second.back();
			if (last_record.first == call_record) {
				last_record.second = last_record.second + 1;
				return;
			}
		}
	}
	m_call_stack_records[_thread_id].emplace_back(std::make_pair(CallStackRecord(fn_id, fn_file_id, _call_status),1));
}

void CallGraphRecorder::record_now(void *_pa,
	CALL_STATUS _call_status) {
	auto thread_id = std::this_thread::get_id();
	std::string fn_name = "";
	std::string fn_file_name = "";
	if (m_mode == RecordType::FILE) {
		fn_file_name = Interceptor_Internal::get().get_function_file_internal(_pa);
	}
	else if (m_mode == RecordType::FUNCTION) {
		fn_name = Interceptor_Internal::get().get_function_name_internal(_pa);
	}
	record(fn_name, fn_file_name, thread_id, _call_status);
}

void CallGraphRecorder::record_lazy(void *_pa,
									CALL_STATUS _call_status ) {

	auto thread_id = std::this_thread::get_id();
	UniqueGuard guard_lazy_record_lock(m_lazy_record_lock);
	m_lazy_records[thread_id].push_back(CallStackLazyRecord(_pa, _call_status));
}


void CallGraphRecorder::populate_lazy_data() {
	Log("updating symbols for lazy data collection...");
	for (auto &thread : m_lazy_records) {
		for (auto &thread_data : thread.second) {
			auto pa = thread_data.get_pa();
			std::string fn_name = "";
			std::string fn_file_name = "";
			if (m_mode == RecordType::FILE) {
				fn_file_name  = Interceptor_Internal::get().get_function_file_internal(pa);
			}
			else if (m_mode == RecordType::FUNCTION) {
				fn_name = Interceptor_Internal::get().get_function_name_internal(pa);
			}
			record(fn_name, fn_file_name, thread.first, thread_data.get_call_status());
		}
	}
	Log("lazy symbols evaluated!");
}

void Interceptor::CallGraphRecorder::print() {

}

void get_call_chart(const std::vector<std::pair<CallStackRecord,std::size_t> > &_call_stack,
	CALL_GRAPH &_call_graph,
	const RecordType _mode) {
	if (_call_stack.empty())
		return;

	std::stack<string_id> call_stack;
	
	for (auto &call_record : _call_stack) {
		auto count = call_record.second;
		while (count > 0) {
			count--;
			if (call_record.first.get_call_status() == CALL_STATUS::CALL_IN) {
				string_id caller ;
				bool caller_found = false;
				if (!call_stack.empty()) {
					caller = call_stack.top();
					caller_found = true;
				}

				Interceptor::string_id callee;
				if (_mode == Interceptor::RecordType::FILE) {
					callee = call_record.first.get_file_data();
				}
				else if (_mode == Interceptor::RecordType::FUNCTION) {
					callee = call_record.first.get_function_name();
				}

				call_stack.push(callee);
				if (caller_found) {
					auto iter = _call_graph.find(caller);
					if (iter == _call_graph.end()) {
						_call_graph[caller][callee] = 1;
					}
					else {
						auto callee_iter = iter->second.find(callee);
						if (callee_iter == iter->second.end()) {
							iter->second[callee] = 1;
						}
						else {
							++(callee_iter->second);
						}
					}
				}

			}
			else {
				if (!call_stack.empty())
					call_stack.pop();
			}
		}
		
	}
}

void AddCallData(std::string& str, const std::string& oldStr, const std::string& newStr) {
	size_t pos = 0;
	while ((pos = str.find(oldStr, pos)) != std::string::npos) {
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}

}

void CallGraphRecorder::create_force_layout_chart(const CALL_GRAPH &_call_graph) {
	Log("creating force layout chart...");
	std::string html_data = get_header_force_layout(_call_graph) + get_connectivity_force_layout(_call_graph);
	auto str = Interceptor::html_call_graph_force_diagram;
	html_data = R"(")" + html_data + R"(")";
	AddCallData(str, html_data_key, html_data);
	std::ofstream ofs;
	auto directory = Utils::get_current_directory();
	ofs.open(directory + "\\call_graph_force_view.html");
	std::cout << "Saving the file at : " << (directory + "\\call_graph_force_view.html") << std::endl;
	ofs << str;
}

std::string CallGraphRecorder::get_package_names_dependency_graph(const CALL_GRAPH &_call_graph,
																std::map<string_id,std::size_t> &_id)const {
	Log("generating the package names for the dependency wheel...");
	std::size_t id = 0;
	std::set<string_id> function_names;
	for (auto &call : _call_graph) {
		function_names.insert(call.first);
		for (auto &call_details : call.second) {
			function_names.insert(call_details.first);
		}
	}
	std::stringstream str;
	for (auto iter = function_names.begin(); iter != function_names.end(); ++iter) {
		auto nextIter = iter;
		nextIter++;
		auto fn_name = m_string_indexer[*iter];
		if (fn_name.empty())
			fn_name = "unknown_function";
		str << "'" << fn_name << "'" << (nextIter == function_names.end() ? "" : ",");
		_id[*iter] = id;
		++id;
	}

	return str.str();
}

std::string CallGraphRecorder::get_connectivity_matrix_dependency_graph(const CALL_GRAPH &_call_graph,
																		std::map<string_id, std::size_t> &_id)const {

	Log("creating connectivity matrix for the dependency wheel...");
	std::stringstream str;
	std::map<std::size_t, std::map<std::size_t, std::size_t> > matrix;

	auto total_fns = _id.size();
	// fill the empty blocks
	for (std::size_t i = 0; i < total_fns; i++) {
		for (std::size_t j = 0; j < total_fns; j++) {
			matrix[i][j] = 0;
		}
	}

	for (auto iter = _call_graph.begin(); iter != _call_graph.end(); ++iter) {
		auto fn1_id = _id[(*iter).first];
		for (auto innerIter = (*iter).second.begin(); innerIter != (*iter).second.end(); ++innerIter) {
			auto fn2_id = _id[(*innerIter).first];
			matrix[fn1_id][fn2_id] = (*innerIter).second;
		}
	}
	for (auto call_iter = matrix.begin(); call_iter != matrix.end(); ++call_iter) {
		auto next_call_iter = call_iter;
		next_call_iter++;
		str << "[";
		for (auto iter = (*call_iter).second.begin(); iter != (*call_iter).second.end(); ++iter) {
			auto nextIter = iter;
			nextIter++;
			str << ((*iter).second>0?1:0) << (nextIter == (*call_iter).second.end() ? "" : ",");
		}
		str << "]"<<(next_call_iter==matrix.end()?"":",");

	}
	return str.str();
}

void CallGraphRecorder::create_dependency_graph(const CALL_GRAPH &_call_graph) {
	Log("creating dependency wheel");
	auto str = Interceptor::html_call_dependency;
	std::map<string_id,std::size_t> ids;
	auto package_names = get_package_names_dependency_graph(_call_graph,ids);
	auto matrix = get_connectivity_matrix_dependency_graph(_call_graph, ids);
	AddCallData(str, html_package_names_key, package_names);
	AddCallData(str, html_dependency_matrix_key, matrix);
	std::ofstream ofs;
	auto directory = Utils::get_current_directory();
	ofs.open(directory + "\\call_dependency_wheel.html");
	std::cout << "Saving the file at : " << (directory + "\\call_dependency_wheel.html") << std::endl;
	ofs << str;
}

void Interceptor::CallGraphRecorder::create_call_chart(InterceptorMode _mode) {
	Log("creating call chart...");
	{
		UniqueGuard guard_lazy_record_lock(m_lazy_record_lock);
		if (!m_lazy_records.empty()) {
			populate_lazy_data();
		}
	}
	
	UniqueGuard guard_lock(m_lock);
	
	CALL_GRAPH call_graph;
	Log("creating call graph...");

	for (auto &call_stacks : m_call_stack_records) {
		get_call_chart(call_stacks.second, call_graph, m_mode);
	}
	switch (_mode) {
		case Interceptor::CALL_DIAGRAM_FUNCTION:
		case Interceptor::CALL_DIAGRAM_FILES:
			create_force_layout_chart(call_graph);
			break;
		case Interceptor::CALL_DEPENDENCY_FUNCTION:
			create_dependency_graph(call_graph);
			break;
		default:
			break;
	}
	Log("chart creation complete");
}

std::string CallGraphRecorder::get_header_force_layout(const CALL_GRAPH &_call_graph)const {
	Log("generating header for the force layout chart...");
	std::set<string_id> function_names;
	for (auto &call : _call_graph) {
		function_names.insert(call.first);
		for (auto &call_details : call.second) {
			function_names.insert(call_details.first);
		}
	}
	std::stringstream str;
	str << function_names.size() << "|";
	for (auto &function_name : function_names) {
		str << m_string_indexer[function_name] << "|";
	}
	for (auto &function_name : function_names) {
		auto fn = m_string_indexer[function_name];
		str << fn << ";" << fn << ";"<< 0.01 << "|";
	}
	
	return str.str();
}

std::string CallGraphRecorder::get_connectivity_force_layout(const CALL_GRAPH &_call_graph)const {
	Log("generating connectivity data for the force layout...");
	std::stringstream str;
	for (auto &call : _call_graph) {
		auto fn1 = m_string_indexer[call.first];
		for (auto &call_details : call.second) {
			auto fn2 = m_string_indexer[call_details.first];
			str << fn1 << ";";
			str << fn2 << ";" <<0.01<< "|";
		}
	}
	return str.str();
}

