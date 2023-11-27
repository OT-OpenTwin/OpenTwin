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

	auto& fileContent = pythonScript->getData()->getData();
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
			const bool lineWasInterpreted = ExtractOTHeader(scriptLine);
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

bool PythonHeaderInterpreter::ExtractOTHeader(const std::string& scriptLine)
{
	size_t endOfEntryType = scriptLine.find_first_of(":") - 1;
	const int offset = 2;
	std::string entryType = scriptLine.substr(offset, endOfEntryType - offset);
	auto noSpaceEnd = std::remove_if(entryType.begin(), entryType.end(), isspace);
	entryType.erase(noSpaceEnd, entryType.end());
	if(std::find(_entryTypeNamesProperty.begin(), _entryTypeNamesProperty.end(), entryType) != _entryTypeNamesProperty.end())
	{
		std::string entryDefinition = scriptLine.substr(endOfEntryType + 1, scriptLine.size());
		ot::JsonDocument* doc = new ot::JsonDocument;
		doc->fromJson(entryDefinition);
		_allPropertiesAsJSON.push_back(doc);
		return true;
	}
	else if (std::find(_entryTypeNamesConnector.begin(), _entryTypeNamesConnector.end(), entryType) != _entryTypeNamesConnector.end())
	{
		const size_t startJSON = scriptLine.find_first_of("{");
		const size_t endOfJSON = scriptLine.find_last_of("}");
		std::string entryDefinition = scriptLine.substr(startJSON, endOfJSON-startJSON+1);
		ot::JsonDocument* doc = new ot::JsonDocument;
		doc->fromJson(entryDefinition);
		_allConnectorsAsJSON.push_back(doc);
		return true;
	}
	else
	{
		return false;
	}
}

bool PythonHeaderInterpreter::CreateObjectsFromJSON()
{
	bool allObjectsCreated = true;
	for (auto& connectorJSON : _allConnectorsAsJSON)
	{
		ot::ConnectorType connectorType = getConnectorType(connectorJSON->GetConstObject());
		std::string name(""), title("");
		for (const std::string& nameVariant : _connectorDefName)
		{
			if (connectorJSON->HasMember(nameVariant.c_str()))
			{
				name = ot::json::getString(*connectorJSON, nameVariant.c_str());
				break;
			}
		}

		for (const std::string& titleVariant : _connectorDefTitle)
		{
			if (connectorJSON->HasMember(titleVariant.c_str()))
			{
				title = ot::json::getString(*connectorJSON, titleVariant.c_str());
				break;
			}
		}

		if (name != "" && title != "" && connectorType != ot::ConnectorType::UNKNOWN)
		{
			_allConnectors.push_back({ connectorType, name, title });
		}
		else
		{
			_report += "Could not create port because ";
			if (name == "") { _report += "name was not set. "; }
			if (title == "") { _report += "name was not set. "; }
			if (connectorType == ot::ConnectorType::UNKNOWN) { _report = "conector type could not be identified."; }
			_report += "\n";
			allObjectsCreated = false;
		}
	}

	return allObjectsCreated;
}

ot::ConnectorType PythonHeaderInterpreter::getConnectorType(const ot::ConstJsonObject& jsonEntry)
{
	std::string typeString("");
	for (const std::string& typeVariant : _connectorDefType)
	{
		if (jsonEntry.HasMember(typeVariant.c_str()))
		{
			typeString = ot::json::getString(jsonEntry, typeVariant.c_str());
		}
	}
	if (typeString == "")
	{
		return ot::ConnectorType::UNKNOWN;
	}

	if (std::find(_connectorDefTypeIn.begin(), _connectorDefTypeIn.end(), typeString) != _connectorDefTypeIn.end())
	{
		return ot::ConnectorType::In;
	}
	if (std::find(_connectorDefTypeInOptional.begin(), _connectorDefTypeInOptional.end(), typeString) != _connectorDefTypeInOptional.end())
	{
		return ot::ConnectorType::InOptional;
	}
	if (std::find(_connectorDefTypeOut.begin(), _connectorDefTypeOut.end(), typeString) != _connectorDefTypeOut.end())
	{
		return ot::ConnectorType::Out;
	}
	
	return ot::ConnectorType::UNKNOWN;
}
