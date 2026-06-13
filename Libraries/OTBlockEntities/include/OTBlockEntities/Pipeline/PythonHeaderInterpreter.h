// @otlicense
// File: PythonHeaderInterpreter.h
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
#include "OTCore/JSON/JSON.h"
#include "OTCore/Python/PythonHeaderAnalyser.h"
#include "OTModelEntities/EntityFile.h"
#include "OTBlockEntities/Connector.h"

// std header
#include <vector>

class OT_BLOCKENTITIES_API_EXPORT PythonHeaderInterpreter
{
public:
	virtual ~PythonHeaderInterpreter();

	bool interpret(std::shared_ptr<EntityFile> pythonScript);
	const std::list<ot::Connector>& getAllConnectors() const { return m_allConnectors; };
	const std::list<EntityPropertiesBase*>& getAllProperties()const { return m_allProperties; };
	const std::string& getErrorReport() const { return m_report; };
private:
	
	std::list<EntityPropertiesBase*> m_allProperties;
	std::list<ot::Connector> m_allConnectors;

	std::string m_report;

	const std::string m_connectorDefTypeIn = "in";
	const std::string m_connectorDefTypeOut = "out";
	const std::string m_connectorDefTypeInOptional = "inopt";
	const std::string m_connectorDefName = "name";

	const std::string m_defType = "type";
	const std::string m_defTitle = "label";

	const std::string m_propertyDefTypeSelection = "selection";
	const std::string m_propertyDefTypeString = "string";
	const std::string m_propertyDefTypeDouble = "double";
	const std::string m_propertyDefTypeInteger = "integer";
	const std::string m_propertyDefTypeBoolean = "boolean";

	const std::string m_propertyDefOptions = "options";
	const std::string m_propertyDefDefault = "default";

	const std::string m_propertyGroupName = "Script based";
	
	bool createConnectorsFromJSON(const PythonHeaderAnalyser::ExtractedEntry& _entryWithLineNb);
	bool createPropertiesFromJSON(const PythonHeaderAnalyser::ExtractedEntry& _entryWithLineNb);
	ot::ConnectorType getConnectorType(const ot::JsonDocument& jsonEntry, std::string& returnMessage);
	EntityPropertiesBase* createPropertyEntity(const ot::JsonDocument& jsonEntry, std::string& returnMessage);
};