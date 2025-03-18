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

ActionHandler& ActionHandler::instance(void) {
	static ActionHandler g_instance;
	return g_instance;
}

ot::ReturnMessage ActionHandler::handleAction(const ot::JsonDocument& doc) {
	std::lock_guard<std::mutex> lock(m_mutex);
	
	ot::ReturnMessage returnMessage ;

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

}

ot::ReturnMessage ActionHandler::initialise(const ot::JsonDocument& doc) {
	const std::string serviceName = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_NAME);
	ot::ReturnMessage returnMessage = ot::ReturnMessage(ot::ReturnMessage::Ok, "Initialisation succeeded");
	if (serviceName == OT_INFO_SERVICE_TYPE_DataBase) {
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
		returnMessage =ot::ReturnMessage(ot::ReturnMessage::Failed, "Not supported initialisation order.");
	}
	return returnMessage;
}

ot::ReturnMessage ActionHandler::handlePing(const ot::JsonDocument& _doc) {
	return ot::ReturnMessage(ot::ReturnMessage::Ok, OT_ACTION_CMD_Ping);
}

ot::ReturnMessage ActionHandler::shutdownProcess(const ot::JsonDocument& doc) {
	OT_LOG_D("Requested Shuttdown. Shutting down process now.");
	exit(0);
}

ot::ReturnMessage ActionHandler::executeScript(const ot::JsonDocument& doc) {
	try {
		std::string temp = doc.toJson();

		PortDataBuffer::instance().clearPortData();

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
		if (doc.HasMember(OT_ACTION_CMD_PYTHON_Portdata_Names)) {
			std::list<std::string> portDataNames = ot::json::getStringList(doc, OT_ACTION_CMD_PYTHON_Portdata_Names);
			OT_LOG_D("Number of port datasets: " + std::to_string(portDataNames.size()));
			auto portData = doc[OT_ACTION_CMD_PYTHON_Portdata_Data].GetArray();
			auto portName = portDataNames.begin();
			for (uint32_t i = 0; i < portData.Size(); i++) {
				auto portDataEntries = ot::json::getArray(portData, i);
				ot::GenericDataStructList values;
				for (const auto& jGenericDataStruct : portDataEntries) {
					ot::GenericDataStruct* genericDataStruct = ot::GenericDataStructFactory::Create(jGenericDataStruct.GetObject());
					values.push_back(genericDataStruct);
				}

				PortDataBuffer::instance().addNewPortData(*portName, values);
				portName++;
			}
		}

		//Execute
		ot::ReturnValues result = m_pythonAPI.execute(scripts, allParameter);
		PortDataBuffer::instance().addModifiedPortData(result);

		if (result.getNumberOfEntries() != 0) {
			return ot::ReturnMessage(std::move(result));;
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
		ot::ReturnValues result = m_pythonAPI.execute(executionCommand);
		if (result.getNumberOfEntries() != 0) {
			return ot::ReturnMessage(result);
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
