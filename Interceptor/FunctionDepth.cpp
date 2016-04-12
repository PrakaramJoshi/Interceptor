#include "FunctionDepth.h"
#include <thread>
using namespace Interceptor;

std::size_t FuntionDepth::operator++() {
	auto id = std::this_thread::get_id();

	m_mutex.lock();
	auto iter = m_function_depth.find(id);
	if (iter != m_function_depth.end()) {
		(++iter->second);
		m_mutex.unlock();
		return iter->second;
	}
	m_function_depth[id] = 1;
	m_mutex.unlock();
	return 1;
}
std::size_t FuntionDepth::operator--() {
	auto id = std::this_thread::get_id();
	m_mutex.lock();
	auto iter = m_function_depth.find(id);
	if (iter != m_function_depth.end()) {
		(--iter->second);
		m_mutex.unlock();
		return iter->second;
	}
	m_mutex.unlock();
	return 0;
}

std::size_t FuntionDepth::load(const std::thread::id &_id)const {
	m_mutex.lock();
	auto iter = m_function_depth.find(_id);
	if (iter != m_function_depth.end()) {
		m_mutex.unlock();
		return (iter->second);
	}
	m_mutex.unlock();
	return 0;
}

std::size_t FuntionDepth::load()const {
	auto id = std::this_thread::get_id();
	m_mutex.lock();
	auto iter = m_function_depth.find(id);
	if (iter != m_function_depth.end()) {
		m_mutex.unlock();
		return (iter->second);
	}
	m_mutex.unlock();
	return 0;
}
FuntionDepth::~FuntionDepth() {
	m_mutex.lock();
}