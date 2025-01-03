#include "OTServiceFoundation/PythonServiceInterface.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/VariableToJSONConverter.h"

ot::PythonServiceInterface::PythonServiceInterface(const std::string& pythonExecutionServiceURL)
	: _pythonExecutionServiceURL(pythonExecutionServiceURL)
{}

void ot::PythonServiceInterface::AddScriptWithParameter(const std::string& scriptName, const scriptParameter& scriptParameter)
{
	_scriptNamesWithParameter.push_back(std::make_tuple(scriptName, scriptParameter));
}

void ot::PythonServiceInterface::AddPortData(const std::string& portName, const ot::GenericDataStructList& data)
{
	assert(_portDataByPortName.find(portName) == _portDataByPortName.end());
	_portDataByPortName[portName] = data;
}

ot::ReturnMessage ot::PythonServiceInterface::SendExecutionOrder()
{
	if (_scriptNamesWithParameter.size() == 0)
	{
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "PythonServiceInterface got nothing to execute.");
	}

	std::string response;
	OT_LOG_D("Sending python execution request");
	ot::msg::send("", _pythonExecutionServiceURL, ot::MessageType::EXECUTE, this->AssembleMessage().toJson(), response,0);
	ot::ReturnMessage message = ot::ReturnMessage::fromJson(response);
	return message;
}

ot::ReturnMessage ot::PythonServiceInterface::SendSingleExecutionCommand(const std::string& command)
{
	std::string response;
	OT_LOG_D("Sending python execution request");
	JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_ExecuteAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, JsonString(OT_ACTION_CMD_PYTHON_EXECUTE_Command, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_CMD_PYTHON_Command, ot::JsonString(command, doc.GetAllocator()), doc.GetAllocator());
	ot::msg::send("", _pythonExecutionServiceURL, ot::MessageType::EXECUTE, doc.toJson(), response, 0);
	return ot::ReturnMessage::fromJson(response);
}

ot::JsonDocument ot::PythonServiceInterface::AssembleMessage()
{
	JsonDocument doc;
	JsonArray allparameter;
	JsonArray scripts;
	ot::VariableToJSONConverter converter;

	for (auto& scriptWithParameter : _scriptNamesWithParameter)
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

	_scriptNamesWithParameter.clear();

	if (_portDataByPortName.size() > 0)
	{
		ot::JsonArray portDataEntries;
		ot::JsonArray portDataNames;
		for (auto& portDataByPortName : _portDataByPortName)
		{
			ot::JsonString portName(portDataByPortName.first.c_str(), doc.GetAllocator());
			portDataNames.PushBack(portName, doc.GetAllocator());

			
			ot::JsonArray portDataJSON;
			const auto& genericPortDataList = portDataByPortName.second;
			for (ot::GenericDataStruct* genericPortData : genericPortDataList)
			{
				ot::JsonObject entry;
				genericPortData->addToJsonObject(entry, doc.GetAllocator());
				portDataJSON.PushBack(entry, doc.GetAllocator());
			}
			portDataEntries.PushBack(portDataJSON, doc.GetAllocator());
		}
		doc.AddMember(OT_ACTION_CMD_PYTHON_Portdata_Data, portDataEntries, doc.GetAllocator());
		doc.AddMember(OT_ACTION_CMD_PYTHON_Portdata_Names, portDataNames,doc.GetAllocator());
		_portDataByPortName.clear();
	}

	doc.AddMember(OT_ACTION_CMD_PYTHON_Parameter, allparameter, doc.GetAllocator());
	doc.AddMember(OT_ACTION_CMD_PYTHON_Scripts, scripts, doc.GetAllocator());
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_ExecuteAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, JsonString(OT_ACTION_CMD_PYTHON_EXECUTE_Scripts, doc.GetAllocator()), doc.GetAllocator());
	
	return doc;
}