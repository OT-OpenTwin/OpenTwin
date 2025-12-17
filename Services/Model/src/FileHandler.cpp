// @otlicense
// File: FileHandler.cpp
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

#include "stdafx.h"
#include "Model.h"
#include "FileHandler.h"
#include "Application.h"
#include "EntityFileCSV.h"
#include "EntityFileText.h"
#include "OTCore/String.h"
#include "OTCore/FolderNames.h"
#include "OTCore/EncodingGuesser.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/Encryption.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "QueuingHttpRequestsRAII.h"
#include "QueuingDatabaseWritingRAII.h"
#include "OTServiceFoundation/UILockWrapper.h"
#include "OTServiceFoundation/ProgressUpdater.h"
#include "DataBase.h"
#include "DocumentAPI.h"
#include "GridFSFileInfo.h"
#include <assert.h>

FileHandler::FileHandler() {
	const std::string pageName = Application::getToolBarPageName();

	m_buttonPythonImport = ot::ToolBarButtonCfg(pageName, c_groupName, "Import Python Script", "Default/python");
	m_buttonPythonImport.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonFileImport = ot::ToolBarButtonCfg(pageName, c_groupName, "Import Text File", "Default/TextVisible");
	m_buttonFileImport.setButtonLockFlags(ot::LockType::ModelWrite);

	m_buttonHandler.connectToolBarButton(m_buttonFileImport, this, &FileHandler::handleImportTextFileButton);
	m_buttonHandler.connectToolBarButton(m_buttonPythonImport, this, &FileHandler::handleImportPythonScriptButton);

	m_actionHandler.connectAction(OT_ACTION_CMD_ImportTextFile, this, &FileHandler::handleImportTextFile);
	m_actionHandler.connectAction(OT_ACTION_CMD_ImportPyhtonScript, this, &FileHandler::handleImportPythonScript);

	m_actionHandler.connectAction(OT_ACTION_CMD_UI_RequestTextData, this, &FileHandler::handleRequestTextData);
}

void FileHandler::addButtons(ot::components::UiComponent* _uiComponent)
{
	const std::string pageName = Application::getToolBarPageName();
	
	_uiComponent->addMenuGroup(pageName, c_groupName);

	_uiComponent->addMenuButton(m_buttonPythonImport);
	_uiComponent->addMenuButton(m_buttonFileImport);
}

void FileHandler::handleImportTextFileButton() {
	const std::string fileMask = ot::FileExtension::toFilterString({ ot::FileExtension::Text, ot::FileExtension::CSV, ot::FileExtension::AllFiles });
	const std::string fileDialogTitle = "Import Text File";
	const std::string subsequentFunction = OT_ACTION_CMD_ImportTextFile;
	importFile(fileMask, fileDialogTitle, subsequentFunction);
}

void FileHandler::handleImportPythonScriptButton() {
	const std::string fileMask = ot::FileExtension::toFilterString({ ot::FileExtension::Python, ot::FileExtension::AllFiles });
	const std::string fileDialogTitle = "Import Python Script";
	const std::string subsequentFunction = OT_ACTION_CMD_ImportPyhtonScript;
	importFile(fileMask, fileDialogTitle, subsequentFunction);
}

void FileHandler::handleImportTextFile(ot::JsonDocument& _document) {
	std::thread worker(&FileHandler::storeTextFile, this, std::move(_document), std::ref(ot::FolderNames::FilesFolder));
	worker.detach();
}

void FileHandler::handleImportPythonScript(ot::JsonDocument& _document) {
	std::thread worker(&FileHandler::storeTextFile, this, std::move(_document), std::ref(ot::FolderNames::PythonScriptFolder));
	worker.detach();
}

void FileHandler::importFile(const std::string& _fileMask, const std::string& _dialogTitle, const std::string& _functionName)
{
	const std::string& serviceURL = Application::instance()->getServiceURL();
	const std::string serviceName =	Application::instance()->getServiceName();
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString(_dialogTitle, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(_fileMask, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString(_functionName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent, ot::JsonValue(true), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_LoadMultiple, ot::JsonValue(true), doc.GetAllocator());
	std::string response;
	Application::instance()->getUiComponent()->sendMessage(false, doc, response);
}

void FileHandler::storeTextFile(ot::JsonDocument&& _document, const std::string& _folderName)
{
	auto uiComponent =	Application::instance()->getUiComponent();
	ot::UILockWrapper uiLock(uiComponent, ot::LockType::ModelWrite);
	std::list<ot::GridFSFileInfo> fileInfos;
	for (const ot::ConstJsonObject& fileInfoObj : ot::json::getObjectList(_document, OT_ACTION_PARAM_FILE_Content)) {
		fileInfos.push_back(ot::GridFSFileInfo(fileInfoObj));
	}
	std::list<std::string> fileNames = ot::json::getStringList(_document, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileFilter = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mask);

	assert(fileInfos.size() == fileNames.size());
	{
		QueuingDatabaseWritingRAII queueDatabase;
		auto info = fileInfos.begin();
		
		ProgressUpdater updater(uiComponent, "Importing files");
		updater.setTotalNumberOfSteps(fileNames.size());
		uint32_t counter(0);
		uiComponent->displayMessage("Storing document(s) in database: ");
		auto start = std::chrono::system_clock::now();
		
		Model* model = Application::instance()->getModel();
		assert(model != nullptr);
		std::list<std::string> folderContent = model->getListOfFolderItems(_folderName, false);
		for (std::string& fileName : fileNames)
		{
			counter++;
			uint64_t dataLen = static_cast<uint64_t>(info->getUncompressedSize());

			DataStorageAPI::DocumentAPI api;
			uint8_t* dataBuffer = nullptr;
			size_t length = 0;

			bsoncxx::oid oid_obj{ info->getDocumentId() };
			bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };

			api.GetDocumentUsingGridFs(id, dataBuffer, length, info->getCollectionName());
			api.DeleteGridFSData(id, info->getCollectionName());

			std::string stringData(reinterpret_cast<char*>(dataBuffer), length);

			std::unique_ptr<uint8_t> data(ot::String::decompressBase64(stringData.c_str(), dataLen));
		
			storeFileInDataBase(std::string(reinterpret_cast<const char*>(data.get()), dataLen), fileName, folderContent, _folderName, fileFilter);
			info++;
			updater.triggerUpdate(counter);
		}
		auto end = std::chrono::system_clock::now();
		uint64_t passedTime =	std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		uiComponent->displayMessage(std::to_string(passedTime) + " ms\nModel update: ");
	}
	auto start = std::chrono::system_clock::now();
	addTextFilesToModel();
	clearBuffer();
	auto end = std::chrono::system_clock::now();
	uint64_t passedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	uiComponent->displayMessage(std::to_string(passedTime) + " ms\n");
}

void FileHandler::storeChangedTable(IVisualisationTable* _entity, const ot::TableCfg& _cfg)
{
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);
	assert(_entity != nullptr);

	_entity->setTable(_cfg.createMatrix());
	model->setModified();
	model->modelChangeOperationCompleted("Updated Table.");
}

ot::ReturnMessage FileHandler::textEditorSaveRequested(const std::string& _entityName, const std::string& _text, size_t _nextChunkStartIndex)
{
	Model* model =	Application::instance()->getModel();
	assert(model != nullptr);

	const auto entityIDsByName = model->getEntityNameToIDMap();
	auto entityIDByName	= entityIDsByName.find(_entityName);
	if(entityIDByName != entityIDsByName.end())
	{
		ot::UID entityID = entityIDByName->second;
		EntityBase* entityBase = model->getEntityByID(entityID);
		IVisualisationText* textVisualisationEntity = dynamic_cast<IVisualisationText*>(entityBase);
		if(textVisualisationEntity != nullptr)
		{
			std::list<std::string> handlingServices = entityBase->getServicesForCallback(EntityBase::Callback::DataHandle);
			if (!handlingServices.empty())
			{
				for (const std::string& owner : handlingServices) {
					if (owner != OT_INFO_SERVICE_TYPE_MODEL) {
						std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(ot::TextEditorActionHandler::createTextEditorSaveRequestDocument(_entityName, _text, _nextChunkStartIndex)), owner);
						workerThread.detach();
					}
				}
			}
			else {
				storeChangedText(textVisualisationEntity, _text, _nextChunkStartIndex);

				for (const std::string& notifyService : entityBase->getServicesForCallback(EntityBase::Callback::DataNotify)) {
					if (notifyService != OT_INFO_SERVICE_TYPE_MODEL) {
						ot::JsonDocument notify = ot::TextEditorActionHandler::createTextEditorSaveRequestDocument(_entityName, _text, _nextChunkStartIndex);
						std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(notify), notifyService);
						workerThread.detach();
					}
				}
			}

			return ot::ReturnMessage::Ok;
		}
		else
		{
			ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Failed to visualise " + _entityName + " since it does not support the corresponding visualisation interface.");
			OT_LOG_E(ret.getWhat());
			return ret;
		}
	}
	else
	{
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Failed to handle changed text request since the entity could not be found by name: " + _entityName);
		OT_LOG_E(ret.getWhat());
		return ret;
	}
}

ot::ReturnMessage FileHandler::tableSaveRequested(const ot::TableCfg& _cfg) {
	const std::string entityName = _cfg.getEntityName();

	Model* model = Application::instance()->getModel();
	assert(model != nullptr);

	const auto entityIDsByName = model->getEntityNameToIDMap();
	auto entityIDByName = entityIDsByName.find(entityName);
	if (entityIDByName != entityIDsByName.end()) {
		ot::UID entityID = entityIDByName->second;
		EntityBase* entityBase = model->getEntityByID(entityID);
		IVisualisationTable* tableVisualisationEntity = dynamic_cast<IVisualisationTable*>(entityBase);
		if (tableVisualisationEntity != nullptr) {
			std::list<std::string> handlingServices = entityBase->getServicesForCallback(EntityBase::Callback::DataHandle);

			if (!handlingServices.empty()) {
				for (const std::string& owner : handlingServices) {
					if (owner != OT_INFO_SERVICE_TYPE_MODEL) {
						std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(ot::TableActionHandler::createTableSaveRequestDocument(_cfg)), owner);
						workerThread.detach();
					}
				}
				return ot::ReturnMessage::Ok;
			}
			else {
				storeChangedTable(tableVisualisationEntity, _cfg);

				for (const std::string& notifyService : entityBase->getServicesForCallback(EntityBase::Callback::DataNotify)) {
					if (notifyService != OT_INFO_SERVICE_TYPE_MODEL) {
						ot::JsonDocument notify = ot::TableActionHandler::createTableSaveRequestDocument(_cfg);
						std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(notify), notifyService);
						workerThread.detach();
					}
				}
			}

			return ot::ReturnMessage::Ok;
		}
		else {
			ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Failed to visualise " + entityName + " since it does not support the corresponding visualisation interface.");
			OT_LOG_E(ret.getWhat());
			return ret;
		}
	}
	else {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Failed to handle changed text request since the entity could not be found by name: " + entityName);
		OT_LOG_E(ret.getWhat());
		return ret;
	}
}

ot::ReturnMessage FileHandler::handleRequestTextData(ot::JsonDocument& _document) {
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);

	std::string entityName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_EntityName);
	EntityBase* entityBase = model->findEntityFromName(entityName);
	if (!entityBase) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Entity not found  { \"Name\": \"" + entityName + "\" }");
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	IVisualisationText* textVisualisationEntity = dynamic_cast<IVisualisationText*>(entityBase);
	if (!textVisualisationEntity) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Entity has no text visualization interface { \"Name\": \"" + entityName + "\", \"Type\": \"" + entityBase->getClassName() + "\" }");
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	std::string text = textVisualisationEntity->getText();

	ot::GridFSFileInfo info;
	info.setCollectionName(DataBase::instance().getCollectionName());

	// Upload the data to gridFS
	DataStorageAPI::DocumentAPI db;

	bsoncxx::types::value result = db.InsertBinaryDataUsingGridFs(reinterpret_cast<const uint8_t*>(text.c_str()), text.size(), info.getCollectionName());
	info.setDocumentId(result.get_oid().value.to_string());

	return ot::ReturnMessage(ot::ReturnMessage::Ok, info.toJson());
}

void FileHandler::storeChangedText(IVisualisationText* _entity, const std::string _text, size_t _nextChunkStartIndex)
{
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);
	assert(_entity != nullptr);

	if (_nextChunkStartIndex > 0) {
		std::string existingTextEnd = _entity->getText().substr(_nextChunkStartIndex);
		std::string newText = _text + existingTextEnd;
		_entity->setText(newText);
	}
	else {
		_entity->setText(_text);
	}

	model->setModified();
	model->modelChangeOperationCompleted("Updated Text.");
}

void FileHandler::NotifyOwnerAsync(ot::JsonDocument&& _doc, const std::string _owner)
{
	std::string response;
	Application::instance()->sendMessage(true, _owner, _doc, response);
}

void FileHandler::storeFileInDataBase(const std::string& _text, const std::string& _fileName, std::list<std::string>& _folderContent, const std::string& _folderName, const std::string& _fileFilter)
{
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);
	
	ot::UID entIDData =	model->createEntityUID();
	ot::UID entIDTopo =	model->createEntityUID();
	const std::string serviceName = Application::instance()->getServiceName();

	size_t fileNamePos = _fileName.find_last_of("/");
	std::string path = _fileName.substr(0, fileNamePos);
	std::string name = _fileName.substr(fileNamePos + 1);
	std::string type = ot::String::toLower(name.substr(name.find_last_of('.') + 1));

	std::unique_ptr<EntityFileText> textFile;
	if (type == "csv")
	{
		textFile.reset(new EntityFileCSV (entIDTopo, nullptr, nullptr, nullptr));
	}
	else
	{
		textFile.reset(new EntityFileText(entIDTopo, nullptr, nullptr, nullptr));
	}

	EntityBinaryData fileContent(entIDData, textFile.get(), nullptr, nullptr);
	fileContent.setData(_text.data(), _text.size());
	fileContent.storeToDataBase();

	textFile->setDataEntity(fileContent);

	ot::EncodingGuesser guesser;
	textFile->setFileProperties(path, name,type);

	textFile->setFileFilter(_fileFilter);

	const std::string entityName = CreateNewUniqueTopologyName(_folderContent, _folderName, name);;
	textFile->setName(entityName);
	_folderContent.push_back(entityName);

	textFile->setTextEncoding(guesser(_text.data(), _text.size()));
	textFile->storeToDataBase();
	m_entityIDsTopo.push_back(entIDTopo);
	m_entityVersionsTopo.push_back(textFile->getEntityStorageVersion());
	m_entityIDsData.push_back(entIDData);
	m_entityVersionsData.push_back(fileContent.getEntityStorageVersion());
	m_forceVisible.push_back(false);
}

void FileHandler::clearBuffer()
{
	m_entityVersionsData.clear();
	m_entityVersionsTopo.clear();
	m_entityIDsData.clear();
	m_entityIDsTopo.clear();
	m_forceVisible.clear();
}

void FileHandler::addTextFilesToModel()
{
	QueuingHttpRequestsRAII wrapper;
	Model* model = Application::instance()->getModel();
	model->addEntitiesToModel(m_entityIDsTopo, m_entityVersionsTopo, m_forceVisible, m_entityIDsData, m_entityVersionsData, m_entityIDsTopo, "Added File", true, false, true);
}