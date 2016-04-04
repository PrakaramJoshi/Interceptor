#pragma once
#include "InterceptorConfig.h"
namespace Interceptor {
	enum class CALL_STATUS { CALL_IN, CALL_OUT };
	class CallStackRecord {

		STD_STRING m_function;

		STD_STRING m_normalized_name;

		CALL_STATUS m_call_status;

		void create_normalized_name();

	public:
		CallStackRecord(const STD_STRING &_function,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		STD_STRING get_name()const;

		STD_STRING get_normalized_name()const;

		CALL_STATUS get_call_status()const;
	};
}
