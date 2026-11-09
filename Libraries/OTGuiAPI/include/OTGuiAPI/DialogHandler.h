// @otlicense
// File: DialogHandler.h
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

// OpenTwin header
#include "OTGui/Dialog/PropertyDialogCfg.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

// std header
#include <string>
#include <optional>

namespace ot
{

	//! @class Frontend
	//! @brief Frontend request layer.
	//! The Frontend prvoides a set methods that may be used to send requests to the frontend.
	class OT_GUIAPI_API_EXPORT DialogHandler
	{
		OT_DECL_NOCOPY(DialogHandler)
		OT_DECL_NOMOVE(DialogHandler)
	public:
		DialogHandler() = default;

		static JsonDocument createDialogRequest(const PropertyDialogCfg& _cfg, const std::string& _callbackAction);

		static JsonDocument createDialogRequest(const DialogCfg* _cfg, const std::string& _requestAction, const std::string& _callbackAction, const std::optional<std::string>& _additionalInfo);
	};
}