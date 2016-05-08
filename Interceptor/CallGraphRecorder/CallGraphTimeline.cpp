#include "CallGraphTimeline.h"
#include "Interceptor_Internal.h"
#include "InterceptorUtils.h"
#include "Logger.h"
#define NODE_CHAR_WIDTH					10
#define NODE_TO_NODE_DISTANCE			50
#define NODE_BOX_HEIGHT					50
#define NODE_MIN_WIDTH					50
#define TIMELINE_TO_TIMELINE_DISTANCE	500
using namespace Interceptor;
using namespace AceLogger;
TimelineNode::TimelineNode(const std::size_t &_id,
	string_id _description) {
	m_id = _id;
	m_description = std::move(_description);
	m_width = -1;
	m_location.x = -1;
	m_location.y = -1;
}

void TimelineNode::add_call_to(const std::size_t &_id) {
	m_call_to.push_back(_id);
}

std::vector<std::size_t> TimelineNode::get_calls_to()const {
	return m_call_to;
}

std::size_t TimelineNode::get_id()const {
	return m_id;
}

string_id TimelineNode::get_description()const {
	return m_description;
}

void TimelineNode::create_location(int _cemter_x, int _y) {
	
	m_location.x = _cemter_x;
	m_location.y = _y;
}

void TimelineNode::translate_y_by(const int _dy) {
	m_location.translate_y_by(_dy);
}

int TimelineNode::x()const {
	if (m_location.x < 0) {
		throw std::runtime_error("TimelineNode location has not been calculated");
	}
	return m_location.x;
}

int TimelineNode::y()const { 
	if (m_location.y < 0) {
		throw std::runtime_error("TimelineNode location has not been calculated");
	}
	return m_location.y;
}

int TimelineNode::width()const {
	if (m_width < 0) {
		m_width = (Interceptor_Internal::get().get_string_from_id(m_description).size() + 1)*NODE_CHAR_WIDTH;
		m_width = (std::max)(NODE_MIN_WIDTH, m_width);
	}
	return m_width;
}

bool TimelineNode::get_svg(std::ostream *_ofs,
							const std::string &_svg_name)const {
	std::string box_svg = _svg_name + R"(.append('rect').attr('x','$X$').attr('y','$Y$').attr('rx','20').attr('ry','20').attr('width','$WIDTH$').attr('height','$HEIGHT$').style('fill','none').style('stroke-width','3').style('stroke','black').style('opacity','0.6'))";
	std::string x_str = Utils::get_string(x()-(width()/2));
	std::string y_str = Utils::get_string(y() -(NODE_BOX_HEIGHT/2));
	std::string width_str = Utils::get_string(width());
	std::string height_str = Utils::get_string(NODE_BOX_HEIGHT);
	Utils::replace_all(box_svg, "$X$", x_str);
	Utils::replace_all(box_svg, "$Y$", y_str);
	Utils::replace_all(box_svg, "$WIDTH$", width_str);
	Utils::replace_all(box_svg, "$HEIGHT$", height_str);

	auto str = Interceptor_Internal::get().get_string_from_id(m_description);

	std::string text_svg = _svg_name + R"(.append('text').attr('x','$X$').attr('y','$Y$').attr('dx','$DX$').attr('dy','$DY$').attr('text-anchor','middle').text('$TEXT$'))";
	x_str = Utils::get_string(x());
	y_str = Utils::get_string(y());
	std::string dx_str = "0";// Utils::get_string((-width() / 2) + 5);
	std::string dy_str = "5";
	Utils::replace_all(text_svg, "$X$", x_str);
	Utils::replace_all(text_svg, "$Y$", y_str);
	Utils::replace_all(text_svg, "$DX$", dx_str);
	Utils::replace_all(text_svg, "$DY$", dy_str);
	Utils::replace_all(text_svg, "$TEXT$", str);
	(*_ofs) << box_svg << "\n" << text_svg << "\n";
	return true;
}

bool TimelineNode::get_connection_svg(const std::map<std::size_t, string_id > &_node_id_to_node_desc,
										const std::map<string_id, TimelineNode > &_nodes,
										const std::string &_svg_name,
										std::ostream *_ofs) {
	bool returnVal = true;
	std::string node_x = Utils::get_string(x());
	std::string node_y_start = Utils::get_string(y() - (NODE_BOX_HEIGHT / 2));
	std::string node_y_end = Utils::get_string(y() + (NODE_BOX_HEIGHT / 2));
	for (auto &connection : m_call_to) {
		
		std::string connection_str_arc = _svg_name + R"(.append('path').attr('d','M $STARTX$ $STARTY$ A$RX$,$RY$ 0 $LONGSIDE$,$MIRROR$ $ENDX$ $ENDY$').attr('stroke','$COLOR$').attr('stroke-width','1').attr('fill','none'))";
		std::string connection_str_beizer = _svg_name + R"(.append('path').attr('d','M $STARTX$, $STARTY$ Q$CONTROLX$, $CONTROLY$ $ENDX$, $ENDY$').attr('stroke','$COLOR$').attr('stroke-width','1').attr('fill','none'))";
		std::string connection_str;
		auto conn_desc_iter = _node_id_to_node_desc.find(connection);
		if (conn_desc_iter == _node_id_to_node_desc.end()) {
			LogErr("unable to get connection info for id " + Utils::get_string(connection));
			returnVal = false;
		}
		else {
			auto node_iter = _nodes.find(conn_desc_iter->second);
			if(node_iter==_nodes.end()){
				LogErr("No node created for description id : " + Utils::get_string(conn_desc_iter->second));
				returnVal = false;
			}
			else {
				auto &conn_node = node_iter->second;
				auto conn_id = conn_node.get_id();
				std::string color_str = "";
				std::string node_y = "";
				std::string conn_node_x = Utils::get_string(conn_node.x());
				
				std::string start_x = "";
				std::string end_x = "";
			
				if (conn_id == m_id) {
					connection_str = connection_str_arc;
					std::string mirror = "";
					int dx = std::abs((conn_node.x() - x()) / 2);
					int dy = std::sqrt(dx) + 1;
					std::string dx_str = Utils::get_string(dx);
					std::string dy_str = Utils::get_string(dy);
					start_x = node_x; ;
					end_x = Utils::get_string(x() + 1);
					color_str = "red";
					node_y = node_y_end;
					mirror = "0";
					dx_str = "50";
					dy_str = "50";
					std::string arclong_side = "0";
					arclong_side = "1";
					Utils::replace_all(connection_str, "$STARTX$", start_x);
					Utils::replace_all(connection_str, "$STARTY$", node_y);
					Utils::replace_all(connection_str, "$ENDX$", end_x);
					Utils::replace_all(connection_str, "$ENDY$", node_y);
					Utils::replace_all(connection_str, "$RX$", dx_str);
					Utils::replace_all(connection_str, "$RY$", dy_str);
					Utils::replace_all(connection_str, "$COLOR$", color_str);
					Utils::replace_all(connection_str, "$LONGSIDE$", arclong_side);
					Utils::replace_all(connection_str, "$MIRROR$", mirror);
				}
				else {
					connection_str = connection_str_beizer;
					std::string control_x = Utils::get_string((x() + conn_node.x()) / 2);
					std::string control_y = "";
					if (conn_id > m_id) {
						start_x = node_x;
						end_x = conn_node_x;
						color_str = "blue";
						node_y = node_y_start;
						control_y = "0";
					}
					else {
						start_x = conn_node_x;
						end_x = node_x;
						color_str = "red";
						node_y = node_y_end;
						control_y = Utils::get_string(TIMELINE_TO_TIMELINE_DISTANCE);
					}
					Utils::replace_all(connection_str, "$STARTX$", start_x);
					Utils::replace_all(connection_str, "$STARTY$", node_y);
					Utils::replace_all(connection_str, "$ENDX$", end_x);
					Utils::replace_all(connection_str, "$ENDY$", node_y);
					Utils::replace_all(connection_str, "$CONTROLX$", control_x);
					Utils::replace_all(connection_str, "$CONTROLY$", control_y);
					Utils::replace_all(connection_str, "$COLOR$", color_str);
				}
								
				(*_ofs) << connection_str << "\n";
			}
		}
	}
	return true;
}

std::string TimelineNode::get_print_str()const {

	std::stringstream str;
	str << "Node id     : " << m_id << "\n"
		<< "description : " << m_description << "\n"
		<< "location    : " << m_location.x << "," << m_location.y << "\n"
		<< "width       : " << width() << "\n";
	return str.str();
}

TimeLine::TimeLine(RecordType _record_type,
					std::size_t _timeline_index)
{
	m_current_id = 0;
	m_recordtype = _record_type;
	m_timeline_id = _timeline_index;
	m_start_y = (TIMELINE_TO_TIMELINE_DISTANCE / 2);
}

void TimeLine::add_empty_node(const CallStackRecord &_callrecord) {
	string_id desc;
	switch (m_recordtype) {
		case Interceptor::FUNCTION:
			desc = _callrecord.get_function_name();
			break;
		case Interceptor::FILE:
			desc = _callrecord.get_file_data();
			break;
		default:
			break;
	}
	auto iter = m_nodes.find(desc);
	if (iter == m_nodes.end()) {
		m_nodes.insert(std::make_pair(desc, TimelineNode(m_current_id, desc)));
		m_node_id_to_node_desc[m_current_id] = desc;
		m_current_id++;
	}
}

void TimeLine::add_calls_to(const CALL_GRAPH &_callgraph) {
	for (auto &c : _callgraph) {
		auto &caller = c.first;
		auto iter = m_nodes.find(caller);
		for (auto &called : c.second) {
			auto called_node_iter = m_nodes.find(called.first);
			(*iter).second.add_call_to(called_node_iter->second.get_id());
		}
	}
}

bool TimeLine::save_to_stream(std::ostream *_ofs) {
	return get_svg(_ofs);
}

bool TimeLine::get_svg(std::ostream *_ofs) {
	bool returnVal = true;

	int max_x = fill_node_locations();
	max_x = (std::max)(max_x, TIMELINE_TO_TIMELINE_DISTANCE);
	std::string timeline_container = R"(var container$SVGID$ = d3.select('body').append('div').attr('id','container$SVGID$');
container$SVGID$.style('height',"500").style('width','100%').style('border','2px solid #000').style('overflow','auto');)";
	std::string timeline_svg = R"(var sky$SVGID$ = container$SVGID$.append('svg').attr('id', 'sky').style('height','$HEIGHT$px').style('width','$WIDTH$px');)";
	std::string timeline_id_str = Utils::get_string(m_timeline_id);
	Utils::replace_all(timeline_container, "$SVGID$", timeline_id_str);
	Utils::replace_all(timeline_svg, "$SVGID$", timeline_id_str);
	Utils::replace_all(timeline_svg, "$WIDTH$", Utils::get_string(max_x));
	
	
	std::string svg_name = "sky" + timeline_id_str;
	
	std::ostringstream str;
	for (auto &n : m_node_id_to_node_desc) {
		auto iter = m_nodes.find(n.second);
		if (!(*iter).second.get_svg(&str, svg_name)) {
			LogErr("unable to get svg for " + (*iter).second.get_print_str());
			returnVal = false;
		}
	}
	
	for (auto &n : m_node_id_to_node_desc) {
		auto iter = m_nodes.find(n.second);
		if (!(*iter).second.get_connection_svg(m_node_id_to_node_desc,m_nodes, svg_name, &str)) {
			LogErr("unable to get connection svg for " + (*iter).second.get_print_str());
			returnVal = false;
		}
	}
	Utils::replace_all(timeline_svg, "$HEIGHT$", Utils::get_string(TIMELINE_TO_TIMELINE_DISTANCE));
	(*_ofs) << timeline_container << "\n";
	(*_ofs) << timeline_svg << "\n";
	(*_ofs) << str.str();
	return returnVal;
}

int TimeLine::fill_node_locations() {
	int start_x = 10;
	int max_x = 0;
	for (auto &n : m_node_id_to_node_desc) {
		auto iter = m_nodes.find(n.second);
		start_x += ((*iter).second.width() / 2);
		(*iter).second.create_location(start_x, m_start_y);
		auto box_end = (*iter).second.x() + ((*iter).second.width()/2);
		max_x = (std::max)(max_x, box_end);
		start_x = box_end + NODE_TO_NODE_DISTANCE;
	}
	return max_x+10;
}

CallGraphTimeline::CallGraphTimeline(const CallStackPerThread &_callstack_records,
									RecordType _record_type):
m_call_stack_records(_callstack_records){
	m_recordtype = _record_type;
}

void CallGraphTimeline::create_graph(const std::string &_file_path) {
	Log("preparing to create call graph timeline...");
	m_ofs.open(_file_path);
	if (m_ofs.is_open()) {
		Log("call graph timeline graph is being created at : " + _file_path);
	}
	else {
		LogErr("unable to file at :" + _file_path);
		return;
	}
	std::string html_header = R"(<!DOCTYPE html><html><body><script type="text/javascript" src="http://mbostock.github.com/d3/d3.js"></script><script>)";
	std::string html_footer = R"(</script><style>svg#sky {border:1px dotted #ccc;background-color: #fff;}</style></body></html>)";
	m_ofs << html_header << "\n";
	bool has_errors = false;
	std::size_t index = 0;

	for (auto &record : m_call_stack_records) {
		CALL_GRAPH call_graph;
		CallStackUtils::get_call_chart(record.second, call_graph, m_recordtype);
		if (!create_graph_internal(record.first, record.second, call_graph ,index)) {
			LogErr("call graph timeline creation for thread id :" + Utils::get_string(record.first));
			has_errors = true;
		}
		index++;
	}
	m_ofs << html_footer << "\n";
	m_ofs.close();
	if (!has_errors)
		Log("call graph time line created.");
	else
		LogErr("call graph time line created with errors!");
	return;
}

bool CallGraphTimeline::create_graph_internal(const std::thread::id &_thread_id,
	const std::vector<std::pair<CallStackRecord, std::size_t>  > &_call_stack,
	const CALL_GRAPH &_callgraph,
	const std::size_t &_timeline_index) {

	TimeLine timeline(m_recordtype, _timeline_index);
	for (auto &record : _call_stack) {
		timeline.add_empty_node(record.first);
	}
	timeline.add_calls_to(_callgraph);

	return timeline.save_to_stream(&m_ofs);
}