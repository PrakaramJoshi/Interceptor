#pragma once
#include <string>
#include "StringIndexer.h"
namespace Interceptor {
	enum class CALL_STATUS { CALL_IN, CALL_OUT };
	class CallStackRecord {

		string_id m_function;

		string_id m_function_file_path;

		CALL_STATUS m_call_status;

	public:
		CallStackRecord(const string_id &_function_name,
			const string_id &_function_file_path,
			CALL_STATUS _call_status = CALL_STATUS::CALL_IN);

		string_id get_name()const;

		CALL_STATUS get_call_status()const;
	};
}
