// @otlicense
// File: PythonServiceInterface.cpp
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

#include "OTServiceFoundation/PythonServiceInterface.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/VariableToJSONConverter.h"

#include "DocumentAPI.h"
#include "DataBase.h"

ot::PythonServiceInterface::PythonServiceInterface(const std::string& _pythonExecutionServiceURL)
	: m_pythonExecutionServiceURL(_pythonExecutionServiceURL)
{}

void ot::PythonServiceInterface::addScriptWithParameter(const std::string& _scriptName, const scriptParameter& _scriptParameter)
{
	m_scriptNamesWithParameter.push_back(std::make_tuple(_scriptName, _scriptParameter));
}

void ot::PythonServiceInterface::addPortData(const std::string& _portName, const ot::JsonValue* _data, const ot::JsonValue* _metadata)
{
	assert(m_portDataByPortName.find(_portName) == m_portDataByPortName.end());
	m_portDataByPortName.insert(std::pair<std::string, std::pair<const ot::JsonValue*, const ot::JsonValue*>>(_portName, std::pair(_data,_metadata)));
}

void ot::PythonServiceInterface::addEnvironmentID(ot::UID _manifestUID)
{
	m_manifestUID = _manifestUID;
}

ot::ReturnMessage ot::PythonServiceInterface::sendExecutionOrder()
{
	if (m_scriptNamesWithParameter.size() == 0)
	{
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "PythonServiceInterface got nothing to execute.");
	}

	std::string response;
	OT_LOG_D("Sending python execution request");
	ot::msg::send("", m_pythonExecutionServiceURL, ot::MessageType::EXECUTE, this->assembleMessage().toJson(), response,0);
	ot::ReturnMessage message = ot::ReturnMessage::fromJson(response);

	if (message.isOk())
	{
				// If the message is ok, we need to read back the port data from the database
		ot::ReturnValues& returnValuesWrapper = message.getValues();
		ot::JsonDocument& returnedValue = returnValuesWrapper.getValues();

		if (returnedValue.HasMember(OT_ACTION_CMD_PYTHON_Portdata))
		{
			const ot::JsonValue& portDataEntry = returnedValue[OT_ACTION_CMD_PYTHON_Portdata];
			if (portDataEntry.IsString())
			{
				std::string gridFSDocumentID = portDataEntry.GetString();
				ot::JsonDocument portDataDocument = this->readPortDataFromDatabase(gridFSDocumentID);
				ot::JsonObject portData;
				portData.CopyFrom(portDataDocument.Move(), returnValuesWrapper.getAllocator());

				returnedValue.RemoveMember(OT_ACTION_CMD_PYTHON_Portdata);
				returnValuesWrapper.addData(OT_ACTION_CMD_PYTHON_Portdata, portData);
			}
		}
	}

	return message;
}

ot::ReturnMessage ot::PythonServiceInterface::sendSingleExecutionCommand(const std::string& command)
{
	std::string response;
	OT_LOG_D("Sending python execution request");
	JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_PYTHON_EXECUTE_Command, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_CMD_PYTHON_Command, ot::JsonString(command, doc.GetAllocator()), doc.GetAllocator());
	ot::msg::send("", m_pythonExecutionServiceURL, ot::MessageType::EXECUTE, doc.toJson(), response, 0);
	return ot::ReturnMessage::fromJson(response);
}

ot::JsonDocument ot::PythonServiceInterface::assembleMessage()
{
	JsonDocument doc;
	JsonArray allparameter;
	JsonArray scripts;
	ot::VariableToJSONConverter converter;

	for (auto& scriptWithParameter : m_scriptNamesWithParameter)
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

	m_scriptNamesWithParameter.clear();

	std::string gridFSDocumentID = writePortDataToDatabase();
	if (!gridFSDocumentID.empty())
	{
		doc.AddMember(OT_ACTION_CMD_PYTHON_Portdata, ot::JsonString(gridFSDocumentID, doc.GetAllocator()), doc.GetAllocator());
	}

	doc.AddMember(OT_ACTION_CMD_PYTHON_Parameter, allparameter, doc.GetAllocator());
	doc.AddMember(OT_ACTION_CMD_PYTHON_Scripts, scripts, doc.GetAllocator());
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_PYTHON_EXECUTE_Scripts, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, ot::JsonString(std::to_string(m_manifestUID),doc.GetAllocator()), doc.GetAllocator());
	m_manifestUID = ot::invalidUID;
	return doc;
}

std::string ot::PythonServiceInterface::writePortDataToDatabase()
{
	ot::JsonDocument doc;
	if (m_portDataByPortName.size() > 0)
	{
		ot::JsonArray portDataEntries;

		for (auto& portDataByPortName : m_portDataByPortName)
		{
			ot::JsonObject portDataEntry;
			ot::JsonString portName(portDataByPortName.first.c_str(), doc.GetAllocator());
			portDataEntry.AddMember("Name", portName, doc.GetAllocator());

			const ot::JsonValue* portData = portDataByPortName.second.first;
			assert(portData != nullptr);
			ot::JsonValue portDataCopy;
			portDataCopy.CopyFrom(*portData, doc.GetAllocator());
			portDataEntry.AddMember("Data", portDataCopy, doc.GetAllocator());


			const ot::JsonValue* portMetaData = portDataByPortName.second.second;
			assert(portMetaData != nullptr);
			ot::JsonValue portMetaDataCopy;
			portMetaDataCopy.CopyFrom(*portMetaData, doc.GetAllocator());
			portDataEntry.AddMember("Meta", portMetaDataCopy, doc.GetAllocator());

			portDataEntries.PushBack(portDataEntry, doc.GetAllocator());
		}
		doc.AddMember(OT_ACTION_CMD_PYTHON_Portdata, portDataEntries, doc.GetAllocator());
		m_portDataByPortName.clear();
	}

	if (!doc.Empty())
	{
		const std::string portData = ot::json::toJson(doc);
		std::stringstream stream(portData);
	
		DataStorageAPI::DocumentAPI gridFSHandle;
		auto documentID = gridFSHandle.InsertDocumentUsingCollectionGridFs(&stream, DataBase::instance().getCollectionName());
		const std::string gridFSDocumentID = documentID.get_oid().value.to_string();
		return gridFSDocumentID;
	}
	else
	{
		return "";
	}
}

ot::JsonDocument ot::PythonServiceInterface::readPortDataFromDatabase(const std::string& _gridFSDocumentID)
{
	//Load data from gridFS
	DataStorageAPI::DocumentAPI gridFSHandle;
	uint8_t* dataBuffer = nullptr;
	size_t length = 0;

	bsoncxx::oid oid_obj{ _gridFSDocumentID };
	bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };

	gridFSHandle.GetDocumentUsingGridFs(id, dataBuffer, length, DataBase::instance().getCollectionName());
	gridFSHandle.DeleteGridFSData(id, DataBase::instance().getCollectionName());

	const std::string portDataJsonString(reinterpret_cast<char*>(dataBuffer), length);
	delete[] dataBuffer;
	dataBuffer = nullptr;

	ot::JsonDocument portDataDoc;
	portDataDoc.fromJson(portDataJsonString);
	return portDataDoc;
}
