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

	public:

		friend UniqueGuard;

		NonRecursiveLock() {
			m_locked = false;
		}

		~NonRecursiveLock() {
			lock();

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

	};
}
