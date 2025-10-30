// @otlicense
// File: aWindowEventHandler.h
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

 // AK header
#include <akCore/globalDataTypes.h>

namespace ak {
	class UICORE_API_EXPORT aWindowEventHandler {
	public:
		aWindowEventHandler() {}

		virtual ~aWindowEventHandler() {}

		// ###################################################################################

		// Event handling routines

		//! @brief This function will be called by the window when the closeEvent occurs
		//! If the return value is true the closeEvent will be acceptet otherwise ignored 
		virtual bool closeEvent(void) { return true; }
	};
}