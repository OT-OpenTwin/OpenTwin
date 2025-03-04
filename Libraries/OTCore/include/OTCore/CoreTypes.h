#pragma once

// OpenTwin header
#include "OTSystem/ArchitectureInfo.h"
#include "OTCore/Flags.h"

// std header
#include <list>
#include <limits>

#define OT_NOTHING 

namespace ot {

	//! @brief ID type used to identify a service (16 bit unsigned integer).
	typedef unsigned short serviceID_t;

	//! @brief The invalid service ID, a Service must not have this ID assigned.
	static const serviceID_t invalidServiceID = 0;

	//! @brief The maximum number a service ID may reach.
	static const serviceID_t maxServiceID = UINT16_MAX;

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

