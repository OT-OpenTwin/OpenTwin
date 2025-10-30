// @otlicense
// File: CurrentProjectAPI.h
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
#include "OTFrontendConnectorAPI/OTFrontendConnectorAPIExport.h"

// std header
#include <string>

namespace ot {
	class OT_FRONTEND_CONNECTOR_API_EXPORT CurrentProjectAPI {
	public:
		static void activateModelVersion(const std::string& _versionName);

	protected:
		virtual void activateModelVersionAPI(const std::string& _versionName) = 0;

		CurrentProjectAPI();
		virtual ~CurrentProjectAPI();
	private:
		static CurrentProjectAPI* instance();
		static CurrentProjectAPI*& getInstanceReference();
	};
}