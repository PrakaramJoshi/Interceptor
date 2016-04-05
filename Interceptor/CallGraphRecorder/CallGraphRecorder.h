#pragma once
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include "CallStackRecord.h"
namespace Interceptor {
	typedef std::map<std::string, std::map<std::string, std::size_t> > CALL_GRAPH;
	class CallGraphRecorder {
		
		std::map<std::thread::id,std::vector<CallStackRecord > > m_call_stack_records;

		std::mutex m_mutex;

		std::string get_header(const CALL_GRAPH &_call_graph)const;

		std::string get_connectivity(const CALL_GRAPH &_call_graph)const;

	public:
		CallGraphRecorder();

		~CallGraphRecorder();

		void record(const std::string &_function_name,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		void print();
		
		void create_call_chart();
		
		
	};
};

