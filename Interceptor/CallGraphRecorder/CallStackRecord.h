#pragma once
#include <string>

namespace Interceptor {
	enum class CALL_STATUS { CALL_IN, CALL_OUT };
	class CallStackRecord {

		std::string m_function;

		CALL_STATUS m_call_status;

	public:
		CallStackRecord(const std::string &_function_name,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		std::string get_name()const;

		CALL_STATUS get_call_status()const;
	};
}
