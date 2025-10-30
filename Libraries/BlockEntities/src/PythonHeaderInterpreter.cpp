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

#include "PythonHeaderInterpreter.h"

#include <sstream>
#include <algorithm>

PythonHeaderInterpreter::~PythonHeaderInterpreter() {
	for (auto doc : _allConnectorsAsJSON) {
		delete doc;
	}
	_allConnectorsAsJSON.clear();

	for (auto doc : _allPropertiesAsJSON) {
		delete doc;
	}
	_allPropertiesAsJSON.clear();

}

bool PythonHeaderInterpreter::interprete(std::shared_ptr<EntityFile> pythonScript)
{
	_allConnectors.clear();
	_allProperties.clear();

	const auto& fileContent = pythonScript->getDataEntity()->getData();
	std::string script(fileContent.begin(), fileContent.end());
	std::istringstream scriptStream(script);
	std::string scriptLine;
	std::getline(scriptStream, scriptLine);
	
	bool headerSuccessfullyInterpreted = true;
	int lineCounter = 1;
	_report = "";
	while (scriptLine[0] == '#' && scriptLine[1] == '@')
	{
		try
		{
			const bool lineWasInterpreted = ExtractOTHeader(scriptLine,lineCounter);
			if (!lineWasInterpreted)
			{
				_report += "Line " + std::to_string(lineCounter) + " had no supported entry type.\n";
			}

			headerSuccessfullyInterpreted &= lineWasInterpreted;
		}
		catch (std::exception& e)
		{
			_report += "Exception in reading line " + std::to_string(lineCounter) + ": " + e.what() + "\n";
			headerSuccessfullyInterpreted = false;
		}

		std::getline(scriptStream, scriptLine);
		lineCounter++;

	}

	if (headerSuccessfullyInterpreted)
	{
		headerSuccessfullyInterpreted &= CreateObjectsFromJSON();
	}

	return headerSuccessfullyInterpreted;
}

const std::string  PythonHeaderInterpreter::extractType(const std::string& lineContent)
{
	//Find type substring
	size_t endOfEntryType = lineContent.find_first_of(":") - 1;
	const int offset = 2;
	std::string entryType = lineContent.substr(offset, endOfEntryType - offset);
	
	//Remove spaces
	auto noSpaceEnd = std::remove_if(entryType.begin(), entryType.end(), isspace);
	entryType.erase(noSpaceEnd, entryType.end());
	
	//Turn all characters to lowercase
	for (size_t i = 0; i < entryType.size(); i++)
	{
		entryType[i] = std::tolower(entryType[i]);
	}
	return entryType;
}

bool PythonHeaderInterpreter::ExtractOTHeader(const std::string& scriptLine, const int scriptLineNumber)
{
	const std::string entryType = extractType(scriptLine);

	const size_t startJSON = scriptLine.find_first_of("{");
	const size_t endOfJSON = scriptLine.find_last_of("}");
	const std::string entryDefinition = scriptLine.substr(startJSON, endOfJSON - startJSON + 1);

	if(entryType == _entryTypeNameProperty)
	{
		ot::JsonDocument* doc = new ot::JsonDocument();
		doc->fromJson(entryDefinition);
		_allPropertiesAsJSON.push_back(doc);
		_jsonEntryToScriptLine[_allPropertiesAsJSON.back()] = scriptLineNumber;
		return true;
	}
	else if (entryType == _entryTypeNameConnector)
	{
		ot::JsonDocument* doc = new ot::JsonDocument();
		doc->fromJson(entryDefinition);
		_allConnectorsAsJSON.push_back(doc);
		_jsonEntryToScriptLine[_allConnectorsAsJSON.back()] = scriptLineNumber;
		return true;
	}
	else
	{
		return false;
	}
}

bool PythonHeaderInterpreter::CreateObjectsFromJSON()
{

	const bool allConnectorsCreated = CreateConnectorsFromJSON();
	const bool allPropertiesCreated = CreatePropertiesFromJSON();
	return allConnectorsCreated && allPropertiesCreated;
}

bool PythonHeaderInterpreter::CreateConnectorsFromJSON()
{
	bool allObjectsCreated = true;
	auto connectorJSON = _allConnectorsAsJSON.begin();
	
	for (size_t i = 0; i < _allConnectorsAsJSON.size(); i++)
	{
		
		bool allMemberFound = true;
		std::string missingMember("");
		if (!(*connectorJSON)->HasMember(_connectorDefName.c_str()))
		{
			allMemberFound = false;
			missingMember += _connectorDefName + ", ";
		}

		if (!(*connectorJSON)->HasMember(_defTitle.c_str()))
		{
			allMemberFound = false;
			missingMember += _defTitle + ", ";
		}

		ot::ConnectorType connectorType = ot::ConnectorType::UNKNOWN;
		std::string status = "";
		if (!(*connectorJSON)->HasMember(_defType.c_str()))
		{
			allMemberFound = false;
			missingMember += _defTitle + ", ";
		}
		else
		{

			connectorType = getConnectorType(**connectorJSON, status);
		}

		if (allMemberFound && connectorType != ot::ConnectorType::UNKNOWN)
		{
			const std::string title = ot::json::getString(**connectorJSON, _defTitle.c_str());
			const std::string name = ot::json::getString(**connectorJSON, _connectorDefName.c_str());
			_allConnectors.push_back({ connectorType, name, title });
		}
		else
		{
			int scriptLine = _jsonEntryToScriptLine[(*connectorJSON)];
			_report += "Could not create port in line " + std::to_string(scriptLine) + " because\n"
				"following fields are missing: " + missingMember.substr(0, missingMember.size() - 2) + "\n";

			if (connectorType == ot::ConnectorType::UNKNOWN) { _report += status + "\n"; }
			_report += "\n";
			allObjectsCreated = false;
		}
		connectorJSON++;
	}
	return allObjectsCreated;
}

bool PythonHeaderInterpreter::CreatePropertiesFromJSON()
{
	bool allObjectsCreated = true;
	auto propertyJSON = _allPropertiesAsJSON.begin();
	for (size_t i = 0; i < _allPropertiesAsJSON.size(); i++)
	{
		std::string missingEntries = "";
		bool allEntriesFound = true;

		if (!(*propertyJSON)->HasMember(_defTitle.c_str()))
		{
			allEntriesFound = false;
			missingEntries += _defTitle + ", ";
		}
		
		EntityPropertiesBase* propertyBase = nullptr;
		std::string status("");
		if (!(*propertyJSON)->HasMember(_defType.c_str()))
		{
			allEntriesFound = false;
			missingEntries += _defType + ", ";
		}
		else
		{
			std::string type = ot::json::getString(**propertyJSON,_defType.c_str());
			if (type == _propertyDefTypeSelection && !(*propertyJSON)->HasMember(_propertyDefOptions.c_str()))
			{
				allEntriesFound = false;
				missingEntries += _propertyDefOptions + ", ";
			}
			
			propertyBase = createPropertyEntity(**propertyJSON, status);
			if (propertyBase == nullptr)
			{
				allEntriesFound = false;
			}

		}
		if (allEntriesFound)
		{
			const std::string title = ot::json::getString((**propertyJSON), _defTitle.c_str());
			
			propertyBase->setGroup(_propertyGroupName);
			propertyBase->setName(title);
			_allProperties.push_back(propertyBase);
		}
		else
		{
			int scriptLine = _jsonEntryToScriptLine[(*propertyJSON)];
			_report += "Could not create property in line " +std::to_string(scriptLine) + " because\n"
				"following fields are missing: " + missingEntries.substr(0, missingEntries.size() - 2) + "\n";
			if (propertyBase == nullptr) { _report += status + "\n"; }
			_report += "\n";
			allObjectsCreated = false;
		}		

		propertyJSON++;
	}
	return allObjectsCreated;
}

ot::ConnectorType PythonHeaderInterpreter::getConnectorType(ot::JsonDocument& jsonEntry, std::string& returnMessage )
{
	std::string typeString = ot::json::getString(jsonEntry, _defType.c_str());
	
	if (_connectorDefTypeIn == typeString)
	{
		return ot::ConnectorType::In;
	}
	else if (_connectorDefTypeInOptional == typeString)
	{
		return ot::ConnectorType::InOptional;
	}
	else if (_connectorDefTypeOut == typeString)
	{
		return ot::ConnectorType::Out;
	}
	else
	{
		returnMessage = "Port type not supported or misspelled: " +typeString;
		return ot::ConnectorType::UNKNOWN;
	}
}

EntityPropertiesBase* PythonHeaderInterpreter::createPropertyEntity(ot::JsonDocument& jsonEntry, std::string& returnMessage)
{
	std::string typeString = ot::json::getString(jsonEntry, _defType.c_str());
	
	if (typeString == _propertyDefTypeBoolean)
	{
		auto entityBase = new EntityPropertiesBoolean();
		if (jsonEntry.HasMember(_propertyDefDefault.c_str()))
		{
			auto& jPropertyDefaultVal =	jsonEntry[_propertyDefDefault.c_str()];
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
	else if (typeString == _propertyDefTypeDouble)
	{
		auto entityBase =new EntityPropertiesDouble();
		if (jsonEntry.HasMember(_propertyDefDefault.c_str()))
		{
			auto& jPropertyDefaultVal = jsonEntry[_propertyDefDefault.c_str()];
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
	else if (typeString == _propertyDefTypeInteger)
	{
		auto entityBase = new EntityPropertiesInteger();
		if (jsonEntry.HasMember(_propertyDefDefault.c_str()))
		{
			auto& jPropertyDefaultVal = jsonEntry[_propertyDefDefault.c_str()];
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
	else if (typeString == _propertyDefTypeString)
	{
		auto entityBase = new EntityPropertiesString();
		if (jsonEntry.HasMember(_propertyDefDefault.c_str()))
		{
			auto& jPropertyDefaultVal = jsonEntry[_propertyDefDefault.c_str()];
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
	else if (typeString == _propertyDefTypeSelection)
	{
		auto entityBase = new EntityPropertiesSelection();
		if (jsonEntry.HasMember(_propertyDefDefault.c_str()))
		{
			auto& jPropertyDefaultVal = jsonEntry[_propertyDefDefault.c_str()];
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
		auto& jPropertyDefOptions = jsonEntry[_propertyDefOptions.c_str()];
		
		std::list<std::string> options = ot::json::getStringList(jsonEntry, _propertyDefOptions.c_str());
		entityBase->resetOptions(options);
		return entityBase;
	}
	else
	{
		returnMessage = "Property type not supported or misspelled.";
		return nullptr;
	}
}
