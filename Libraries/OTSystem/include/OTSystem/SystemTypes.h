//! \file SystemTypes.h
//! \author Alexander Kuester (alexk95)
//! \date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTSystem/ArchitectureInfo.h"

namespace ot {

	//! \typedef port_t
	//! \brief Port number type.
	typedef unsigned short int port_t;
}

#if defined(OT_OS_WINDOWS)

#include <Windows.h>

//! \def OT_PROCESS_HANDLE
//! \brief Process handle
#define OT_PROCESS_HANDLE HANDLE

//! \def OT_INVALID_PROCESS_HANDLE
//! \brief Invalid process handle
#define OT_INVALID_PROCESS_HANDLE nullptr

//! \def OT_INFINITE_TIMEOUT
//! \brief Infinite timeout
#define OT_INFINITE_TIMEOUT INFINITE

namespace  ot {

	//! \typedef ExternalLibraryInstance_t
	//! \brief Instance type for a loaded external library.
	typedef HINSTANCE ExternalLibraryInstance_t;
}


#endif