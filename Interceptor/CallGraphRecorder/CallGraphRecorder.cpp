#include "CallGraphRecorder.h"
#include "CallGraphHTML.h"
#include <set>
#include <stack>
using namespace Interceptor;


Interceptor::CallGraphRecorder::CallGraphRecorder() {
}

Interceptor::CallGraphRecorder::~CallGraphRecorder() {
}

void CallGraphRecorder::record(const STD_STRING &_function,
	CALL_STATUS _call_status) {

	auto thread_id = std::this_thread::get_id();
	m_call_stack_records[thread_id].emplace_back(CallStackRecord(_function, _call_status));

}

void Interceptor::CallGraphRecorder::print() {

}

void get_call_chart(std::vector<CallStackRecord> &_call_stack,
	CALL_GRAPH &_call_graph) {
	if (_call_stack.empty())
		return;

	std::stack<STD_STRING> call_stack;
	call_stack.push(INIT_STR);

	for (auto &call_record : _call_stack) {
		if (call_record.get_call_status() == CALL_STATUS::CALL_IN) {
			auto caller = call_stack.top();
			auto callee = call_record.get_normalized_name();
			call_stack.push(callee);
			//CONSOLE_OUT << callee << std::endl;
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
		else {
			call_stack.pop();
		}
	}
}
void AddCallData(STD_STRING& str, const STD_STRING& oldStr, const STD_STRING& newStr) {
	size_t pos = 0;
	while ((pos = str.find(oldStr, pos)) != std::string::npos) {
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}

}

void Interceptor::CallGraphRecorder::create_call_chart() {
	std::map<STD_STRING, std::map<STD_STRING, std::size_t> > call_graph;
	for (auto &call_stacks : m_call_stack_records) {
		get_call_chart(call_stacks.second, call_graph);
	}
	STD_STRING html_data = get_header(call_graph)+get_connectivity(call_graph);
	auto str = Interceptor::html;
#ifdef USING_WSTRING
	html_data = LR"(")" + html_data + LR"(")";
#else
	html_data = R"(")" + html_data + R"(")";
#endif
	AddCallData(str, html_data_key, html_data);
	FILE_OSTREAM ofs;
	ofs.open("call_graph_force_view.html");
	ofs << str;
}

STD_STRING CallGraphRecorder::get_header(const CALL_GRAPH &_call_graph)const {
	std::set<STD_STRING> function_names;
	for (auto &call : _call_graph) {
		function_names.insert(call.first);
		for (auto &call_details : call.second) {
			function_names.insert(call_details.first);
		}
	}
	STRSTREAM str;
	str << function_names.size() << "|";
	for (auto &function_name : function_names) {
		str << function_name << "|";
	}
	for (auto &function_name : function_names) {
		str << function_name << ";" << function_name << ";"<< 0.01 << "|";
	}
	
	return str.str();
}

STD_STRING CallGraphRecorder::get_connectivity(const CALL_GRAPH &_call_graph)const {
	STRSTREAM str;
	for (auto &call : _call_graph) {
		for (auto &call_details : call.second) {
			str << call.first << ";";
			str << call_details.first << ";" <<0.01<< "|";
		}
	}
	return str.str();
}

