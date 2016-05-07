#include "CallStackRecord.h"
#include "Interceptor_Internal.h"
#include <stack>

using namespace Interceptor;

CallStackRecord::CallStackRecord(const string_id &_function_name,
								const string_id &_function_file_path,
							CALL_STATUS _call_status) {
	m_function = _function_name;
	m_call_status = _call_status;
	m_function_file_data = _function_file_path;
}

bool CallStackRecord::operator ==(const CallStackRecord &_record)const {
	return (m_function == _record.m_function) && 
			m_call_status == _record.m_call_status && 
		m_function_file_data == _record.m_function_file_data;
}

bool CallStackRecord::fn_equal(const CallStackRecord &_record)const {
	return (m_function == _record.m_function) &&
		m_function_file_data == _record.m_function_file_data;
}

string_id CallStackRecord::get_function_name() const {
	return m_function;
}

string_id CallStackRecord::get_file_data()const {
	return m_function_file_data;
}


CALL_STATUS CallStackRecord::get_call_status() const {
	return m_call_status;
}

void CallStackUtils::check_call_stack(const std::vector<std::pair<CallStackRecord, std::size_t> > &_call_stack) {
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

void CallStackUtils::get_call_chart(const std::vector<std::pair<CallStackRecord, std::size_t> > &_call_stack,
	CALL_GRAPH &_call_graph,
	const RecordType &_mode) {
	//	check_call_stack(_call_stack);

	if (_call_stack.empty())
		return;

	std::stack<string_id> call_stack;

	for (auto &call_record : _call_stack) {
		auto count = call_record.second;
		while (count > 0) {
			count--;
			if (call_record.first.get_call_status() == CALL_STATUS::CALL_IN) {
				string_id caller;
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