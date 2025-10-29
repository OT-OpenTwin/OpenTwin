// @otlicense

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

#undef min
#undef max

namespace ot {

#ifndef OT_OS_64Bit
	static_assert(false, "OpenTwin requires a 64-bit architecture!");
#endif

	//! @brief ID type used to identify a service (16 bit unsigned integer) inside a session.
	typedef unsigned short serviceID_t;

	//! @brief Invalid service ID, a Service must not have this ID assigned at any point.
	static constexpr serviceID_t invalidServiceID = 0U;

	//! @brief Maximum number a service ID may reach.
	static constexpr serviceID_t maxServiceID = std::numeric_limits<serviceID_t>::max();

	//! @brief Unique identifier (64 bit unsigned integer).
	//! Mainly used for entities.
	typedef uint64_t UID;

	//! @brief Invalid unique identifier.
	static constexpr UID invalidUID = std::numeric_limits<UID>::max();

	//! @brief Unique identifier list
	typedef std::list<UID> UIDList;
}

