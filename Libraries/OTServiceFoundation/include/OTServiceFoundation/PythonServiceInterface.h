// @otlicense
// File: PythonServiceInterface.h
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
#include <string>
#include <tuple>
#include <optional>
#include <map>
#include <memory>

#include "OTCore/GenericDataStruct.h"
#include "OTCore/Variable.h"
#include "OTCore/ReturnMessage.h"
#include "OTServiceFoundation/FoundationAPIExport.h"

namespace ot
{
	//! @brief Class that serves as interface to the PythonExecutionService. Here a execution order can be assambled and send out to the pythonservice.
	class OT_SERVICEFOUNDATION_API_EXPORT PythonServiceInterface
	{
	public:
		using scriptParameter = std::optional<std::list<ot::Variable>>;
		PythonServiceInterface(const std::string& _pythonExecutionServiceURL);
		void addScriptWithParameter(const std::string& _scriptName, const scriptParameter& _scriptParameter);
		void addPortData(const std::string& _portName, const ot::JsonValue*  _data, const JsonValue* _metadata);
		void addManifestUID(ot::UID _manifestUID);
		
		ot::ReturnMessage sendExecutionOrder();

		ot::ReturnMessage sendSingleExecutionCommand(const std::string& command);

	private:
		const std::string m_pythonExecutionServiceURL;
		ot::UID m_manifestUID = ot::invalidUID;
		
		std::list<std::tuple<std::string, scriptParameter>> m_scriptNamesWithParameter;

		std::map<std::string, std::pair<const ot::JsonValue*, const ot::JsonValue*>> m_portDataByPortName;
		
		ot::JsonDocument assembleMessage();

		//! @brief Writes the port data to the database as a GridFS document.
		//! @return gridFS document ID 
		std::string writePortDataToDatabase();

		ot::JsonDocument readPortDataFromDatabase(const std::string& _gridFSDocumentID);
	};
}
