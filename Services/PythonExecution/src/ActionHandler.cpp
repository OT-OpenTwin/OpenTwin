#include "ActionHandler.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/Variable.h"
#include "OTCore/Logger.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/ReturnValues.h"
#include "OTCore/GenericDataStruct.h"
#include "OTCore/GenericDataStructFactory.h"
#include "PortDataBuffer.h"

#include "Application.h"
#include "DataBase.h"
#include "EntityBase.h"

ActionHandler::ActionHandler()
{
	auto arguments = std::placeholders::_1;
	_checkParameterFunctions[OT_ACTION_CMD_ServiceShutdown] = _noParameterCheck;
	_handlingFunctions[OT_ACTION_CMD_ServiceShutdown] = std::bind(&ActionHandler::ShutdownProcess, this, arguments);

	_checkParameterFunctions[OT_ACTION_CMD_PYTHON_EXECUTE_Scripts] = std::bind(&ActionHandler::CheckParameterExecuteScript, this, arguments);
	_handlingFunctions[OT_ACTION_CMD_PYTHON_EXECUTE_Scripts] = std::bind(&ActionHandler::ExecuteScript, this, arguments);

	_checkParameterFunctions[OT_ACTION_CMD_PYTHON_EXECUTE_Command] = std::bind(&ActionHandler::CheckParameterExecuteCommand, this, arguments);
	_handlingFunctions[OT_ACTION_CMD_PYTHON_EXECUTE_Command] = std::bind(&ActionHandler::ExecuteCommand, this, arguments);

	_checkParameterFunctions[OT_ACTION_CMD_Init] = _noParameterCheck;
	_handlingFunctions[OT_ACTION_CMD_Init] = std::bind(&ActionHandler::Initialise, this, arguments);

}

ot::ReturnMessage ActionHandler::Handle(ot::JsonDocument& doc)
{
	std::lock_guard<std::mutex> lock(_mutex);
	
	ot::ReturnMessage returnMessage ;

	std::string action = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_ActionName);
	if (_handlingFunctions.find(action) != _handlingFunctions.end())
	{
		auto& checkParameter = _checkParameterFunctions[action];
		auto checkResult = checkParameter(doc);
		if (checkResult.getStatus() == ot::ReturnMessage::Ok)
		{
			OT_LOG_D("Executing action " + action);
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
		returnMessage = ot::ReturnMessage(ot::ReturnMessage::Failed, "Action \"" + action + "\" not supported.");
	}

	return returnMessage;
}

ot::ReturnMessage ActionHandler::Initialise(ot::JsonDocument& doc)
{
	const std::string serviceName = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_NAME);
	ot::ReturnMessage returnMessage = ot::ReturnMessage(ot::ReturnMessage::Ok, "Initialisation succeeded");
	if (serviceName == OT_INFO_SERVICE_TYPE_DataBase)
	{
		OT_LOG_D("Initialise Database Connection");
		const std::string url = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		const std::string userName = ot::json::getString(doc, OT_PARAM_AUTH_USERNAME);
		const std::string psw = ot::json::getString(doc, OT_PARAM_AUTH_PASSWORD);
		const std::string collectionName = ot::json::getString(doc, OT_ACTION_PARAM_COLLECTION_NAME);
		const std::string siteID = ot::json::getString(doc, OT_ACTION_PARAM_SITE_ID);

		DataBase::GetDataBase()->setProjectName(collectionName);
		DataBase::GetDataBase()->setUserCredentials(userName, psw);
		DataBase::GetDataBase()->InitializeConnection(url, siteID);
	}
	else if (serviceName == OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE)
	{
		OT_LOG_D("Initialise UID Generator");
		const int sessionID = ot::json::getInt(doc, OT_ACTION_PARAM_SESSION_ID);
		const int serviceID = ot::json::getInt(doc, OT_ACTION_PARAM_SERVICE_ID);
		EntityBase::setUidGenerator(new DataStorageAPI::UniqueUIDGenerator(sessionID, serviceID));
	}
	else if (serviceName == OT_INFO_SERVICE_TYPE_MODEL)
	{
		OT_LOG_D("Connecting with modelService");
		const std::string url = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		Application::instance()->setModelServiceURL(url);

	}
	else if (serviceName == OT_INFO_SERVICE_TYPE_UI)
	{
		OT_LOG_D("Connecting with uiService");
		const std::string url = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		Application::instance()->setUIServiceURL(url);
	}
	else
	{
		returnMessage =ot::ReturnMessage(ot::ReturnMessage::Failed, "Not supported initialisation order.");
	}
	return returnMessage;
}

ot::ReturnMessage ActionHandler::ShutdownProcess(ot::JsonDocument& doc)
{
	OT_LOG_D("Requested Shuttdown. Shutting down process now.");
	exit(0);
}

ot::ReturnMessage ActionHandler::ExecuteScript(ot::JsonDocument& doc)
{
	try
	{

		std::string temp = doc.toJson();

		PortDataBuffer::INSTANCE().clearPortData();

		//Extract script entity names from json doc
		std::list<std::string> scripts = ot::json::getStringList(doc, OT_ACTION_CMD_PYTHON_Scripts);
		OT_LOG_D("Number of scripts being executed: " + std::to_string(scripts.size()));

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
				auto parameterArray = ot::json::getArray(parameterArrayArray, i);
				//Todo: support of lists/maps as parameter

				std::list<ot::Variable> scriptParameter = converterJ2V(parameterArray);
				allParameter.emplace_back(scriptParameter);
			}
		}

		//Extract port data if existing
		if (doc.HasMember(OT_ACTION_CMD_PYTHON_Portdata_Names))
		{
			std::list<std::string> portDataNames = ot::json::getStringList(doc, OT_ACTION_CMD_PYTHON_Portdata_Names);
			OT_LOG_D("Number of port datasets: " + std::to_string(portDataNames.size()));
			auto portData = doc[OT_ACTION_CMD_PYTHON_Portdata_Data].GetArray();
			auto portName = portDataNames.begin();
			for (uint32_t i = 0; i < portData.Size(); i++)
			{
				auto portDataEntries = ot::json::getArray(portData, i);
				ot::GenericDataStructList values;
				for (const auto& jGenericDataStruct : portDataEntries)
				{
					ot::GenericDataStruct* genericDataStruct = ot::GenericDataStructFactory::Create(jGenericDataStruct.GetObject());
					values.push_back(genericDataStruct);
				}

				PortDataBuffer::INSTANCE().addNewPortData(*portName, values);
				portName++;
			}
		}

		//Execute
		ot::ReturnValues result = _pythonAPI.Execute(scripts, allParameter);
		PortDataBuffer::INSTANCE().AddModifiedPortData(result);

		if (result.getNumberOfEntries() != 0)
		{
			return ot::ReturnMessage(std::move(result));;
		}
		else
		{
			return ot::ReturnMessage(ot::ReturnMessage::Ok);
		}
	}
	catch (std::exception& e)
	{
		OT_LOG_D("Script execution failed due to exception: " + std::string(e.what()));
		return ot::ReturnMessage(ot::ReturnMessage::ReturnMessageStatus::Failed, e.what());
	}

}

ot::ReturnMessage ActionHandler::ExecuteCommand(ot::JsonDocument& doc)
{
	std::string executionCommand = ot::json::getString(doc, OT_ACTION_CMD_PYTHON_Command);
	try
	{
		ot::ReturnValues result = _pythonAPI.Execute(executionCommand);
		if (result.getNumberOfEntries() != 0)
		{
			return ot::ReturnMessage(result);
		}
		else
		{
			return ot::ReturnMessage(ot::ReturnMessage::Ok);
		}
	}
	catch (std::exception& e)
	{
		OT_LOG_D("Execution of command failed due to exception: " + std::string(e.what()));
		return ot::ReturnMessage(ot::ReturnMessage::Failed, e.what());
	}
}

ot::ReturnMessage ActionHandler::CheckParameterExecuteScript(ot::JsonDocument& doc)
{
	if (!doc.HasMember(OT_ACTION_CMD_PYTHON_Scripts) || !doc.HasMember(OT_ACTION_CMD_PYTHON_Parameter))
	{
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Either the scripts or parameter are not contained in the message");
	}
	else
	{
		return ot::ReturnMessage();
	}
}

ot::ReturnMessage ActionHandler::CheckParameterExecuteCommand(ot::JsonDocument& doc)
{
	if (!doc.HasMember(OT_ACTION_CMD_PYTHON_Command))
	{
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Message does not contain a command to execute");
	}
	else
	{
		return ot::ReturnMessage();
	}
}
