// @otlicense
// File: DebugHelper.h
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

// Open Twin header
#include "OTCore/ServiceDebugInformation.h"

namespace ot {

	//! @class DebugHelper
	//! @brief This class provides helper methods for debugging purposes.
	class OT_CORE_API_EXPORT DebugHelper {
		OT_DECL_STATICONLY(DebugHelper)
	public:
		static std::string getSetupCompletedMessagePrefix() { return "Service Setup Completed: "; };

		//! @brief This method is used to log the service initialization completed message.
		//! @param _serviceInfo The service information of the service that has completed its initialization.
		static void serviceSetupCompleted(const ot::ServiceDebugInformation& _serviceInfo);
	};
}