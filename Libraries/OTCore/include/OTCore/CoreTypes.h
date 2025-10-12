//! @file CoreTypes.h
//! @brief This file contains the core defines of OpenTwin.
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin System header
#include "OTSystem/SystemTypes.h"

// OpenTwin Core header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <list>
#include <limits>
#include <type_traits>

namespace ot {

	//! @brief ID type used to identify a service (16 bit unsigned integer).
	typedef unsigned short serviceID_t;

	//! @brief The invalid service ID, a Service must not have this ID assigned.
	static const serviceID_t invalidServiceID = 0;

	//! @brief The maximum number a service ID may reach.
	static const serviceID_t maxServiceID = UINT16_MAX;

#ifdef OT_OS_64Bit
	//! @brief Unique identifier (64 bit unsigned integer)
	typedef uint64_t UID;

#else
	//! @brief Unique identifier (32 bit unsigned integer)
	typedef uint32_t UID;

#endif // DEBUG

	//! @brief Unique identifier list
	typedef std::list<UID> UIDList;
}

