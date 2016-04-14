#pragma once
#include <atomic>
#include <thread>
namespace Interceptor {

	class UniqueGuard;
	class NonRecursiveLock {
		// the tasks are being reschudeled on the same thread.
		// this leads the mutex to not behave properly as its thread aware
		// this mostly kicks in when std::async is being intercepted.

		std::atomic<bool> m_locked;

		std::atomic<bool> m_notify_flag;

		std::atomic<bool> m_flow_waiting;

		void lock() {
			bool falseVal = false;
			while (true) {
				if (m_locked.compare_exchange_strong(falseVal, true, std::memory_order::memory_order_seq_cst)) {
					return;
				}
				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}
		}

		void unlock() {
			m_locked = false;
		}

		void wait() {
			unlock();
			bool trueVal = true;
			while (true) {
				m_flow_waiting = true;
				if (m_notify_flag.compare_exchange_strong(trueVal, false, std::memory_order::memory_order_seq_cst)) {
					m_flow_waiting = false;
					lock();
					return;
				}
				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}
		}

	public:

		friend UniqueGuard;

		NonRecursiveLock() {
			m_locked = false;
			m_notify_flag = false;
			m_flow_waiting = false;
		}

		~NonRecursiveLock() {
			lock();

		}

		void notify() {
			m_notify_flag.store(m_flow_waiting);	
		}
	};

	class UniqueGuard {

		NonRecursiveLock	&m_lock;

		UniqueGuard() = delete;
		UniqueGuard(const UniqueGuard &) = delete;
		UniqueGuard(UniqueGuard &&) = delete;
		UniqueGuard& operator = (const UniqueGuard& ) = delete;
		UniqueGuard& operator = (const UniqueGuard&&) = delete;

	public:

		UniqueGuard(NonRecursiveLock &_lock) :m_lock(_lock) {
			m_lock.lock();
		}

		~UniqueGuard() {
			m_lock.unlock();
		}

		void wait() {
			m_lock.wait();
		}

		void notify() {
			m_lock.notify();
		}
	};
}
