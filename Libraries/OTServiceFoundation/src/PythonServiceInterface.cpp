#include "OTServiceFoundation/PythonServiceInterface.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/VariableToJSONConverter.h"

ot::PythonServiceInterface::PythonServiceInterface(const std::string& _pythonExecutionServiceURL)
	: m_pythonExecutionServiceURL(_pythonExecutionServiceURL)
{}

void ot::PythonServiceInterface::addScriptWithParameter(const std::string& _scriptName, const scriptParameter& _scriptParameter)
{
	m_scriptNamesWithParameter.push_back(std::make_tuple(_scriptName, _scriptParameter));
}

void ot::PythonServiceInterface::addPortData(const std::string& _portName, const ot::JsonValue* _data, const ot::JsonValue* _metadata)
{
	assert(m_portDataByPortName.find(_portName) == m_portDataByPortName.end());
	m_portDataByPortName.insert(std::pair<std::string, std::pair<const ot::JsonValue*, const ot::JsonValue*>>(_portName, std::pair(_data,_metadata)));
}

ot::ReturnMessage ot::PythonServiceInterface::sendExecutionOrder()
{
	if (m_scriptNamesWithParameter.size() == 0)
	{
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "PythonServiceInterface got nothing to execute.");
	}

	std::string response;
	OT_LOG_D("Sending python execution request");
	ot::msg::send("", m_pythonExecutionServiceURL, ot::MessageType::EXECUTE, this->assembleMessage().toJson(), response,0);
	ot::ReturnMessage message = ot::ReturnMessage::fromJson(response);
	return message;
}

ot::ReturnMessage ot::PythonServiceInterface::sendSingleExecutionCommand(const std::string& command)
{
	std::string response;
	OT_LOG_D("Sending python execution request");
	JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_PYTHON_EXECUTE_Command, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_CMD_PYTHON_Command, ot::JsonString(command, doc.GetAllocator()), doc.GetAllocator());
	ot::msg::send("", m_pythonExecutionServiceURL, ot::MessageType::EXECUTE, doc.toJson(), response, 0);
	return ot::ReturnMessage::fromJson(response);
}

ot::JsonDocument ot::PythonServiceInterface::assembleMessage()
{
	JsonDocument doc;
	JsonArray allparameter;
	JsonArray scripts;
	ot::VariableToJSONConverter converter;

	for (auto& scriptWithParameter : m_scriptNamesWithParameter)
	{
		scripts.PushBack(JsonString(std::get<0>(scriptWithParameter).c_str(), doc.GetAllocator()), doc.GetAllocator());
		
		scriptParameter& currentParameterSet = std::get<1>(scriptWithParameter);
		if (currentParameterSet.has_value())
		{
			JsonArray parameter;
			for (auto& singleParameter : currentParameterSet.value())
			{
				parameter.PushBack(converter(singleParameter, doc.GetAllocator()), doc.GetAllocator());
			}
			allparameter.PushBack(parameter, doc.GetAllocator());
		}
		else
		{
			allparameter.PushBack(JsonNullValue(), doc.GetAllocator());
		}
	}

	m_scriptNamesWithParameter.clear();

	if (m_portDataByPortName.size() > 0)
	{
		ot::JsonArray portDataEntries;
		
		for (auto& portDataByPortName : m_portDataByPortName)
		{
			ot::JsonObject portDataEntry;
			ot::JsonString portName(portDataByPortName.first.c_str(), doc.GetAllocator());
			portDataEntry.AddMember("Name", portName, doc.GetAllocator());
			
			const ot::JsonValue* portData = portDataByPortName.second.first;
			assert(portData != nullptr);
			ot::JsonValue portDataCopy;
			portDataCopy.CopyFrom(*portData, doc.GetAllocator());
			portDataEntry.AddMember("Data", portDataCopy, doc.GetAllocator());


			const ot::JsonValue* portMetaData = portDataByPortName.second.second;
			assert(portMetaData != nullptr);
			ot::JsonValue portMetaDataCopy;
			portMetaDataCopy.CopyFrom(*portMetaData, doc.GetAllocator());
			portDataEntry.AddMember("Meta", portMetaDataCopy, doc.GetAllocator());

			portDataEntries.PushBack(portDataEntry, doc.GetAllocator());
		}
		doc.AddMember(OT_ACTION_CMD_PYTHON_Portdata, portDataEntries, doc.GetAllocator());
		
		m_portDataByPortName.clear();
	}

	doc.AddMember(OT_ACTION_CMD_PYTHON_Parameter, allparameter, doc.GetAllocator());
	doc.AddMember(OT_ACTION_CMD_PYTHON_Scripts, scripts, doc.GetAllocator());
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_PYTHON_EXECUTE_Scripts, doc.GetAllocator()), doc.GetAllocator());
	
	return doc;
}