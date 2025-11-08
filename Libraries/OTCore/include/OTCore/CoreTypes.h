// @otlicense
// File: CoreTypes.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
	inline constexpr serviceID_t invalidServiceID = 0U;

	//! @brief Maximum number a service ID may reach.
	inline constexpr serviceID_t maxServiceID = std::numeric_limits<serviceID_t>::max();

	//! @brief Unique identifier (64 bit unsigned integer).
	//! Mainly used for entities.
	typedef uint64_t UID;

	//! @brief Invalid unique identifier.
	inline constexpr UID invalidUID = std::numeric_limits<UID>::max();

	//! @brief Unique identifier list
	typedef std::list<UID> UIDList;
}

