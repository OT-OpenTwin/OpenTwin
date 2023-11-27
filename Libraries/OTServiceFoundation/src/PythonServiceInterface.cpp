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

ot::ReturnMessage ot::PythonServiceInterface::SendExecutionOrder()
{
	if (_scriptNamesWithParameter.size() == 0)
	{
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "PythonServiceInterface got nothing to execute.");
	}

	std::string response;
	ot::msg::send("", _pythonExecutionServiceURL, ot::MessageType::EXECUTE, this->AssembleMessage(), response);
	return ot::ReturnMessage::fromJson(response);
}

std::string ot::PythonServiceInterface::AssembleMessage()
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

	doc.AddMember(OT_ACTION_CMD_PYTHON_Parameter, allparameter, doc.GetAllocator());
	doc.AddMember(OT_ACTION_CMD_PYTHON_Scripts, scripts, doc.GetAllocator());
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_ExecuteAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, JsonString(OT_ACTION_CMD_PYTHON_EXECUTE, doc.GetAllocator()), doc.GetAllocator());
	
	return doc.toJson();
}
