#pragma once
#include "InterceptorConfig.h"
#include "CallStackRecord.h"
#include "StringIndexer.h"
#include <fstream>
#include <map>
#include <thread>
#include <vector>
namespace Interceptor {

	struct point {
		int x, y;
		point() {
			x = 0; y = 0;
		}
	};

	class TimelineNode {

		std::size_t					m_id;

		std::vector<std::size_t>	m_call_to;

		string_id					m_description;

		point						m_location; //center of the box

		mutable int					m_width;

		TimelineNode() = delete;

	public:

		TimelineNode(const std::size_t &_id,
			string_id _description);

		void add_call_to(const std::size_t &_id);

		std::vector<std::size_t> get_calls_to()const;

		std::size_t get_id()const;

		string_id get_description()const;

		void create_location(int _cemter_x, int _y);

		int x()const;

		int y()const;

		int width()const;

		bool get_svg(std::ostream *_ofs)const;

		bool get_connection_svg(const std::map<std::size_t, string_id > &_node_id_to_node_desc,
								const std::map<string_id, TimelineNode > &_nodes,
								std::ostream *_ofs);

		std::string get_print_str()const;

	};

	class TimeLine {

		std::map<string_id, TimelineNode >	m_nodes;

		std::map<std::size_t, string_id >   m_node_id_to_node_desc;

		std::size_t							m_current_id;

		RecordType							m_recordtype;

		int									m_start_y;

		void fill_node_locations();

		bool get_svg(std::ostream *_ofs);

	public:

		TimeLine(RecordType _record_type, 
				std::size_t _timeline_index);

		void add_empty_node(const CallStackRecord &_callrecord);

		void add_calls_to(const CALL_GRAPH &_callgraph);

		bool save_to_stream(std::ostream *_ofs);
	};

	class CallGraphTimeline {

		//the original sequence of calls
		const CallStackPerThread	&m_call_stack_records;

		std::ofstream				m_ofs;

		RecordType					m_recordtype;

		bool create_graph_internal(const std::thread::id &_thread_id,
				const std::vector<std::pair<CallStackRecord, std::size_t>  > &_call_stack,
				const CALL_GRAPH &_callgraph,
				const std::size_t &_timeline_index);

	public:

		CallGraphTimeline(const CallStackPerThread &_callstack_records,
						RecordType _record_type);

		void create_graph(const std::string &_file_path);
	};
}
