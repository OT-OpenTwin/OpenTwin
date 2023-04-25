#pragma once

// OpenTwin header
#include "OpenTwinSystem/ArchitectureInfo.h"
#include "OpenTwinCore/Flags.h"

// std header
#include <list>

namespace ot {

	//! @brief ID type used to identify a service (16 bit unsigned integer)
	typedef unsigned short serviceID_t;

	//! @brief The invalid service ID, a Service must not have this ID assigned
	static const serviceID_t invalidServiceID = 0;

#ifdef OT_OS_64Bit
	//! @brief Unique identifier (64 bit unsigned integer)
	typedef unsigned long long UID;
#else
	//! @brief Unique identifier (32 bit unsigned integer)
	typedef unsigned long UID;
#endif // DEBUG

	//! @brief Unique identifier list
	typedef std::list<UID> UIDList;
}

