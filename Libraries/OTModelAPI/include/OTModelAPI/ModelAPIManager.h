// @otlicense
// File: ModelAPIManager.h
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
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTModelAPI/OTModelAPIExport.h"

// std header
#include <string>

namespace ot {

	class OT_MODELAPI_API_EXPORT ModelAPIManager {
		OT_DECL_NOCOPY(ModelAPIManager)
		OT_DECL_NOMOVE(ModelAPIManager)
	public:
		static void setModelServiceURL(const std::string& _url);
		static const std::string& getModelServiceURL(void);

		static bool sendToModel(MessageType _messageType, const std::string& _message, std::string& _response);
		static bool sendToModel(MessageType _messageType, const JsonDocument& _doc, std::string& _response);

	private:
		static ModelAPIManager& instance(void);

		ModelAPIManager();
		~ModelAPIManager();

		std::string m_serviceUrl;
	};

}