#include "OTCore/PythonHeader/PythonHeaderEventBuilder.h"
#include "OTCore/PythonHeader/PythonHeaderAnalyser.h"
#include "OTCore/String.h"

bool PythonHeaderEventBuilder::interpret(const std::string& _scriptContent)
{
	PythonHeaderAnalyser analyser;
	analyser.analysePythonScript(_scriptContent);
	const std::string report = analyser.getAnalysisReport();
	const std::list<PythonHeaderAnalyser::ExtractedEntry>& entries = analyser.getEntriesOfType(PythonHeaderEntryType::Event);

	bool success = true;
	for (const PythonHeaderAnalyser::ExtractedEntry& entry : entries)
	{
		bool partialSuccess = extractEventExecution(entry.m_content);
		if (!partialSuccess)
		{
			m_report += "Detected in line: " + std::to_string(entry.m_lineNumber) + "\n";
		}
		success |= partialSuccess;
	}
	return success;
}

std::optional<std::string> PythonHeaderEventBuilder::getFunctionName(PythonEventType _type)
{
	auto executionByType = m_eventExecutionByEventType.find(_type);
	if (executionByType == m_eventExecutionByEventType.end())
	{
		return std::nullopt;
	}
	else
	{
		return executionByType->second;
	}
	
}

bool PythonHeaderEventBuilder::extractEventExecution(const ot::JsonDocument& _content)
{
	bool complete = true;
	if (!ot::json::exists(_content, m_keyEventType))
	{
		m_report += "Entry missing: " + m_keyEventType+ "\n";
		complete = false;
	}
	if (!ot::json::exists(_content, m_keyFunction))
	{
		m_report += "Entry missing: " + m_keyFunction+ "\n";
		complete = false;
	}
	
	if (complete)
	{
		try
		{
			const std::string functionName = ot::json::getString(_content, m_keyFunction);
			std::string type = ot::json::getString(_content, m_keyEventType);
			type = ot::String::toLower(type);
			ot::String::removeControlCharacters(type);
			ot::String::removeWhitespaces(type);
			auto typeByName = m_eventTypeByName.find(type);
			if (typeByName == m_eventTypeByName.end())
			{
				m_report += "Unsupported event type: " + type + "\n";
			}
			else
			{
				if(m_eventExecutionByEventType.find(typeByName->second) != m_eventExecutionByEventType.end())
				{
					m_report += "Duplicate event type: " + type + ". Skipping duplicates.\n";
				}
				else
				{
					m_eventExecutionByEventType[typeByName->second] = functionName;
				}
			}
		}
		catch (const std::exception& _e)
		{
			m_report += "Exception in processing event entry: " + std::string(_e.what()) + "\n";
		}
	}

	return complete;
}
