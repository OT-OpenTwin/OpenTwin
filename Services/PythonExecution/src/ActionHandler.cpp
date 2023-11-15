#pragma once
#include "ActionHandler.h"
#include "PortDataBuffer.h"
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
		std::string temp = ot::rJSON::toJSON(doc);

		PortDataBuffer::INSTANCE().clearPortData();

		//Extract script entity names from json doc
		std::list<std::string> scripts = ot::rJSON::getStringList(doc, OT_ACTION_CMD_PYTHON_Scripts);
		
		//Extract parameter array from json doc
		auto parameterArrayArray = doc[OT_ACTION_CMD_PYTHON_Parameter].GetArray();
		std::list<std::list<ot::Variable>> allParameter;

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
				
				std::list<ot::Variable> scriptParameter = converterJ2V(parameterArray);
				allParameter.emplace_back(scriptParameter);
			}
		}

		//Extract port data if existing
		if (doc.HasMember(OT_ACTION_CMD_PYTHON_Portdata_Names))
		{
			std::list<std::string> portDataNames = ot::rJSON::getStringList(doc,OT_ACTION_CMD_PYTHON_Portdata_Names);
			auto portData = doc[OT_ACTION_CMD_PYTHON_Portdata_Data].GetArray();
			auto portName = portDataNames.begin();
			for (uint32_t i = 0; i < portData.Size(); i++)
			{
				auto portDataEntry = portData[i].GetArray();
				PythonObjectBuilder builder;
				CPythonObjectNew pPortData = builder.setVariableList(portDataEntry);
				PortDataBuffer::INSTANCE().addPortData(*portName,std::move(pPortData));
				portName++;
			}
		}

		//Execute
		std::list<ot::Variable> result = _pythonAPI.Execute(scripts, allParameter);
		if (result.size() != 0)
		{
			ot::VariableToJSONConverter converterV2J;
			OT_rJSON_createDOC(returnMessageContent);
			OT_rJSON_val returnValueArray = converterV2J(result, returnMessageContent);
			ot::rJSON::add(returnMessageContent, OT_ACTION_CMD_PYTHON_EXECUTE_RESULTS, returnValueArray);
			const std::string whatMessage = ot::rJSON::toJSON(returnMessageContent);
			return ot::ReturnMessage(ot::ReturnMessage::Ok, whatMessage);
		}
		else
		{
			return ot::ReturnMessage(ot::ReturnMessage::Ok);
		}
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
