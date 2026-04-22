// @otlicense
// File: PythonHeaderInterpreter.cpp
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
#include "OTBlockEntities/Pipeline/PythonHeaderInterpreter.h"

// std header
#include <sstream>
#include <algorithm>

PythonHeaderInterpreter::~PythonHeaderInterpreter() {
}

bool PythonHeaderInterpreter::interpret(std::shared_ptr<EntityFile> pythonScript)
{
	m_allConnectors.clear();
	m_allProperties.clear();
	m_report = "";

	const auto& fileContent = pythonScript->getDataEntity()->getData();
	std::string script(fileContent.begin(), fileContent.end());
	PythonHeaderAnalyser analyser;
	analyser.analysePythonScript(script);
	m_report =  analyser.getAnalysisReport();


	const std::list<PythonHeaderAnalyser::ExtractedEntry>& portEntries = analyser.getEntriesOfType(PythonHeaderEntryType::Port);
	bool success = true;
	for (const auto& portEntry : portEntries)
	{
		success |= createConnectorsFromJSON(portEntry);
	}

	const std::list<PythonHeaderAnalyser::ExtractedEntry>& propertyEntries = analyser.getEntriesOfType(PythonHeaderEntryType::Property);
	for (const auto& propertyEntry : propertyEntries)
	{
		success |= createPropertiesFromJSON(propertyEntry);
	}
	
	return success;
}

bool PythonHeaderInterpreter::createConnectorsFromJSON(const PythonHeaderAnalyser::ExtractedEntry& _entryWithLineNb)
{
	const ot::JsonDocument& entry = _entryWithLineNb.m_content;
	int lineNb = _entryWithLineNb.m_lineNumber;

	bool allMemberFound = true;
	std::string missingMember("");
	if (!entry.HasMember(m_connectorDefName.c_str()))
	{
		missingMember += m_connectorDefName + ", ";
		allMemberFound = false;
	}

	if (!entry.HasMember(m_defTitle.c_str()))
	{
		missingMember += m_defTitle + ", ";
		allMemberFound = false;
	}

	ot::ConnectorType connectorType = ot::ConnectorType::UNKNOWN;
	std::string status = "";
	if (!entry.HasMember(m_defType.c_str()))
	{
		allMemberFound = false;
		missingMember += m_defType + ", ";
	}
	else
	{

		connectorType = getConnectorType(entry, status);
	}

	if (allMemberFound && connectorType != ot::ConnectorType::UNKNOWN)
	{
		const std::string title = ot::json::getString(entry, m_defTitle.c_str());
		const std::string name = ot::json::getString(entry, m_connectorDefName.c_str());
		m_allConnectors.push_back({ connectorType, name, title });
	}
	else
	{
		m_report += "Could not create port in line " + std::to_string(lineNb) + " because\n"
			"following fields are missing: " + missingMember.substr(0, missingMember.size() - 2) + "\n";

		if (connectorType == ot::ConnectorType::UNKNOWN) 
		{ 
			m_report += status + "\n"; 
		}
		m_report += "\n";
	}
	return allMemberFound && connectorType != ot::ConnectorType::UNKNOWN;
}

bool PythonHeaderInterpreter::createPropertiesFromJSON(const PythonHeaderAnalyser::ExtractedEntry& _entryWithLineNb)
{
	const auto& propertyJSON = _entryWithLineNb.m_content;
	int lineNb = _entryWithLineNb.m_lineNumber;

	std::string missingEntries = "";
	bool allEntriesFound = true;

	if (!propertyJSON.HasMember(m_defTitle.c_str()))
	{
		allEntriesFound = false;
		missingEntries += m_defTitle + ", ";
	}

	EntityPropertiesBase* propertyBase = nullptr;
	std::string status("");
	if (!propertyJSON.HasMember(m_defType.c_str()))
	{
		allEntriesFound = false;
		missingEntries += m_defType + ", ";
	}
	else
	{
		std::string type = ot::json::getString(propertyJSON, m_defType.c_str());
		if (type == m_propertyDefTypeSelection && !propertyJSON.HasMember(m_propertyDefOptions.c_str()))
		{
			allEntriesFound = false;
			missingEntries += m_propertyDefOptions + ", ";
		}

		propertyBase = createPropertyEntity(propertyJSON, status);
		if (propertyBase == nullptr)
		{
			allEntriesFound = false;
		}

	}
	if (allEntriesFound)
	{
		const std::string title = ot::json::getString(propertyJSON, m_defTitle.c_str());

		propertyBase->setGroup(m_propertyGroupName);
		propertyBase->setName(title);
		m_allProperties.push_back(propertyBase);
	}
	else
	{
		m_report += "Could not create property in line " + std::to_string(lineNb) + " because\n"
			"following fields are missing: " + missingEntries.substr(0, missingEntries.size() - 2) + "\n";
		if (propertyBase == nullptr)
		{ 
			m_report += status + "\n"; 
		}
		m_report += "\n";
	}

	return allEntriesFound;
}

ot::ConnectorType PythonHeaderInterpreter::getConnectorType(const ot::JsonDocument& jsonEntry, std::string& returnMessage )
{
	std::string typeString = ot::json::getString(jsonEntry, m_defType.c_str());
	
	if (m_connectorDefTypeIn == typeString)
	{
		return ot::ConnectorType::In;
	}
	else if (m_connectorDefTypeInOptional == typeString)
	{
		return ot::ConnectorType::InOptional;
	}
	else if (m_connectorDefTypeOut == typeString)
	{
		return ot::ConnectorType::Out;
	}
	else
	{
		returnMessage = "Port type not supported or misspelled: " +typeString;
		return ot::ConnectorType::UNKNOWN;
	}
}

EntityPropertiesBase* PythonHeaderInterpreter::createPropertyEntity(const ot::JsonDocument& jsonEntry, std::string& returnMessage)
{
	std::string typeString = ot::json::getString(jsonEntry, m_defType.c_str());
	
	if (typeString == m_propertyDefTypeBoolean)
	{
		auto entityBase = new EntityPropertiesBoolean();
		if (jsonEntry.HasMember(m_propertyDefDefault.c_str()))
		{
			auto& jPropertyDefaultVal =	jsonEntry[m_propertyDefDefault.c_str()];
			const bool typeIsCorrect = jPropertyDefaultVal.IsBool();
			if (typeIsCorrect)
			{
				const bool defaultValue = jPropertyDefaultVal.GetBool();
				entityBase->setValue(defaultValue);
			}
			else
			{
				returnMessage = "Default value needs to be a boolean.";
				delete entityBase;
				entityBase = nullptr;
			}
		}
		return entityBase;
	}
	else if (typeString == m_propertyDefTypeDouble)
	{
		auto entityBase =new EntityPropertiesDouble();
		if (jsonEntry.HasMember(m_propertyDefDefault.c_str()))
		{
			auto& jPropertyDefaultVal = jsonEntry[m_propertyDefDefault.c_str()];
			const bool typeIsCorrect = jPropertyDefaultVal.IsDouble();
			if (typeIsCorrect)
			{
				const double defaultValue = jPropertyDefaultVal.GetDouble();
				entityBase->setValue(defaultValue);
			}
			else
			{
				returnMessage = "Default value needs to be a double.";
				delete entityBase;
				entityBase = nullptr;
			}
		}
		return entityBase;
	}
	else if (typeString == m_propertyDefTypeInteger)
	{
		auto entityBase = new EntityPropertiesInteger();
		if (jsonEntry.HasMember(m_propertyDefDefault.c_str()))
		{
			auto& jPropertyDefaultVal = jsonEntry[m_propertyDefDefault.c_str()];
			const bool typeIsCorrect = jPropertyDefaultVal.IsInt();
			if (typeIsCorrect)
			{
				const int defaultValue = jPropertyDefaultVal.GetInt();
				entityBase->setValue(defaultValue);
			}
			else
			{
				returnMessage = "Default value needs to be an integer.";
				delete entityBase;
				entityBase = nullptr;
			}
		}
		return entityBase;
		
	}
	else if (typeString == m_propertyDefTypeString)
	{
		auto entityBase = new EntityPropertiesString();
		if (jsonEntry.HasMember(m_propertyDefDefault.c_str()))
		{
			auto& jPropertyDefaultVal = jsonEntry[m_propertyDefDefault.c_str()];
			const bool typeIsCorrect = jPropertyDefaultVal.IsString();
			if (typeIsCorrect)
			{
				const std::string defaultValue = jPropertyDefaultVal.GetString();
				entityBase->setValue(defaultValue);
			}
			else
			{
				returnMessage = "Default value needs to be a string.";
				delete entityBase;
				entityBase = nullptr;
			}
		}
		return entityBase;
	}
	else if (typeString == m_propertyDefTypeSelection)
	{
		auto entityBase = new EntityPropertiesSelection();
		if (jsonEntry.HasMember(m_propertyDefDefault.c_str()))
		{
			auto& jPropertyDefaultVal = jsonEntry[m_propertyDefDefault.c_str()];
			const bool typeIsCorrect = jPropertyDefaultVal.IsString();
			if (typeIsCorrect)
			{
				const std::string defaultValue = jPropertyDefaultVal.GetString();
				entityBase->setValue(defaultValue);
			}
			else
			{
				returnMessage = "Default value needs to be a string list.";
				delete entityBase;
				entityBase = nullptr;
				return entityBase;
			}
		}
		auto& jPropertyDefOptions = jsonEntry[m_propertyDefOptions.c_str()];
		
		std::list<std::string> options = ot::json::getStringList(jsonEntry, m_propertyDefOptions.c_str());
		entityBase->resetOptions(options);
		return entityBase;
	}
	else
	{
		returnMessage = "Property type not supported or misspelled.";
		return nullptr;
	}
}
