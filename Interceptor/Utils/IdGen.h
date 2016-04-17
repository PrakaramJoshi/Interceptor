#pragma once
#include <vector>
#include <atomic>
namespace Interceptor {

	class IDGen {
		std::atomic<std::size_t> m_current_id;

		IDGen(const IDGen &) = delete;
		IDGen& operator=(const IDGen &) = delete;
		IDGen(IDGen &&) = delete;
		IDGen& operator=(IDGen &&) = delete;
	public:

		IDGen() {
			m_current_id = 0;
		}

		std::size_t get_next_id() {
			return m_current_id.fetch_add(1);
		}
	};
}
