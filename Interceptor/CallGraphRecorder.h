#pragma once
#include <thread>
namespace Interceptor {
	enum class CALL_STATUS { CALL_IN, CALL_OUT };
	class CallGraphRecorder {
		void *pcallee;
		std::size_t depth;
		CALL_STATUS call_status;
		std::thread::id thread_id;

		void append(std::string &_str);

		void append(std::wstring &_str);

	public:
		CallGraphRecorder(void *_callee,
			const std::size_t &_depth,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);
		
		template<typename OutStream>
		void print(OutStream &_ostr) {
			auto fn = get_function_name(pcallee, disable_calls);
			using StringType = decltype(fn);
			if (!fn.empty()) {
				StringType out_str;
				out_str.clear();
				for (auto i = depth; i > 0; i--) {
					append(out_str);

				}
				out_str.append(fn);
				if (call_status == CALL_STATUS::CALL_IN)
					_ostr << "(in  " << depth << ", Thread: " << thread_id << ")" << out_str << std::endl;
				else
					_ostr << "(out " << depth << ", Thread: " << thread_id << ")" << out_str << std::endl;
			}
		}
	};
};

