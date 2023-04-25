#pragma once

#include "OpenTwinSystem/ArchitectureInfo.h"

namespace ot {

	//! @brief Port number type (unsigned 16 bit Integer)
	typedef unsigned short int port_t;
}

#if defined(OT_OS_WINDOWS)

#include <Windows.h>

//! Handle to a process
#define OT_PROCESS_HANDLE HANDLE

//! Invalid handle (no handle)
#define OT_INVALID_PROCESS_HANDLE nullptr

#define OT_INFINITE_TIMEOUT INFINITE

#endif