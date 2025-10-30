// @otlicense

#pragma once

#include "OTSystem/SystemAPIExport.h"
#include "OTSystem/SystemTypes.h"

#include <string>

namespace ot {

	//! @namespace net
	namespace net {

		//! @brief Will check if a TCP connection can be established
		//! @param _port The target port
		//! @param _ip The target ip
		//! @return 0 if a connection could be established, othewise returns the error code
		OT_SYS_API_EXPORT unsigned long checkTCPConnection(port_t _port, const char * _ip);

		//! @brief Will return the last error text
		OT_SYS_API_EXPORT std::wstring getLastError(void);
	}
}