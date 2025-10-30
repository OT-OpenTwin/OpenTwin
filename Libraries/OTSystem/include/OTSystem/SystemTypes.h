// @otlicense

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
