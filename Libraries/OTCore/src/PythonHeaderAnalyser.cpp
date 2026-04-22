#include "OTCore/PythonHeader/PythonHeaderAnalyser.h"
#include "OTCore/String.h"
PythonHeaderAnalyser::PythonHeaderAnalyser()
	:m_types{ 
		{"property", PythonHeaderEntryType::Property},{ "port", PythonHeaderEntryType::Port},{ "event" ,PythonHeaderEntryType::Event}}
{
	m_entriesByType[PythonHeaderEntryType::Unknown];
}
void PythonHeaderAnalyser::analysePythonScript(const std::string& _scriptContent)
{
	m_report = "";

	std::string script(_scriptContent.begin(), _scriptContent.end());
	std::istringstream scriptStream(script);
	std::string scriptLine;
	std::getline(scriptStream, scriptLine);

	int lineCounter = 1;

	while (scriptLine[0] == '#' && scriptLine[1] == '@')
	{
		try
		{
			PythonHeaderEntryType type = extractType(scriptLine);
			if (type == PythonHeaderEntryType::Unknown)
			{
				m_report += "Line " + std::to_string(lineCounter) + " had no supported entry type.\n";
			}
			else
			{
				ot::JsonDocument content = extractDetails(scriptLine);
				ExtractedEntry entry;
				entry.m_lineNumber = lineCounter;
				entry.m_content = std::move(content);
				m_entriesByType[type].push_back(std::move(entry));
			}
		}
		catch (std::exception& e)
		{
			m_report += "Exception in reading line " + std::to_string(lineCounter) + ": " + e.what() + "\n";
		}

		std::getline(scriptStream, scriptLine);
		lineCounter++;
	}
}

const std::list<PythonHeaderAnalyser::ExtractedEntry>& PythonHeaderAnalyser::getEntriesOfType(PythonHeaderEntryType _type) const
{
	auto entryByName = m_entriesByType.find(_type);
	if (entryByName != m_entriesByType.end())
	{
		return m_entriesByType.find(_type)->second;
	}
	else
	{
		
		return m_entriesByType.find(PythonHeaderEntryType::Unknown)->second;
	}
}

PythonHeaderEntryType PythonHeaderAnalyser::extractType(const std::string& _lineContent)
{
	//Find type substring
	size_t endOfEntryType = _lineContent.find_first_of(":") - 1;
	const int offset = 2;
	std::string entryType = _lineContent.substr(offset, endOfEntryType - offset);
	
	entryType = ot::String::toLower(entryType);
	ot::String::removeControlCharacters(entryType);
	ot::String::removeWhitespaces(entryType);

	auto type =  m_types.find(entryType);
	if(type == m_types.end())
	{
		return PythonHeaderEntryType::Unknown;
	}
	else
	{ 
		return type->second;
	}
}

ot::JsonDocument PythonHeaderAnalyser::extractDetails(const std::string& _lineContent)
{
	const size_t startJSON = _lineContent.find_first_of("{");
	const size_t endOfJSON = _lineContent.find_last_of("}");
	const std::string entryContent = _lineContent.substr(startJSON, endOfJSON - startJSON + 1);

	ot::JsonDocument doc;
	doc.fromJson(entryContent);
	ot::JsonDocument cleanDoc = cleanJSONKeys(std::move(doc));

	return cleanDoc;
}

ot::JsonDocument PythonHeaderAnalyser::cleanJSONKeys(ot::JsonDocument&& _doc)
{
	ot::JsonDocument cleanDocument;
	
	for (auto& entry : _doc.getConstObject())
	{
		std::string oldName =	entry.name.GetString();
		ot::String::removeControlCharacters(oldName);
		ot::String::removeWhitespaces(oldName);
		std::string newName = ot::String::toLower(oldName);

		ot::JsonValue newKey(newName.c_str(), cleanDocument.GetAllocator());
		auto& value = entry.value;
		ot::JsonValue newValue;
		newValue.CopyFrom(value, cleanDocument.GetAllocator());

		cleanDocument.AddMember(newKey, newValue, cleanDocument.GetAllocator());
	}

	return cleanDocument;
}
