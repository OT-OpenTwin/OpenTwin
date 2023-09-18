#pragma once
#include "ActionHandler.h"
#include "OpenTwinCore/ReturnMessage.h"
#include "OpenTwinCore/Variable.h"
#include "OpenTwinCore/VariableToJSONConverter.h"
#include "OpenTwinCore/JSONToVariableConverter.h"

ActionHandler::ActionHandler(const std::string& urlMasterService)
	:_urlMasterService(urlMasterService)
{
	_checkParameterFunctions[OT_ACTION_CMD_ServiceShutdown] =_noParameterCheck;
	_handlingFunctions[OT_ACTION_CMD_ServiceShutdown] = std::bind(&ActionHandler::ShutdownProcess, this, std::placeholders::_1);
	
	_checkParameterFunctions[OT_ACTION_CMD_PYTHON_EXECUTE] = std::bind(&ActionHandler::CheckParameterExecute, this, std::placeholders::_1);
	_handlingFunctions[OT_ACTION_CMD_PYTHON_EXECUTE] = std::bind(&ActionHandler::Execute, this, std::placeholders::_1);
}

const char* ActionHandler::Handle(const char* json, const char* senderIP)
{
	std::lock_guard<std::mutex> lock(_mutex);
	std::string returnMessage;
	OT_rJSON_doc doc = ot::rJSON::fromJSON(json);
	if (ot::rJSON::memberExists(doc, OT_ACTION_PARAM_SENDER_URL))
	{
		std::string senderURL = ot::rJSON::getString(doc, OT_ACTION_PARAM_SENDER_URL);
		
		if (senderURL != _urlMasterService)
		{
			ot::ReturnMessage rMsg(ot::ReturnMessage::Failed, "Access denied.");
			returnMessage = rMsg.toJson();
		}
		else
		{
			std::string action = ot::rJSON::getString(doc, OT_ACTION_PARAM_MODEL_ActionName);
			if (_handlingFunctions.find(action) != _handlingFunctions.end())
			{
				auto& checkParameter = _checkParameterFunctions[action];
				auto checkResult = checkParameter(doc);
				if (checkResult.getStatus() == ot::ReturnMessage::Ok)
				{
					auto& handlingFunction = _handlingFunctions[action];
					returnMessage = handlingFunction(doc).toJson();
				}
				else
				{
					returnMessage = checkResult.toJson();
				}
			}
			else
			{
				assert(0); // Action not supported
				ot::ReturnMessage rMsg(ot::ReturnMessage::Failed, "Action \"" + action + "\" not supported.");
				returnMessage = rMsg.toJson();
			}
		}
	}
	else
	{
		ot::ReturnMessage rMsg(ot::ReturnMessage::Failed, "Message requires a senderURL which is missing.");
		returnMessage = rMsg.toJson();
	}
	char* returnValue = new char[returnMessage.size() + 1];
	std::strcpy(returnValue, returnMessage.c_str());

    return returnValue;
}

ot::ReturnMessage ActionHandler::ShutdownProcess(OT_rJSON_doc& doc)
{
	exit(1);
}

ot::ReturnMessage ActionHandler::Execute(OT_rJSON_doc& doc)
{
	try
	{
		//Extract script entity names from json doc
		auto scriptArray = doc[OT_ACTION_CMD_PYTHON_Scripts].GetArray();
		std::list<std::string> scripts;

		for (auto& element : scriptArray)
		{
			scripts.emplace_back(element.GetString());
		}

		//Extract parameter array from json doc
		auto parameterArrayArray = doc[OT_ACTION_CMD_PYTHON_Parameter].GetArray();
		std::list<std::optional<std::list<ot::Variable>>> allParameter;

		ot::JSONToVariableConverter converterJ2V;
		for (uint32_t i = 0; i < parameterArrayArray.Size(); i++)
		{
			if (parameterArrayArray[i].IsNull())
			{
				allParameter.push_back({});
			}
			else
			{
				auto parameterArray = parameterArrayArray[i].GetArray();
				//Todo: support of lists/maps as parameter
				std::list<ot::Variable> scriptParameter;
				for (uint32_t i = 0; i < parameterArray.Size(); i++)
				{
					auto paramerter = converterJ2V(parameterArray[i]);
					scriptParameter.emplace_back(paramerter);
				}
				allParameter.emplace_back(scriptParameter);
			}
		}

		//Execute
		std::list<ot::Variable> result = _pythonAPI.Execute(scripts, allParameter);

		//Wrapp result in json string
		OT_rJSON_createDOC(returnValues);
		OT_rJSON_createValueArray(rJsonResult);
		ot::VariableToJSONConverter converterV2J;

		for (ot::Variable& var : result)
		{
			rJsonResult.PushBack(converterV2J(var,returnValues), returnValues.GetAllocator());
		}

		return ot::ReturnMessage(ot::ReturnMessage::Ok, ot::rJSON::toJSON(returnValues));
	}
	catch (std::exception& e)
	{
		return ot::ReturnMessage(ot::ReturnMessage::Failed, e.what());
	}

}

ot::ReturnMessage ActionHandler::CheckParameterExecute(OT_rJSON_doc& doc)
{
	if (!ot::rJSON::memberExists(doc, OT_ACTION_CMD_PYTHON_Scripts) || !ot::rJSON::memberExists(doc, OT_ACTION_CMD_PYTHON_Parameter))
	{
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Either the scripts or parameter are not contained in the message");
	}
	else
	{
		return _noParameterCheck(doc);
	}
}
