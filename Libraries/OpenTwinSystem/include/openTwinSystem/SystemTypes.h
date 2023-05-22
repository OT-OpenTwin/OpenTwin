#pragma once

#include "OpenTwinSystem/ArchitectureInfo.h"

namespace ot {

	//! @brief Port number type (unsigned 16 bit Integer)
	typedef unsigned short int port_t;
}

#if defined(OT_OS_WINDOWS)

#include <Windows.h>

//! @brief Process handle
#define OT_PROCESS_HANDLE HANDLE

//! @brief Invalid process handle
#define OT_INVALID_PROCESS_HANDLE nullptr

//! @brief Infinite timeout
#define OT_INFINITE_TIMEOUT INFINITE

namespace  ot {
	//! @brief Instance to an loaded external library
	typedef HINSTANCE ExternalLibraryInstance_t;
}


#endif