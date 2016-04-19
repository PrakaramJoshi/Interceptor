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

}

Interceptor::CallGraphRecorder::~CallGraphRecorder() {

	UniqueGuard guard_lock(m_lock);
	UniqueGuard guard_lazy_record_lock(m_lazy_record_lock);
}

void CallGraphRecorder::set_record_type(RecordType _mode) {
	m_mode = _mode;
}

std::string CallGraphRecorder::get_string_from_id(const string_id &_id)const {
	return Interceptor_Internal::get().get_string_from_id(_id);
}

void CallGraphRecorder::suppress_ids(std::set<string_id> &_ids) {
	m_suppressed_ids = _ids;
}

void CallGraphRecorder::record_compression(const string_id &_function_name,
	const string_id &_function_file_path,
	const std::thread::id &_thread_id,
	CALL_STATUS _call_status) {
	if (m_mode == RecordType::FILE) {
		if (m_suppressed_ids.find(_function_file_path) != m_suppressed_ids.end())
			return;
	}
	else if (m_mode == RecordType::FUNCTION) {
		if (m_suppressed_ids.find(_function_name) != m_suppressed_ids.end())
			return;
	}
	UniqueGuard guard_lock(m_lock);
	auto fn_id = _function_name;
	auto fn_file_id = _function_file_path;
	auto iter = m_call_stack_records.find(_thread_id);
	if (iter != m_call_stack_records.end()) {
		if (!(*iter).second.empty()) {
			CallStackRecord call_record(fn_id, fn_file_id, _call_status);
			auto & last_record = (*iter).second.back();
			if (last_record.first == call_record) {
				last_record.second = last_record.second + 1;
				return;
			}
			if (_call_status == CALL_STATUS::CALL_OUT) {
				if ((*iter).second.size() >= 3) {
					// last record check for IN
					if (last_record.first.fn_equal(call_record) && last_record.first.get_call_status() == CALL_STATUS::CALL_IN) {
						auto iterback = (*iter).second.rbegin();
						iterback++;
						//second last record check for OUT
						if ((*iterback).first.fn_equal(call_record) && (*iterback).first.get_call_status() == CALL_STATUS::CALL_OUT) {
							iterback++;
							//third last record check for IN
							if ((*iterback).first.fn_equal(call_record) && (*iterback).first.get_call_status() == CALL_STATUS::CALL_IN) {
								// found Fn in ->Fn Out -> Fn in ->Fn out pattern
								(*iterback).second = (*iterback).second + (*iter).second.back().second;
								(*iter).second.pop_back();
								(*iter).second.back().second = (*iter).second.back().second + 1;
								return;
							}
						}
					}
				}

			}
		}
	}
	m_call_stack_records[_thread_id].emplace_back(std::make_pair(CallStackRecord(fn_id, fn_file_id, _call_status), 1));
}

void CallGraphRecorder::record(const string_id &_fn_id,
										const string_id &_file_id,
										CALL_STATUS _call_status) {
	auto thread_id = std::this_thread::get_id();
	record_compression(_fn_id, _file_id, thread_id, _call_status);
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
			string_id fn_id = Interceptor_Internal::get().get_function_file_internal(pa);
			string_id fn_file_id = Interceptor_Internal::get().get_function_name_internal(pa);
			record_compression(fn_id, fn_file_id, thread.first, thread_data.get_call_status());
		}
	}
	Log("lazy symbols evaluated!");
}

void Interceptor::CallGraphRecorder::print() {

}

void check_call_stack(const std::vector<std::pair<CallStackRecord, std::size_t> > &_call_stack) {
	std::map<string_id, int64_t> counts;
	for (auto &call : _call_stack) {
		auto fn_name = call.first.get_function_name();
		auto iter = counts.find(fn_name);
		if (iter == counts.end())
			counts[fn_name] = 0;
		if (call.first.get_call_status() == CALL_STATUS::CALL_IN) {
			counts[fn_name] = counts[fn_name] + call.second;
		}
		else {
			counts[fn_name] = counts[fn_name] - call.second;
		}
	}
	for (auto &c : counts) {
		if (c.second != 0) {
			std::cout << c.first << " " << c.second << std::endl;
		}
	}
}

void get_call_chart(const std::vector<std::pair<CallStackRecord,std::size_t> > &_call_stack,
	CALL_GRAPH &_call_graph,
	const RecordType _mode) {
//	check_call_stack(_call_stack);

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
		auto fn_name = get_string_from_id(*iter);
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

void CallGraphRecorder::create_dependency_graph_for_call_graph(
	const CALL_GRAPH &_call_graph,
	const std::string &_cell_title,
	const std::size_t &_chart_number,
	const std::size_t &_colspan,
	std::string &_data_script,
	std::string &_rows_placeholder) {

	std::map<string_id, std::size_t> ids;
	auto package_names = get_package_names_dependency_graph(_call_graph, ids);
	auto matrix = get_connectivity_matrix_dependency_graph(_call_graph, ids);

	std::string chart_number_str = Utils::get_string(_chart_number);
	std::string chart_name = "chart" + chart_number_str;
	std::string data_name = "data" + chart_number_str;
	std::string chart_placholder_name = "chart_placeholder" + chart_number_str;
	std::string row_placholders = Interceptor::html_dependency_wheel_chart_placholder;
	std::string data_script = Interceptor::html_dependency_wheel_chart_script;
	AddCallData(data_script, html_package_names_key, package_names);
	AddCallData(data_script, html_dependency_matrix_key, matrix);
	AddCallData(data_script, html_dependency_wheel_data_key, data_name);
	AddCallData(data_script, html_dependency_wheel_chart_key, chart_name);
	AddCallData(data_script, html_dependency_wheel_char_placeholder_key, chart_placholder_name);
	_data_script.append("\n");
	_data_script = _data_script + data_script;
	AddCallData(row_placholders, html_dependency_wheel_thread_key, _cell_title);
	AddCallData(row_placholders, html_dependency_wheel_char_placeholder_key, chart_placholder_name);
	AddCallData(row_placholders, html_dependency_wheel_colspan_key, Utils::get_string(_colspan));
	_rows_placeholder.append("\n");

	if (_colspan == 2) {
		_rows_placeholder = _rows_placeholder + "<tr>\n" + row_placholders + "</tr>\n";
	}
	else if (_chart_number % 2 == 1) {
		_rows_placeholder = _rows_placeholder + "<tr>\n" + row_placholders;
	}
	else {
		_rows_placeholder = _rows_placeholder + row_placholders + "</tr>\n";
	}
}

void CallGraphRecorder::create_dependency_graph_per_thread(std::string &_data_script,
														std::string &_rows_placeholder) {
	Log("creating dependency wheel per thread...");
	std::size_t chart_number = 1;
	for (auto &call_stacks : m_call_stack_records) {
		Log("creating call graph...");
		CALL_GRAPH call_graph;
		get_call_chart(call_stacks.second, call_graph, m_mode);
		create_dependency_graph_for_call_graph(call_graph,
											"Thread Id: "+Utils::get_string(call_stacks.first), 
											chart_number,
											1,
											_data_script,
											_rows_placeholder);
		chart_number++;
	}

	if (chart_number % 2 == 0) {
		_rows_placeholder.append("</tr>\n");
	}
}

void CallGraphRecorder::create_dependency_graph(const CALL_GRAPH &_call_graph) {

	Log("creating dependency wheel");
	
	std::string row_all_placeholders = "";
	std::string all_data_scripts = "";

	create_dependency_graph_for_call_graph(_call_graph,
											"Process", 
											0, 
											2,
											all_data_scripts,
											row_all_placeholders);

	create_dependency_graph_per_thread(all_data_scripts, row_all_placeholders);
	
	
	auto str = Interceptor::html_call_dependency;
	AddCallData(str, html_dependency_wheel_placeholder_key, row_all_placeholders);
	AddCallData(str, html_dependency_wheel_all_charts_key, all_data_scripts);
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
	
	
	Log("creating call graph...");
	CALL_GRAPH call_graph;
	for (auto &call_stacks : m_call_stack_records) {
		get_call_chart(call_stacks.second, call_graph, m_mode);
	}
	
	switch (_mode) {
		case Interceptor::FORCE_DIAGRAM:
			create_force_layout_chart(call_graph);
			break;
		case Interceptor::DEPENDENCY_WHEEL:
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
		str << get_string_from_id(function_name) << "|";
	}
	for (auto &function_name : function_names) {
		auto fn = get_string_from_id(function_name);
		str << fn << ";" << fn << ";"<< 0.01 << "|";
	}
	
	return str.str();
}

std::string CallGraphRecorder::get_connectivity_force_layout(const CALL_GRAPH &_call_graph)const {
	Log("generating connectivity data for the force layout...");
	std::stringstream str;
	for (auto &call : _call_graph) {
		auto fn1 = get_string_from_id(call.first);
		for (auto &call_details : call.second) {
			auto fn2 = get_string_from_id(call_details.first);
			str << fn1 << ";";
			str << fn2 << ";" <<0.01<< "|";
		}
	}
	return str.str();
}

