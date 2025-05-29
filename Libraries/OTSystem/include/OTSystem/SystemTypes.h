//! @file SystemTypes.h
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/ArchitectureInfo.h"

// std header
#include <cstdint>

namespace ot {

	//! @typedef port_t
	//! @brief Port number type.
	typedef uint16_t port_t;

	static const port_t invalidPortNumber = 0; //! @brief Invalid port number.

	static const port_t maxPortNumber = 49151; //! @brief Maximum port number (IANA registered ports).
}

#if defined(OT_OS_WINDOWS)

#include <Windows.h>

//! @def OT_PROCESS_HANDLE
//! @brief Process handle
#define OT_PROCESS_HANDLE HANDLE

//! @def OT_INVALID_PROCESS_HANDLE
//! @brief Invalid process handle
#define OT_INVALID_PROCESS_HANDLE nullptr

//! @def OT_INFINITE_TIMEOUT
//! @brief Infinite timeout
#define OT_INFINITE_TIMEOUT INFINITE

namespace  ot {

	//! @typedef ExternalLibraryInstance_t
	//! @brief Instance type for a loaded external library.
	typedef HINSTANCE ExternalLibraryInstance_t;
}


#endif