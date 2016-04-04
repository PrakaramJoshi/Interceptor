#pragma once
#include <map>
#include <thread>
#include <vector>
#include "CallStackRecord.h"
namespace Interceptor {
	typedef std::map<STD_STRING, std::map<STD_STRING, std::size_t> > CALL_GRAPH;
	class CallGraphRecorder {
		
		std::map<std::thread::id,std::vector<CallStackRecord > > m_call_stack_records;

		STD_STRING get_header(const CALL_GRAPH &_call_graph)const;

		STD_STRING get_connectivity(const CALL_GRAPH &_call_graph)const;

	public:
		CallGraphRecorder();

		~CallGraphRecorder();

		void record(const STD_STRING &_function,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		void print();
		
		void create_call_chart();
		
		
	};
};

