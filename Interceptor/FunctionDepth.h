#pragma once
#include "NonRecursiveLock.h"
#include <map>
#include <thread>
namespace Interceptor {

	// a class to record the current stack depth for each stack.
	// thread safe
	class FuntionDepth {
		mutable NonRecursiveLock m_mutex;
		std::map<std::thread::id, std::size_t> m_function_depth;
	public:
		std::size_t operator++();

		std::size_t operator--();

		std::size_t load(const std::thread::id &_id)const;

		std::size_t load()const;

		~FuntionDepth();
	};

}

