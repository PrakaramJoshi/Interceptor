#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <mutex>

namespace Interceptor {

	class NonRecursiveLock {
		// the tasks are being reschudeled on the same thread.
		// this leads the mutex to not behave properly as its thread aware
		// this mostly kicks in when std::async is being intercepted.

		std::atomic<bool> m_locked;

	public:

		NonRecursiveLock() {
			m_locked = false;
		}

		~NonRecursiveLock() {
			lock();

		}

		void lock() {
			bool falseVal = false;
			while (true) {
				if (m_locked.compare_exchange_strong(falseVal, true,std::memory_order::memory_order_seq_cst)) {
					return;
				}
				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}
		}

		void unlock() {
			m_locked = false;
		}

	};

	class KeyVal {
		std::string m_key;

		std::string m_val;

		std::string m_key_i;

		KeyVal() = delete;

	public:

		KeyVal(const std::string &_key,
			const std::string &_val);

		std::string key()const;

		std::string val()const;

		bool is_key(const std::string &_key)const;

		bool is_key_i(const std::string &_key)const;

	};

	class Utils {
	public:

		static std::string get_file_name_from_path(const std::string &_file_path);

		static std::string get_current_directory();

		static void split(const std::string &_str,
						std::vector<std::string> &_tokens,
						const char delimiter);

		static bool read_key_val_file(const std::string &_file_path,
									std::vector<KeyVal> &_keyVals,
									const char _delimiter,
									const char _comment);

		static bool get_key_val(const std::string &_str, 
								std::string &_key, 
								std::string &_val,	
								const char delimiter);

		static std::string get_lower(const std::string &_str);

		static void trim_leading_white_space(std::string &_str);

		static void trim_trailing_white_space(std::string &_str);

		static void trim_leading_trailing_white_space(std::string &_str);
	};
}
#pragma once
