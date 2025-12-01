// @otlicense
// File: ActionHandler.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "ActionHandler.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/Variable.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/ReturnValues.h"
#include "OTCore/GenericDataStruct.h"
#include "OTCore/GenericDataStructFactory.h"
#include "DataBuffer.h"

#include "Application.h"
#include "DataBase.h"
#include "EntityBase.h"
#include "DocumentAPI.h"
#include "PackageHandler.h"

#include "OTServiceFoundation/TimeFormatter.h"
#include "ExceptionRestartRequired.h"

ActionHandler& ActionHandler::instance(void) {
	static ActionHandler g_instance;
	return g_instance;
}

ot::ReturnMessage ActionHandler::handleAction(const ot::JsonDocument& doc) {
	std::lock_guard<std::mutex> lock(m_mutex);
	
	ot::ReturnMessage returnMessage;

	std::string action = ot::json::getString(doc, OT_ACTION_MEMBER);
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
	
		DataBase::instance().setCollectionName(collectionName);
		DataBase::instance().setUserCredentials(userName, psw);
		DataBase::instance().initializeConnection(url);

		ot::UID manifestUID = ot::json::getUInt64(doc, OT_ACTION_PARAM_Python_Environment);
		m_pythonAPI.initializeEnvironment(manifestUID);
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
		std::string environmentName = "Pyrit";
		m_pythonAPI.initializeEnvironment(environmentName);
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
	else 
	{
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
		OT_LOG_D("Python subservice message received");
		auto start = std::chrono::high_resolution_clock::now();
		//Extract script entity names from json doc
		std::list<std::string> scripts = ot::json::getStringList(doc, OT_ACTION_CMD_PYTHON_Scripts);
		OT_LOG_D("Number of scripts being executed: " + std::to_string(scripts.size()));
		ot::UID manifestUID = ot::json::getUInt64(doc, OT_ACTION_PARAM_Python_Environment);
		PackageHandler::instance().initializeManifest(manifestUID);

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
		loadPortDataToBuffer(doc);
		//Execute
		m_pythonAPI.execute(scripts, allParameter);

		//put modified port data in return message. The data is not serialised here!! The buffer has to keep the data until the return message is serialised
		const std::string gridFSDocumentID =  writeReturnDataToDatabase();
		auto end = std::chrono::high_resolution_clock::now();
		const std::string duration = TimeFormatter::formatDuration(start, end);
		OT_LOG_D("Python script execution time took : " + duration + "\n");
		OT_LOG_D("Python subservice message returning");
		ot::ReturnValues returnValues;

		if (gridFSDocumentID.empty())
		{
			return ot::ReturnMessage(ot::ReturnMessage::Ok);
		}
		else
		{
			returnValues.addData(OT_ACTION_CMD_PYTHON_Portdata, ot::JsonString(gridFSDocumentID, returnValues.getAllocator()));
			return ot::ReturnMessage(std::move(returnValues));;
		}
	}
	catch (ExceptionRestartRequired&)
	{
		ot::ReturnMessage message;
		message.setStatus(ot::ReturnMessage::Ok);
		message.setWhat("<Restart>");
		return message;
	}
	catch (std::exception& e) {
		OT_LOG_D("Script execution failed due to exception: " + std::string(e.what()));
		return ot::ReturnMessage(ot::ReturnMessage::ReturnMessageStatus::Failed, e.what());
	}

}

void ActionHandler::loadPortDataToBuffer(const ot::JsonDocument& _doc)
{
	//Extract port data if existing
	if (ot::json::exists(_doc, OT_ACTION_CMD_PYTHON_Portdata))
	{
		const std::string gridFSDocumentID = ot::json::getString(_doc, OT_ACTION_CMD_PYTHON_Portdata);

		//Load data from gridFS
		DataStorageAPI::DocumentAPI gridFSHandle;
		uint8_t* dataBuffer = nullptr;
		size_t length = 0;

		bsoncxx::oid oid_obj{ gridFSDocumentID };
		bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };

		gridFSHandle.GetDocumentUsingGridFs(id, dataBuffer, length, DataBase::instance().getCollectionName());
		const std::string portDataJsonString(reinterpret_cast<char*>(dataBuffer), length);
		delete[] dataBuffer;
		dataBuffer = nullptr;
		ot::JsonDocument portDataDoc;
		portDataDoc.fromJson(portDataJsonString);
		
		auto portData = ot::json::getArray(portDataDoc, OT_ACTION_CMD_PYTHON_Portdata);
		OT_LOG_D("Number of port datasets: " + std::to_string(portData.Size()));

		for (uint32_t i = 0; i < portData.Size(); i++) {
			auto portDataEntry = ot::json::getObject(portData, i);
			const std::string portName = ot::json::getString(portDataEntry, "Name");
			const ot::JsonValue& data = portDataEntry["Data"];
			const ot::JsonValue& metadata = portDataEntry["Meta"];
			DataBuffer::instance().addNewPortData(portName, ot::json::toJson(data), ot::json::toJson(metadata));
		}

		gridFSHandle.DeleteGridFSData(id, DataBase::instance().getCollectionName());
	}
}

std::string ActionHandler::writeReturnDataToDatabase()
{
	std::list<PortData*> modifiedPortData = DataBuffer::instance().getModifiedPortData();
	ot::JsonDocument returnDataDoc;

	for (PortData* portData : modifiedPortData)
	{
		const std::string& portName = portData->getPortName();
		ot::JsonValue& data = portData->getDataAndMetadata();
		ot::JsonValue portDataEntry(data.Move(), returnDataDoc.GetAllocator());
		returnDataDoc.AddMember(ot::JsonValue(portName.c_str(), returnDataDoc.GetAllocator()).Move(), portDataEntry.Move(), returnDataDoc.GetAllocator());
	}

	auto& scriptReturnValues = DataBuffer::instance().getReturnDataByScriptName();
	for (auto& scriptReturnValue : scriptReturnValues)
	{
		const std::string& scriptName = scriptReturnValue.first;
		ot::JsonValue& data = scriptReturnValue.second;
		ot::JsonValue scriptDataEntry(data.Move(), returnDataDoc.GetAllocator());
		returnDataDoc.AddMember(ot::JsonValue(scriptName.c_str(), returnDataDoc.GetAllocator()).Move(), scriptDataEntry.Move(), returnDataDoc.GetAllocator());
	}

	if(returnDataDoc.ObjectEmpty())
	{
		return "";
	}

	const std::string returnDataJson = returnDataDoc.toJson();
	std::stringstream stream(returnDataJson);

	DataStorageAPI::DocumentAPI gridFSHandle;
	auto documentID = gridFSHandle.InsertDocumentUsingCollectionGridFs(&stream, DataBase::instance().getCollectionName());
	const std::string gridFSDocumentID = documentID.get_oid().value.to_string();
	return gridFSDocumentID;
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
