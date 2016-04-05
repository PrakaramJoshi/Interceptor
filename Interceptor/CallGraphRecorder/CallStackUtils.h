#pragma once
#include <string>
#define CONCAT_IMPL(A,B) A##B
#define CONCAT(A,B) CONCAT_IMPL(A,B)
#define SLOCK(MUTEX) std::lock_guard<std::mutex> CONCAT(__lock,__LINE__)(MUTEX);
namespace Interceptor {
	class Utils {
	public:
		static std::string get_normalized_function_name(const std::string &_function_name);
	};
}
