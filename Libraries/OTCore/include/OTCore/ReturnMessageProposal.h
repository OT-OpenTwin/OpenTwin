// @otlicense

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"

// std header
#include <string>

namespace ot {

	class ReturnMessage2 {
	public:
		enum ReturnMessageFlag {
			Ok = 0 << 0, //! @brief Ok.
			GeneralError = 1 << 0, //! @brief General error during execution.
			IOError = 1 << 1, //! @brief Error while transmitting data.
			DeserializeFailed = 1 << 2, //! @brief Error while deserializing data.

			//! @brief All failed and error flags.
			Failed = GeneralError | IOError | DeserializeFailed,
		};
	};
}

OT_ADD_FLAG_FUNCTIONS(ot::ReturnMessage2::ReturnMessageFlag)