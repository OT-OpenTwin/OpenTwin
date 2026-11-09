// @otlicense
// File: PythonHeaderEventBuilder.h
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
#include "OTCore/CoreAPIExport.h"
#include <string>
#include <map>
#include "OTCore/Python/PythonEventTypes.h"
#include "OTCore/JSON/JSON.h"
#include <optional>
#include "OTCore/CoreAPIExport.h"
//! @brief Events as string: "Item Deleted", "Task Started", "Task Finished", "Table Filter Changed", "Property Updated"
class OT_CORE_API_EXPORT PythonHeaderEventBuilder
{
public:
	
	bool interpret(const std::string& _scriptContent);
	const std::string& getReport() {return m_report;}
	std::optional<std::string> getFunctionName(PythonEventType _type);
private:
	std::string m_report;
	std::map<PythonEventType, std::string> m_eventExecutionByEventType;
	//#@ Event : {name: "Delete", function : "main"}
	const std::string m_keyEventType = "name";
	const std::string m_keyFunction = "function";
	std::map<std::string, PythonEventType> m_eventTypeByName
	{
		{"itemdeleted", PythonEventType::Delete},
		{"taskstarted", PythonEventType::Execute},
		{"taskfinished", PythonEventType::Done},
		{"propertyupdated", PythonEventType::PropertyChanged},
		{"tablefilterchanged", PythonEventType::TableFilterChanged}
	};

	bool extractEventExecution(const ot::JsonDocument& _content);

};