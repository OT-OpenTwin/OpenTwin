#pragma once
#include "ActionHandler.h"
#include "OpenTwinCore/ReturnMessage.h"
#include "OpenTwinCore/Variable.h"

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
			returnMessage = ot::ReturnMessage(OT_ACTION_RETURN_VALUE_FAILED, "Access denied.");
		}
		else
		{
			std::string action = ot::rJSON::getString(doc, OT_ACTION_PARAM_MODEL_ActionName);
			if (_handlingFunctions.find(action) != _handlingFunctions.end())
			{
				auto& checkParameter = _checkParameterFunctions[action];
				auto checkResult = checkParameter(doc);
				if (checkResult.getStatus() == OT_ACTION_RETURN_VALUE_OK)
				{
					auto& handlingFunction = _handlingFunctions[action];
					returnMessage = handlingFunction(doc);
				}
				else
				{
					returnMessage = checkResult;
				}
			}
			else
			{
				assert(0); // Action not supported
				returnMessage = ot::ReturnMessage(OT_ACTION_RETURN_VALUE_FAILED, "Action " + action + " not supported.");
			}
		}
	}
	else
	{
		returnMessage = ot::ReturnMessage(ot::ReturnStatus::Failed(), "Message requires a senderURL which is missing.");
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
		std::list<std::optional<std::list<ot::variable_t>>> allParameter;

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
				std::list<ot::variable_t> scriptParameter;
				for (uint32_t i = 0; i < parameterArray.Size(); i++)
				{
					auto paramerter = converterJ2V(parameterArray[i]);
					scriptParameter.emplace_back(paramerter);
				}
				allParameter.emplace_back(scriptParameter);
			}
		}

		//Execute
		std::list<ot::variable_t> result = _pythonAPI.Execute(scripts, allParameter);

		//Wrapp result in json string
		OT_rJSON_createDOC(returnValues);
		OT_rJSON_createValueArray(rJsonResult);
		ot::VariableToJSONConverter converterV2J;

		for (ot::variable_t& var : result)
		{
			rJsonResult.PushBack(converterV2J(var), returnValues.GetAllocator());
		}

		return ot::ReturnMessage(OT_ACTION_RETURN_VALUE_OK, ot::rJSON::toJSON(returnValues));
	}
	catch (std::exception& e)
	{
		return ot::ReturnMessage(OT_ACTION_RETURN_VALUE_FAILED, e.what());
	}

}

ot::ReturnMessage ActionHandler::CheckParameterExecute(OT_rJSON_doc& doc)
{
	if (!ot::rJSON::memberExists(doc, OT_ACTION_CMD_PYTHON_Scripts) || !ot::rJSON::memberExists(doc, OT_ACTION_CMD_PYTHON_Parameter))
	{
		return ot::ReturnMessage(OT_ACTION_RETURN_VALUE_FAILED, "Either the scripts or parameter are not contained in the message");
	}
	else
	{
		return _noParameterCheck(doc);
	}
}
