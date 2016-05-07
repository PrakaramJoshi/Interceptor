#pragma once
#include "InterceptorConfig.h"
#include "StringIndexer.h"
#include <string>
#include <thread>
#include <vector>
namespace Interceptor {
	enum class CALL_STATUS { CALL_IN, CALL_OUT };
	class CallStackRecord {

		string_id m_function;

		string_id m_function_file_data;

		CALL_STATUS m_call_status;

	public:
		CallStackRecord(const string_id &_function_name,
			const string_id &_function_file_path,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		bool operator ==(const CallStackRecord &_record)const;

		bool fn_equal(const CallStackRecord &_record)const;

		string_id get_function_name()const;

		string_id get_file_data()const;

		CALL_STATUS get_call_status()const;
	};

	// maps of thread id to the collection of calls stack records
	// if the call is Fn in ->Fn Out -> Fn in ->Fn out pattern then the count(second parameter in the pair 
	// is incremented
	typedef std::map<std::thread::id, std::vector<std::pair<CallStackRecord, std::size_t>  > > CallStackPerThread;

	//CALL_GRAPH : map of string id of function name to all the functions it calls and the count 
	typedef std::map<string_id, std::map<string_id, std::size_t> > CALL_GRAPH;

	class CallStackUtils {
	public:
		static void get_call_chart(const std::vector<std::pair<CallStackRecord, std::size_t> > &_call_stack,
			CALL_GRAPH &_call_graph,
			const RecordType &_mode);
		static void check_call_stack(const std::vector<std::pair<CallStackRecord, std::size_t> > &_call_stack);
	};
}
