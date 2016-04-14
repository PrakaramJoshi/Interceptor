#ifndef MEMORY_CACHE_H
#define MEMORY_CACHE_H
#include <deque>
#include <map>
#include <vector>
#include "NonRecursiveLock.h"
#include <atomic>
template<typename TYPE>
class MemoryCache{
	std::deque<TYPE*> m_available_mem;

	std::map<unsigned int, std::deque<TYPE*> >m_contiguous_mem;
	Interceptor::NonRecursiveLock m_mutex;
	Interceptor::NonRecursiveLock m_mutex_array;
	unsigned int m_max_size;
	MemoryCache(const MemoryCache &);
	MemoryCache* operator = (const MemoryCache &);
	std::atomic<unsigned long long> m_count_new;
	std::atomic<unsigned long long> m_count_reused;

public:

	MemoryCache(const unsigned int &_maxsize){
		m_max_size= _maxsize;
		m_count_reused = 0;
		m_count_new = 0;
		
	}

	MemoryCache(){
		m_max_size = 100;
		m_count_reused = 0;
		m_count_new = 0;
	}

	void fill_cache() {
		Interceptor::UniqueGuard lock(m_mutex);
		while(m_available_mem.size() < m_max_size) {
			m_available_mem.push_back(new TYPE());
		}
	}

	void add_to_cache(TYPE *&_mem){
		Interceptor::UniqueGuard lock(m_mutex);
		if (m_available_mem.size() <= m_max_size){
			m_available_mem.push_back(_mem);
		}
		else {
			delete _mem;
		}
		_mem = nullptr;
	}

	void add_to_cache_array(TYPE *&_mem, unsigned int &_count){
		Interceptor::UniqueGuard lock(m_mutex_array);
		if (m_contiguous_mem.size() <= m_max_size){
			m_contiguous_mem[_count].push_back(_mem);
		}
		else
			delete[] _mem;
		_mem = nullptr;
	}

	TYPE * get_mem(){
		Interceptor::UniqueGuard lock(m_mutex);
		if (m_available_mem.empty()){
			m_count_new++;
			return new TYPE();
		}
		m_count_reused++;
		auto mem = m_available_mem.back();
		m_available_mem.pop_back();
		return mem;
	}

	TYPE *get_mem_array(unsigned int &_count){
		Interceptor::UniqueGuard lock(m_mutex_array);
		if (_count == 0)
			return nullptr;

		auto iter = m_contiguous_mem.find(_count);
		if (iter != m_contiguous_mem.end()){
			// correct size
			// pop the last and return
			m_count_reused++;
			auto mem = (*iter).second.back();
			(*iter).second.pop_back();
			if ((*iter).second.empty())
				m_contiguous_mem.erase(iter);
			return mem;
		}
		m_count_new++;
		return new TYPE[_count]();
	}

	int get_reuse_count(){ return m_count_reused; }

	int get_new_count(){ return m_count_new; }

	~MemoryCache(){
		Interceptor::UniqueGuard lock(m_mutex);
		Interceptor::UniqueGuard lock2(m_mutex_array);
		for (auto mem : m_available_mem)
			delete mem;
		for (const auto & mem : m_contiguous_mem){
			for (auto mem_obj : mem.second){
				delete[] mem_obj;
			}
		}
	}
};

#endif