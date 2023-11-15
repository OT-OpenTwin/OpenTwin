#include "OpenTwinFoundation/PythonServiceInterface.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/Msg.h"
#include "OpenTwinCommunication/CommunicationTypes.h"
#include "OpenTwinCore/JSONToVariableConverter.h"
#include "OpenTwinCore/VariableToJSONConverter.h"


ot::PythonServiceInterface::PythonServiceInterface(const std::string& pythonExecutionServiceURL)
	: _pythonExecutionServiceURL(pythonExecutionServiceURL)
{}

void ot::PythonServiceInterface::AddScriptWithParameter(const std::string& scriptName, const scriptParameter& scriptParameter)
{
	_scriptNamesWithParameter.push_back(std::make_tuple(scriptName, scriptParameter));
}

void ot::PythonServiceInterface::AddPortData(const std::string& portName, const std::list<ot::Variable>& data)
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

	OT_rJSON_doc jsonMessage = AssembleMessage();

	std::string response;
	std::string message = ot::rJSON::toJSON(jsonMessage);
	ot::msg::send("", _pythonExecutionServiceURL, ot::MessageType::EXECUTE, message, response);
	return ot::ReturnMessage::fromJson(response);
}

OT_rJSON_doc ot::PythonServiceInterface::AssembleMessage()
{
	OT_rJSON_createDOC(doc);	
	OT_rJSON_createValueArray(allparameter);
	OT_rJSON_createValueArray(scripts);
	ot::VariableToJSONConverter converter;

	for (auto& scriptWithParameter : _scriptNamesWithParameter)
	{
		OT_rJSON_val scripName;
		scripName.SetString(std::get<0>(scriptWithParameter).c_str(), doc.GetAllocator());
		scripts.PushBack(scripName,doc.GetAllocator());
		
		scriptParameter& currentParameterSet = std::get<1>(scriptWithParameter);
		if (currentParameterSet.has_value())
		{
			OT_rJSON_createValueArray(parameter);
			for (auto& singleParameter : currentParameterSet.value())
			{
				parameter.PushBack(converter(singleParameter,doc),doc.GetAllocator());
			}
			allparameter.PushBack(parameter, doc.GetAllocator());
		}
		else
		{
			OT_rJSON_val nullValue;
			allparameter.PushBack(nullValue, doc.GetAllocator());
		}
	}
	ot::rJSON::add(doc, OT_ACTION_CMD_PYTHON_Parameter, allparameter);
	ot::rJSON::add(doc, OT_ACTION_CMD_PYTHON_Scripts, scripts);
	
	if (_portDataByPortName.size() > 0)
	{
		OT_rJSON_createValueArray(portDataEntries);
		OT_rJSON_createValueArray(portDataNames);
		for (auto& portDataByPortName : _portDataByPortName)
		{
			OT_rJSON_val portName;
			portName.SetString(portDataByPortName.first.c_str(), doc.GetAllocator());
			portDataNames.PushBack(portName, doc.GetAllocator());

			std::list<ot::Variable>& portData = portDataByPortName.second;
			OT_rJSON_createValueArray(portDataJSON);
			for (ot::Variable& value : portData)
			{
				portDataJSON.PushBack(converter(value, doc), doc.GetAllocator());
			}
			portDataEntries.PushBack(portDataJSON, doc.GetAllocator());
		}
		ot::rJSON::add(doc, OT_ACTION_CMD_PYTHON_Portdata_Data, portDataEntries);
		ot::rJSON::add(doc, OT_ACTION_CMD_PYTHON_Portdata_Names, portDataNames);
	}


	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_ExecuteAction);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ActionName, OT_ACTION_CMD_PYTHON_EXECUTE);
	
	return doc;
}
