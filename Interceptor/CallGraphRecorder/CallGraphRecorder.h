#pragma once
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include "CallStackRecord.h"
#include "CallStackLazyRecord.h"
#include "StringIndexer.h"
#include "InterceptorConfig.h"
#include "InterceptorUtils.h"
#include <atomic>
namespace Interceptor {
	typedef std::map<string_id, std::map<string_id, std::size_t> > CALL_GRAPH;
	class CallGraphRecorder {
		
		std::map<std::thread::id,std::vector<CallStackRecord > > m_call_stack_records;

		std::map<std::thread::id, std::vector<CallStackLazyRecord> > m_lazy_records;

		NonRecursiveLock m_lock;

		NonRecursiveLock m_lazy_record_lock;

		std::atomic<bool >m_mutex_locked;

		mutable StringIndexer m_string_indexer;

		std::string get_header(const CALL_GRAPH &_call_graph)const;

		std::string get_connectivity(const CALL_GRAPH &_call_graph)const;

		void populate_lazy_data();

	public:
		CallGraphRecorder();

		~CallGraphRecorder();

		void record(const std::string &_function_name,
			const std::string &_function_file_path,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		void record(const std::string &_function_name,
			const std::string &_function_file_path,
			const std::thread::id &_thread_id,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		void record_lazy (void *_pa,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		void print();
		
		void create_call_chart(InterceptorMode _mode);
		
	};
};

