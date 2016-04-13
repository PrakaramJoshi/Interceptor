#include "FunctionDepth.h"
#include <thread>
using namespace Interceptor;

std::size_t FuntionDepth::operator++() {

	auto id = std::this_thread::get_id();

	UniqueGuard guard(m_mutex);

	auto iter = m_function_depth.find(id);
	if (iter != m_function_depth.end()) {
		(++iter->second);
		return iter->second;
	}
	m_function_depth[id] = 1;
	return 1;
}
std::size_t FuntionDepth::operator--() {
	auto id = std::this_thread::get_id();

	UniqueGuard guard(m_mutex);

	auto iter = m_function_depth.find(id);
	if (iter != m_function_depth.end()) {
		(--iter->second);
		return iter->second;
	}
	
	return 0;
}

std::size_t FuntionDepth::load(const std::thread::id &_id)const {

	UniqueGuard guard(m_mutex);

	auto iter = m_function_depth.find(_id);
	if (iter != m_function_depth.end()) {
		return (iter->second);
	}
	return 0;
}

std::size_t FuntionDepth::load()const {

	auto id = std::this_thread::get_id();
	UniqueGuard guard(m_mutex);

	auto iter = m_function_depth.find(id);
	if (iter != m_function_depth.end()) {
		return (iter->second);
	}
	return 0;
}
FuntionDepth::~FuntionDepth() {
	UniqueGuard guard(m_mutex);
}