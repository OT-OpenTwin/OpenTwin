// @otlicense
// File: ReturnMessageProposal.h
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