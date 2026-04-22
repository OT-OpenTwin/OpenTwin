// @otlicense
#pragma once
#include "OTCore/CoreAPIExport.h"
#include <string>
#include <map>
#include "OTCore/PythonHeader/PythonEventTypes.h"
#include "OTCore/JSON/JSON.h"
//! @brief Events as string: "Item Deleted", "Task Started", "Task Finished", "Table Filter Changed", "Property Updated"
class PythonHeaderEventBuilder
{
public:
	
	bool interpret(const std::string& _scriptContent);

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