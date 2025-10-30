// @otlicense
// File: SystemTypes.h
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

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTSystem/OTAssert.h"
#include "OTSystem/Exception.h"
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/ArchitectureInfo.h"

// std header
#include <stdint.h>

namespace ot {

	//! @typedef port_t
	//! @brief Port number type.
	typedef uint16_t port_t;

	static const port_t invalidPortNumber = 0; //! @brief Invalid port number.

	static const port_t maxPortNumber = 49151; //! @brief Maximum port number (IANA registered ports).
}
