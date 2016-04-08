#pragma once
#include "CallStackRecord.h"
namespace Interceptor {
	class CallStackLazyRecord {
		void *m_pa;
		CALL_STATUS m_call_status;
	public:
		CallStackLazyRecord(void *_pa, const CALL_STATUS &_call_status) {
			m_pa = _pa;
			m_call_status = _call_status;
		}

		void* get_pa()const { 
			return m_pa;
		}

		CALL_STATUS get_call_status()const {
			return m_call_status;
		}
		
	};
}
