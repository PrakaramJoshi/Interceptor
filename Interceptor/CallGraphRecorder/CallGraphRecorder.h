#pragma once
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include "CallStackRecord.h"
#include "CallStackLazyRecord.h"
#include "StringIndexer.h"
#include "InterceptorConfig.h"
#include "NonRecursiveLock.h"
#include "SymbolDB.h"
#include <atomic>
namespace Interceptor {
	typedef std::map<string_id, std::map<string_id, std::size_t> > CALL_GRAPH;
	
	class CallGraphRecorder {
		
		std::map<std::thread::id,std::vector<std::pair<CallStackRecord,std::size_t>  > >	m_call_stack_records;

		std::map<std::thread::id, std::vector<CallStackLazyRecord> >						m_lazy_records;

		std::set<string_id>																	m_suppressed_ids;

		NonRecursiveLock																	m_lock;

		NonRecursiveLock																	m_lazy_record_lock;

		std::atomic<bool>																	m_mutex_locked;

		RecordType																			m_mode;

		std::string get_header_force_layout(const CALL_GRAPH &_call_graph)const;

		std::string get_package_names_dependency_graph(const CALL_GRAPH &_call_graph,
														std::map<string_id, std::size_t> &_id)const;

		std::string get_connectivity_matrix_dependency_graph(const CALL_GRAPH &_call_graph,
															std::map<string_id, std::size_t> &_id)const;

		std::string get_connectivity_force_layout(const CALL_GRAPH &_call_graph)const;

		void create_force_layout_chart(const CALL_GRAPH &_call_graph);

		void create_dependency_graph(const CALL_GRAPH &_call_graph);

		void populate_lazy_data();
		
		void record_compression(const string_id &_function_name,
			const string_id &_function_file_path,
			const std::thread::id &_thread_id,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		std::string get_string_from_id(const string_id &_id)const;

	public:
		CallGraphRecorder();

		~CallGraphRecorder();

		void suppress_ids(std::set<string_id> &_ids);

		void set_record_type(RecordType _mode);

		void record_lazy (void *_pa,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		void record(const string_id &_fn_id,
			const string_id &_file_id,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		void print();
		
		void create_call_chart(InterceptorMode _mode);
		
	};
};

