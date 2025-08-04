#include "ActionHandler.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/Variable.h"
#include "OTCore/Logger.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/ReturnValues.h"
#include "OTCore/GenericDataStruct.h"
#include "OTCore/GenericDataStructFactory.h"
#include "DataBuffer.h"

#include "Application.h"
#include "DataBase.h"
#include "EntityBase.h"

ActionHandler& ActionHandler::instance(void) {
	static ActionHandler g_instance;
	return g_instance;
}

ot::ReturnMessage ActionHandler::handleAction(const ot::JsonDocument& doc) {
	std::lock_guard<std::mutex> lock(m_mutex);
	
	ot::ReturnMessage returnMessage;

	std::string action = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_ActionName);
	if (m_handlingFunctions.find(action) != m_handlingFunctions.end()) {
		auto& checkParameter = m_checkParameterFunctions[action];
		auto checkResult = checkParameter(doc);
		if (checkResult.getStatus() == ot::ReturnMessage::Ok) {
			OT_LOG_D("Executing action " + action);
			auto& handlingFunction = m_handlingFunctions[action];
			returnMessage = handlingFunction(doc);
		}
		else {
			OT_LOG_D("Check parameter failed");
			returnMessage = checkResult;
		}
	}
	else {
		OT_LOG_EAS("Action \"" + action + "\" not supported. Document: " + doc.toJson());
		returnMessage = ot::ReturnMessage(ot::ReturnMessage::Failed, "Action \"" + action + "\" not supported.");
	}
	
	return returnMessage;
}

ActionHandler::ActionHandler() {
	auto arguments = std::placeholders::_1;
	m_checkParameterFunctions[OT_ACTION_CMD_ServiceShutdown] = m_noParameterCheck;
	m_handlingFunctions[OT_ACTION_CMD_ServiceShutdown] = std::bind(&ActionHandler::shutdownProcess, this, arguments);

	m_checkParameterFunctions[OT_ACTION_CMD_Ping] = m_noParameterCheck;
	m_handlingFunctions[OT_ACTION_CMD_Ping] = std::bind(&ActionHandler::handlePing, this, arguments);

	m_checkParameterFunctions[OT_ACTION_CMD_PYTHON_EXECUTE_Scripts] = std::bind(&ActionHandler::checkParameterExecuteScript, this, arguments);
	m_handlingFunctions[OT_ACTION_CMD_PYTHON_EXECUTE_Scripts] = std::bind(&ActionHandler::executeScript, this, arguments);

	m_checkParameterFunctions[OT_ACTION_CMD_PYTHON_EXECUTE_Command] = std::bind(&ActionHandler::checkParameterExecuteCommand, this, arguments);
	m_handlingFunctions[OT_ACTION_CMD_PYTHON_EXECUTE_Command] = std::bind(&ActionHandler::executeCommand, this, arguments);

	m_checkParameterFunctions[OT_ACTION_CMD_Init] = m_noParameterCheck;
	m_handlingFunctions[OT_ACTION_CMD_Init] = std::bind(&ActionHandler::initialise, this, arguments);

	m_checkParameterFunctions[OT_ACTION_CMD_SetLogFlags] = m_noParameterCheck;
	m_handlingFunctions[OT_ACTION_CMD_SetLogFlags] = std::bind(&ActionHandler::setLogFlags, this, arguments);
}

ot::ReturnMessage ActionHandler::initialise(const ot::JsonDocument& doc) {
	if (doc.HasMember(OT_ACTION_PARAM_LogFlags)) {
		ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(ot::json::getArray(doc, OT_ACTION_PARAM_LogFlags)));
	}

	const std::string serviceName = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_NAME);
	ot::ReturnMessage returnMessage = ot::ReturnMessage(ot::ReturnMessage::Ok, "Initialisation succeeded");
	if (serviceName == OT_INFO_SERVICE_TYPE_DataBase) {
		OT_LOG_D("Initialise Database Connection");
		const std::string url = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		const std::string userName = ot::json::getString(doc, OT_PARAM_AUTH_USERNAME);
		const std::string psw = ot::json::getString(doc, OT_PARAM_AUTH_PASSWORD);
		const std::string collectionName = ot::json::getString(doc, OT_ACTION_PARAM_COLLECTION_NAME);
	
		DataBase::GetDataBase()->setProjectName(collectionName);
		DataBase::GetDataBase()->setUserCredentials(userName, psw);
		DataBase::GetDataBase()->InitializeConnection(url);
	}
	else if (serviceName == OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE) {
		OT_LOG_D("Initialise UID Generator");
		const int sessionCount = ot::json::getInt(doc, OT_ACTION_PARAM_SESSION_COUNT);
		const int serviceID = ot::json::getInt(doc, OT_ACTION_PARAM_SERVICE_ID);
		EntityBase::setUidGenerator(new DataStorageAPI::UniqueUIDGenerator(sessionCount, serviceID));
	}
	else if (serviceName == OT_INFO_SERVICE_TYPE_PYRIT) {
		OT_LOG_D("Initialise UID Generator");
		const int sessionCount = ot::json::getInt(doc, OT_ACTION_PARAM_SESSION_COUNT);
		const int serviceID = ot::json::getInt(doc, OT_ACTION_PARAM_SERVICE_ID);
		EntityBase::setUidGenerator(new DataStorageAPI::UniqueUIDGenerator(sessionCount, serviceID));
	}
	else if (serviceName == OT_INFO_SERVICE_TYPE_MODEL) {
		OT_LOG_D("Connecting with modelService");
		const std::string url = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		Application::instance().setModelServiceURL(url);

	}
	else if (serviceName == OT_INFO_SERVICE_TYPE_UI) {
		OT_LOG_D("Connecting with uiService");
		const std::string url = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		Application::instance().setUIServiceURL(url);
	}
	else {
		returnMessage = ot::ReturnMessage(ot::ReturnMessage::Failed, "Not supported initialisation order.");
	}

	return returnMessage;
}

ot::ReturnMessage ActionHandler::setLogFlags(const ot::JsonDocument& _doc) {
	ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(ot::json::getArray(_doc, OT_ACTION_PARAM_LogFlags)));
	return ot::ReturnMessage();
}

ot::ReturnMessage ActionHandler::handlePing(const ot::JsonDocument& _doc) {
	return ot::ReturnMessage(ot::ReturnMessage::Ok, OT_ACTION_CMD_Ping);
}

ot::ReturnMessage ActionHandler::shutdownProcess(const ot::JsonDocument& doc) {
	OT_LOG_D("Requested Shuttdown. Shutting down process now.");
	exit(0);
}

ot::ReturnMessage ActionHandler::executeScript(const ot::JsonDocument& doc) {
	try 
	{
		//Extract script entity names from json doc
		std::list<std::string> scripts = ot::json::getStringList(doc, OT_ACTION_CMD_PYTHON_Scripts);
		OT_LOG_D("Number of scripts being executed: " + std::to_string(scripts.size()));

		//Extract parameter array from json doc
		auto parameterArrayArray = ot::json::getArray(doc, OT_ACTION_CMD_PYTHON_Parameter);
		std::list<std::list<ot::Variable>> allParameter;

		ot::JSONToVariableConverter converterJ2V;
		ot::JsonSizeType numberOfParameterArrays = parameterArrayArray.Size();
		for (ot::JsonSizeType i = 0; i < numberOfParameterArrays; i++) {
			if (parameterArrayArray[i].IsNull()) {
				allParameter.push_back({});
			}
			else {
				
				auto parameterArray = ot::json::getArray(parameterArrayArray, i);
				//Todo: support of lists/maps as parameter

				std::list<ot::Variable> scriptParameter = converterJ2V(parameterArray);
				allParameter.emplace_back(scriptParameter);
			}
		}

		//Extract port data if existing
		if (ot::json::exists(doc,OT_ACTION_CMD_PYTHON_Portdata))
		{
			auto portData = ot::json::getArray(doc, OT_ACTION_CMD_PYTHON_Portdata);
			OT_LOG_D("Number of port datasets: " + std::to_string(portData.Size()));
			
			for (uint32_t i = 0; i < portData.Size(); i++) {
				auto portDataEntry = ot::json::getObject(portData, i);
				const std::string portName = ot::json::getString(portDataEntry,"Name");
				const ot::JsonValue& data = portDataEntry["Data"];
				const ot::JsonValue& metadata = portDataEntry["Meta"];
				DataBuffer::instance().addNewPortData(portName, ot::json::toJson(data), ot::json::toJson(metadata));
			}
		}

		//Execute
		m_pythonAPI.execute(scripts, allParameter);

		//put modified port data in return message. The data is not serialised here!! The buffer has to keep the data until the return message is serialised
		std::list<PortData*> modifiedPortData = DataBuffer::instance().getModifiedPortData();
		ot::ReturnValues returnValues;
		for (PortData* portData : modifiedPortData)
		{
			const std::string& portName = portData->getPortName();
			const ot::JsonValue& data = portData->getDataAndMetadata();
			const std::string tt =  ot::json::toJson(data);
			returnValues.addData(portName, &data);

		}

		auto& scriptReturnValues = DataBuffer::instance().getReturnDataByScriptName();
		for (auto& scriptReturnValue : scriptReturnValues)
		{
			returnValues.addData(scriptReturnValue.first, &scriptReturnValue.second);
		}


		if (returnValues.getNumberOfEntries() != 0) {
			return ot::ReturnMessage(std::move(returnValues));;
		}
		else {
			return ot::ReturnMessage(ot::ReturnMessage::Ok);
		}
	}
	catch (std::exception& e) {
		OT_LOG_D("Script execution failed due to exception: " + std::string(e.what()));
		return ot::ReturnMessage(ot::ReturnMessage::ReturnMessageStatus::Failed, e.what());
	}

}

ot::ReturnMessage ActionHandler::executeCommand(const ot::JsonDocument& doc) {
	std::string executionCommand = ot::json::getString(doc, OT_ACTION_CMD_PYTHON_Command);
	try {
		m_pythonAPI.execute(executionCommand);
		auto& result = DataBuffer::instance().getReturnDataByScriptName();
		if (!result.empty()) 
		{
			auto resultData = result.begin();
			
			ot::ReturnValues returnValue;
			returnValue.addData(resultData->first, &resultData->second);
			return ot::ReturnMessage(returnValue);
		}
		else {
			return ot::ReturnMessage(ot::ReturnMessage::Ok);
		}
	}
	catch (std::exception& e) {
		OT_LOG_D("Execution of command failed due to exception: " + std::string(e.what()));
		return ot::ReturnMessage(ot::ReturnMessage::Failed, e.what());
	}
}

ot::ReturnMessage ActionHandler::checkParameterExecuteScript(const ot::JsonDocument& doc) {
	if (!doc.HasMember(OT_ACTION_CMD_PYTHON_Scripts) || !doc.HasMember(OT_ACTION_CMD_PYTHON_Parameter)) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Either the scripts or parameter are not contained in the message");
	}
	else {
		return ot::ReturnMessage();
	}
}

ot::ReturnMessage ActionHandler::checkParameterExecuteCommand(const ot::JsonDocument& doc) {
	if (!doc.HasMember(OT_ACTION_CMD_PYTHON_Command)) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Message does not contain a command to execute");
	}
	else {
		return ot::ReturnMessage();
	}
}
