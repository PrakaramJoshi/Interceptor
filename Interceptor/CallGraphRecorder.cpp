#include "CallGraphRecorder.h"
using namespace Interceptor;

CallGraphRecorder::CallGraphRecorder(void *_callee,
	const std::size_t &_depth,
	CALL_STATUS _call_status ) {
	pcallee = _callee;
	depth = _depth;
	thread_id = std::this_thread::get_id();
	call_status = _call_status;
}
void CallGraphRecorder::append(std::string &_str) {
	_str.append("-");
}

void CallGraphRecorder::append(std::wstring &_str) {
	_str.append(L"-");
}