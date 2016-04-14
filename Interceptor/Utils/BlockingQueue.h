#ifndef BUFFER_H
#define BUFFER_H

#include "NonRecursiveLock.h"

#include <thread>
#include <queue>
#include <atomic>
#include <numeric>

template<class QueueData>class BlockingQueue
{
private:
	std::queue<QueueData*> m_queue;						// Use STL queue to store data

	Interceptor::NonRecursiveLock m_mutex;				// The mutex to synchronise on

	std::atomic<bool> m_done;

	std::atomic<std::size_t > m_size;

	BlockingQueue(const BlockingQueue &);

	BlockingQueue* operator=(const BlockingQueue &);

	BlockingQueue(BlockingQueue &&);

	BlockingQueue* operator=(BlockingQueue &&);

public:

	BlockingQueue()
	{
		m_done = false;
		m_size = 0;
	}

	~BlockingQueue(){
		ShutDown();
		CleanUp();
	};

	// Add data to the queue and notify others
	bool Insert(QueueData *_data)
	{
		if (m_done)
			return false;
		
		// Acquire lock on the queue
		Interceptor::UniqueGuard lock(m_mutex);
		m_queue.push(_data);
		m_size++;
		// Notify others that data is ready
		m_mutex.notify();
		return true;
	} // Lock is automatically released here

	// Get data from the queue. Wait for data if not available
	bool Remove(QueueData **_data)
	{
		QueueData* data = nullptr;
		// Acquire lock on the queue
		Interceptor::UniqueGuard lock(m_mutex);

		// When there is no data, wait till someone fills it.
		// Lock is automatically released in the wait and obtained
		// again after the wait
		while (m_queue.size() == 0 && !m_done) lock.wait();

		if (m_queue.size() == 0 && m_done)
		{
			m_mutex.notify();
			return false;
		}
		// Retrieve the data from the queue
		*_data = &(*m_queue.front()); m_queue.pop();
		m_size--;
		m_mutex.notify();
		return true;
	}; // Lock is automatically released here;

	// Get data from the queue. Wait for data if not available
	bool CanInsert()
	{
		if (m_done)
			return false;
		return true;
	}; // Lock is automatically released here;

	void ShutDown()
	{
		m_done = true;
		m_mutex.notify();
	}

	std::size_t Size() {
		return m_size;
	}

	bool IsShutDown(){
		return m_done.load();
	}

	void CleanUp()
	{
		Interceptor::UniqueGuard lock(m_mutex);
		while (!m_queue.empty())
		{
			m_queue.pop();
			m_size--;
		}
	}
};

#endif