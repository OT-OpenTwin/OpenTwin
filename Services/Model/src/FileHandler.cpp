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

// Service header
#include "Model.h"
#include "FileHandler.h"
#include "Application.h"
#include "QueuingHttpRequestsRAII.h"
#include "QueuingDatabaseWritingRAII.h"

// OpenTwin header
#include "OTSystem/OperatingSystem.h"

#include "OTCore/String.h"
#include "OTCore/FolderNames.h"
#include "OTCore/EncodingGuesser.h"
#include "OTCore/Python/PyhonParameterBuilderValueComparisons.h"

#include "OTCommunication/ActionTypes.h"

#include "OTServiceFoundation/Encryption.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UILockWrapper.h"
#include "OTServiceFoundation/ProgressUpdater.h"
#include "OTServiceFoundation/Python/PythonServiceInterface.h"

#include "OTDataStorage/DocumentAPI.h"
#include "OTDataStorage/GridFSFileInfo.h"

#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityFileCSV.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTModelEntities/IEventHandler.h"
#include "OTModelEntities/EntityPythonManifest.h"

#include "OTGui/Dialog/PropertyDialogCfg.h"
#include "OTGui/Properties/PropertyGroup.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTGui/Properties/PropertyString.h"

#include "OTGuiAPI/Frontend.h"

// std header
#include <assert.h>
#include <filesystem>

FileHandler::FileHandler() {
	const std::string pageName = Application::getToolBarPageName();

	m_buttonPythonImport = ot::ToolBarButtonCfg(pageName, c_groupName, "Import Python Script", "Default/python");
	m_buttonPythonImport.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonFileImport = ot::ToolBarButtonCfg(pageName, c_groupName, "Import Text File", "Default/TextVisible");
	m_buttonFileImport.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonExportFileToLibrary = ot::ToolBarButtonCfg(pageName, c_groupName, "Export File to Library ", "Default/Export");
	m_buttonExportFileToLibrary.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonExportToUserLibrary = ot::ToolBarButtonCfg(pageName, c_groupName, "Export to User Library", "Default/Export");
	m_buttonExportToUserLibrary.setButtonLockFlags(ot::LockType::ModelWrite);

	m_buttonHandler.connectToolBarButton(m_buttonFileImport, this, &FileHandler::handleImportTextFileButton);
	m_buttonHandler.connectToolBarButton(m_buttonPythonImport, this, &FileHandler::handleImportPythonScriptButton);
	m_buttonHandler.connectToolBarButton(m_buttonExportFileToLibrary, this, &FileHandler::handleExportFilesToLibrary);
	m_buttonHandler.connectToolBarButton(m_buttonExportToUserLibrary, this, &FileHandler::handleExportToUserLibrary);

	m_actionHandler.connectAction(OT_ACTION_CMD_ImportTextFile, this, &FileHandler::handleImportTextFile);
	m_actionHandler.connectAction(OT_ACTION_CMD_ImportPyhtonScript, this, &FileHandler::handleImportPythonScript);

	m_actionHandler.connectAction(OT_ACTION_CMD_UI_RequestTextData, this, &FileHandler::handleRequestTextData);

	m_actionHandler.connectAction(std::string(c_promptActionOverwriteFile), this, &FileHandler::handleOverwriteFilePromptResponse);
}

void FileHandler::addButtons(ot::components::UiComponent* _uiComponent)
{
	const std::string pageName = Application::getToolBarPageName();
	
	_uiComponent->addMenuGroup(pageName, c_groupName);

	_uiComponent->addMenuButton(m_buttonPythonImport);
	_uiComponent->addMenuButton(m_buttonFileImport);
	_uiComponent->addMenuButton(m_buttonExportToUserLibrary);

	if (!ot::OperatingSystem::getEnvironmentVariableString("OPENTWIN_DEV_ROOT").empty()) {
		_uiComponent->addMenuButton(m_buttonExportFileToLibrary);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Button Handler

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

void FileHandler::handleExportFilesToLibrary() {
	// This is the local export, show metadata file lists
	showExportDialog("Export to Library", OT_ACTION_CMD_ExportFilesToLibrary, true);
}

void FileHandler::handleExportToUserLibrary() {
	// This is the user export, show additional info fields
	showExportDialog("Export to User Library", OT_ACTION_CMD_ExportFileToUserLibrary, false);
}

void FileHandler::showExportDialog(const std::string& _title, const std::string& _callbackAction, bool _isLocalExport) {
	Model* model = Application::instance()->getModel();

	// Get Entities from respective folders
	std::string circuitModelFolder = ot::FolderNames::FilesFolder;
	std::string pythonScriptFolder = ot::FolderNames::PythonScriptFolder;
	std::string manifestFolder = ot::FolderNames::PythonManifestFolder;
	std::string metaFileFolder = ot::FolderNames::FilesFolder;

	auto circuitModels = model->getListOfFolderItems(circuitModelFolder, true);
	auto pythonScripts = model->getListOfFolderItems(pythonScriptFolder, true);
	auto manifestFiles = model->getListOfFolderItems(manifestFolder, true);
	auto metaFiles = model->getListOfFolderItems(metaFileFolder, true);

	ot::PropertyDialogCfg dialogConfig;
	dialogConfig.setTitle(_title);
	dialogConfig.setName(_isLocalExport ? "ExportToLibraryDialog" : "ExportToUserLibraryDialog");

	ot::PropertyGridCfg gridConfig;
	ot::PropertyGroup* pythonScriptGroup = new ot::PropertyGroup(m_pythonScriptDialogGroup);
	ot::PropertyGroup* circuitModelGroup = new ot::PropertyGroup(m_circuitModelDialogGroup);

	// Create basic properties (always present)
	ot::PropertyStringList* circuitModelsProp = new ot::PropertyStringList("Circuit Models", "", circuitModels);
	ot::PropertyStringList* pythonScriptProp = new ot::PropertyStringList("Python Script", "", pythonScripts);

	circuitModelGroup->addProperty(circuitModelsProp);
	pythonScriptGroup->addProperty(pythonScriptProp);

	if (_isLocalExport) {
		// Local Export: Show dropdowns for all metadata files
		ot::PropertyStringList* circuitModelMetaProp = new ot::PropertyStringList("Circuit Model Metadata File", "", metaFiles);
		ot::PropertyStringList* pythonManifestProp = new ot::PropertyStringList("Python Manifest", "", manifestFiles);
		ot::PropertyStringList* pythonMetaProp = new ot::PropertyStringList("Python Metadata File", "", metaFiles);
		ot::PropertyStringList* manifestMetaProp = new ot::PropertyStringList("Manifest Metadata File", "", metaFiles);

		circuitModelGroup->addProperty(circuitModelMetaProp);
		pythonScriptGroup->addProperty(pythonManifestProp);
		pythonScriptGroup->addProperty(pythonMetaProp);
		pythonScriptGroup->addProperty(manifestMetaProp);
	}
	else {
		// User Export: Show manifest selection and text fields for additional info
		ot::PropertyStringList* pythonManifestProp = new ot::PropertyStringList("Python Manifest", "", manifestFiles);
		ot::PropertyString* circuitAdditionalInfoProp = new ot::PropertyString("Circuit Model Additional Info", "");
		ot::PropertyString* circuitDescriptionProp = new ot::PropertyString("Circuit Model Description", "");
		ot::PropertyString* pythonAdditionalInfoProp = new ot::PropertyString("Python Script Additional Info", "");
		ot::PropertyString* pythonDescriptionProp = new ot::PropertyString("Python Script Description", "");
		ot::PropertyString* manifestAdditionalInfoProp = new ot::PropertyString("Manifest Additional Info", "");
		ot::PropertyString* manifestDescriptionProp = new ot::PropertyString("Manifest Description", "");

		circuitModelGroup->addProperty(circuitAdditionalInfoProp);
		circuitModelGroup->addProperty(circuitDescriptionProp);
		pythonScriptGroup->addProperty(pythonManifestProp);
		pythonScriptGroup->addProperty(pythonAdditionalInfoProp);
		pythonScriptGroup->addProperty(pythonDescriptionProp);
		pythonScriptGroup->addProperty(manifestAdditionalInfoProp);
		pythonScriptGroup->addProperty(manifestDescriptionProp);
	}

	gridConfig.addRootGroup(pythonScriptGroup);
	gridConfig.addRootGroup(circuitModelGroup);
	dialogConfig.setGridConfig(gridConfig);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_PropertyDialog, doc.GetAllocator()), doc.GetAllocator());
	ot::JsonObject jConfig;
	dialogConfig.addToJsonObject(jConfig, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, jConfig, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString(_callbackAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(Application::instance()->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	Application::instance()->getUiComponent()->sendMessage(true, doc, response);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action Handler

void FileHandler::handleImportTextFile(ot::JsonDocument& _document) {
	std::thread worker(&FileHandler::storeTextFile, this, std::move(_document), std::ref(ot::FolderNames::FilesFolder));
	worker.detach();
}

void FileHandler::handleImportPythonScript(ot::JsonDocument& _document) {
	std::thread worker(&FileHandler::storeTextFile, this, std::move(_document), std::ref(ot::FolderNames::PythonScriptFolder));
	worker.detach();
}

ot::ReturnMessage FileHandler::textEditorSaveRequested(const std::string& _entityName, const std::string& _text, size_t _nextChunkStartIndex)
{
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);

	const auto entityIDsByName = model->getEntityNameToIDMap();
	auto entityIDByName = entityIDsByName.find(_entityName);
	if (entityIDByName != entityIDsByName.end())
	{
		ot::UID entityID = entityIDByName->second;
		EntityBase* entityBase = model->getEntityByID(entityID);
		ot::IVisualisationText* textVisualisationEntity = dynamic_cast<ot::IVisualisationText*>(entityBase);
		if (textVisualisationEntity != nullptr)
		{
			std::list<std::string> handlingServices = entityBase->getServicesForCallback(EntityBase::Callback::DataHandle);
			if (!handlingServices.empty())
			{
				for (const std::string& owner : handlingServices)
				{
					if (owner != OT_INFO_SERVICE_TYPE_MODEL)
					{
						std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(ot::TextEditorActionHandler::createTextEditorSaveRequestDocument(_entityName, _text, _nextChunkStartIndex)), owner);
						workerThread.detach();
					}
				}
			}
			else
			{
				storeChangedText(textVisualisationEntity, _text, _nextChunkStartIndex);

				for (const std::string& notifyService : entityBase->getServicesForCallback(EntityBase::Callback::DataNotify))
				{
					if (notifyService != OT_INFO_SERVICE_TYPE_MODEL)
					{
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

ot::ReturnMessage FileHandler::tableSaveRequested(const ot::TableCfg& _cfg)
{
	const std::string entityName = _cfg.getEntityName();

	Model* model = Application::instance()->getModel();
	assert(model != nullptr);

	const auto entityIDsByName = model->getEntityNameToIDMap();
	auto entityIDByName = entityIDsByName.find(entityName);
	if (entityIDByName != entityIDsByName.end())
	{
		ot::UID entityID = entityIDByName->second;
		EntityBase* entityBase = model->getEntityByID(entityID);
		ot::IVisualisationTable* tableVisualisationEntity = dynamic_cast<ot::IVisualisationTable*>(entityBase);
		if (tableVisualisationEntity != nullptr)
		{
			std::list<std::string> handlingServices = entityBase->getServicesForCallback(EntityBase::Callback::DataHandle);

			if (!handlingServices.empty())
			{
				for (const std::string& owner : handlingServices)
				{
					if (owner != OT_INFO_SERVICE_TYPE_MODEL)
					{
						std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(ot::TableActionHandler::createTableSaveRequestDocument(_cfg)), owner);
						workerThread.detach();
					}
				}
				return ot::ReturnMessage::Ok;
			}
			else
			{
				storeChangedTable(tableVisualisationEntity, _cfg);

				for (const std::string& notifyService : entityBase->getServicesForCallback(EntityBase::Callback::DataNotify))
				{
					if (notifyService != OT_INFO_SERVICE_TYPE_MODEL)
					{
						ot::JsonDocument notify = ot::TableActionHandler::createTableSaveRequestDocument(_cfg);
						std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(notify), notifyService);
						workerThread.detach();
					}
				}
			}

			return ot::ReturnMessage::Ok;
		}
		else
		{
			ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Failed to visualise " + entityName + " since it does not support the corresponding visualisation interface.");
			OT_LOG_E(ret.getWhat());
			return ret;
		}
	}
	else
	{
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Failed to handle changed text request since the entity could not be found by name: " + entityName);
		OT_LOG_E(ret.getWhat());
		return ret;
	}
}

ot::ReturnMessage FileHandler::tableColumnFilterChanged(const ot::TableFilterChangeEvent& _event)
{
	const std::string entityName = _event.getTableInformation().getEntityName();

	Model* model = Application::instance()->getModel();
	assert(model != nullptr);

	const auto entityIDsByName = model->getEntityNameToIDMap();
	auto entityIDByName = entityIDsByName.find(entityName);
	if (entityIDByName != entityIDsByName.end())
	{
		ot::UID entityID = entityIDByName->second;
		EntityBase* entityBase = model->getEntityByID(entityID);
		ot::IVisualisationTable* tableVisualisationEntity = dynamic_cast<ot::IVisualisationTable*>(entityBase);
		if (tableVisualisationEntity != nullptr)
		{
			std::list<std::string> handlingServices = entityBase->getServicesForCallback(EntityBase::Callback::DataHandle);

			if (!handlingServices.empty())
			{
				for (const std::string& owner : handlingServices)
				{
					if (owner != OT_INFO_SERVICE_TYPE_MODEL)
					{
						ot::TableFilterChangeEvent event(_event);
						event.setForwarding();
						std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(ot::TableActionHandler::createTableColumnFilterChangeRequestDocument(event)), owner);
						workerThread.detach();
					}
				}
				return ot::ReturnMessage::Ok;
			}
			else
			{
				processTableColumnFilterChanged(_event, tableVisualisationEntity);

				for (const std::string& notifyService : entityBase->getServicesForCallback(EntityBase::Callback::DataNotify))
				{
					if (notifyService != OT_INFO_SERVICE_TYPE_MODEL)
					{
						ot::TableFilterChangeEvent event(_event);
						event.setForwarding();
						std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(ot::TableActionHandler::createTableColumnFilterChangeRequestDocument(event)), notifyService);
						workerThread.detach();
					}
				}
			}

			return ot::ReturnMessage::Ok;
		}
		else
		{
			ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Failed to handle table column filter changed event for entity \"" + entityName + "\" since it does not support the corresponding visualisation interface.");
			OT_LOG_E(ret.getWhat());
			return ret;
		}
	}
	else
	{
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Failed to handle table column filter changed event since entity \"" + entityName + "\" was not found");
		OT_LOG_E(ret.getWhat());
		return ret;
	}
}

ot::ReturnMessage FileHandler::handleRequestTextData(ot::JsonDocument& _document)
{
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);

	std::string entityName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_EntityName);
	EntityBase* entityBase = model->findEntityFromName(entityName);
	if (!entityBase)
	{
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Entity not found  { \"Name\": \"" + entityName + "\" }");
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	ot::IVisualisationText* textVisualisationEntity = dynamic_cast<ot::IVisualisationText*>(entityBase);
	if (!textVisualisationEntity)
	{
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

void FileHandler::handleOverwriteFilePromptResponse(ot::JsonDocument& _document)
{
	ot::MessageDialogCfg::BasicButton result = ot::MessageDialogCfg::stringToButton(ot::json::getString(_document, OT_ACTION_PARAM_Result));
	std::string contentFilePath = ot::json::getString(_document, OT_ACTION_PARAM_Info);

	// Handle file overwrite response
	if ((result & ot::MessageDialogCfg::Yes) == ot::MessageDialogCfg::Yes)
	{
		// User wants to overwrite
		handleOverwriteResponse(contentFilePath, true);
	}
	else if ((result & ot::MessageDialogCfg::No) == ot::MessageDialogCfg::No)
	{
		// User doesn't want to overwrite - add counter to filename
		handleOverwriteResponse(contentFilePath, false);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Helper

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
	auto uiComponent = Application::instance()->getUiComponent();
	ot::UILockWrapper uiLock(uiComponent, ot::LockType::ModelWrite);
	std::list<ot::GridFSFileInfo> fileInfos;
	for (const ot::ConstJsonObject& fileInfoObj : ot::json::getObjectList(_document, OT_ACTION_PARAM_FILE_Content)) {
		fileInfos.push_back(ot::GridFSFileInfo(fileInfoObj));
	}
	std::list<std::string> fileNames = ot::json::getStringList(_document, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileFilter = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mask);

	assert(fileInfos.size() == fileNames.size());
	{
		//QueuingDatabaseWritingRAII queueDatabase;
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
			std::string compressedData;
			{
				DataStorageAPI::DocumentAPI api;
				bsoncxx::oid oid_obj{ info->getDocumentId() };
				bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };
				std::vector<uint8_t> dataBuffer;
				api.GetDocumentUsingGridFs(id, info->getCollectionName(), dataBuffer);
				api.DeleteGridFSData(id, info->getCollectionName());
				compressedData = std::string(reinterpret_cast<char*>(dataBuffer.data()), dataBuffer.size());
			}
			
			uint64_t dataLen = static_cast<uint64_t>(info->getUncompressedSize());
			std::unique_ptr<uint8_t[]> data(ot::String::decompressBase64(compressedData.c_str(), dataLen));
			info++;
		
			storeFileInDataBase(std::string(reinterpret_cast<const char*>(data.get()), dataLen), fileName, folderContent, _folderName, fileFilter);
			updater.triggerUpdate(counter);
		}
		auto end = std::chrono::system_clock::now();
		uint64_t passedTime =	std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		uiComponent->displayMessage(std::to_string(passedTime) + " ms\nModel update: ");
	}
	ProgressUpdater updater(uiComponent, "Performing model update",true);
	auto start = std::chrono::system_clock::now();
	addTextFilesToModel();
	clearBuffer();
	auto end = std::chrono::system_clock::now();
	uint64_t passedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	uiComponent->displayMessage(std::to_string(passedTime) + " ms\n");
}

void FileHandler::addTextFilesToModel()
{
	QueuingHttpRequestsRAII wrapper;
	Model* model = Application::instance()->getModel();
	model->addEntitiesToModel(m_entityIDsTopo, m_entityVersionsTopo, m_forceVisible, m_entityIDsData, m_entityVersionsData, m_entityIDsTopo, "Added File", true, false, true);
}

void FileHandler::storeChangedText(ot::IVisualisationText* _entity, const std::string _text, size_t _nextChunkStartIndex)
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

void FileHandler::storeChangedTable(ot::IVisualisationTable* _entity, const ot::TableCfg& _cfg)
{
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);
	assert(_entity != nullptr);

	_entity->setTable(_cfg.createMatrix());
	model->setModified();
	model->modelChangeOperationCompleted("Updated Table.");
}

void FileHandler::processTableColumnFilterChanged(const ot::TableFilterChangeEvent& _event, ot::IVisualisationTable* _entity)
{
	assert(_entity != nullptr);

	// Store the event data
	_entity->setActiveFilters(_event.getFilterDescriptions());

	IEventHandler* eventHandler = dynamic_cast<IEventHandler*>(_entity);
	if (eventHandler != nullptr)
	{
		const std::string scriptName = eventHandler->getScriptName();
		const std::string environmentName = eventHandler->getEnvironmentName();
		auto& entityMap = Application::instance()->getModel()->getAllEntitiesByUID();
		std::optional<std::string> functionName = eventHandler->getEventHandlingFunction(PythonEventType::TableFilterChanged,entityMap);
		if (functionName.has_value())
		{
			// Otherwise there is no handler for this event in the script.
			const std::string temp = functionName.value();
			auto asyncRequest = [_event, scriptName,temp]()
				{
					try
					{
						auto uiComponent = Application::instance()->getUiComponent();
						ot::UILockWrapper uiLock(uiComponent, ot::LockType::ModelWrite);
						ot::PythonServiceInterface interface(Application::instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE)->getServiceURL());
						std::list<ot::ValueComparisonDescription> valueCompares = _event.getFilterDescriptions();
						PythonParameter parameter = PyhonParameterBuilderValueComparisons::create("", valueCompares);
						interface.addScriptWithParameter(scriptName,temp, parameter);
						ot::ReturnMessage answer = interface.sendExecutionOrder();
						if (answer.getStatus() != ot::ReturnMessage::Ok)
						{
							OT_USER_LOG_E("Failed to react on table-column-header-changed-event due to an error during the python execution: " + answer.getWhat());
						}
					}
					catch (std::exception& _e)
					{
						OT_LOG_E(_e.what());
					}

					Model* model = Application::instance()->getModel();
					assert(model != nullptr);
					model->setModified();
					model->modelChangeOperationCompleted("Updated Table Filters.");
				};

			std::thread worker(asyncRequest);
			worker.detach();
		}
		else
		{
			// No event handler function set, save state (filter have changed)

			Model* model = Application::instance()->getModel();
			assert(model != nullptr);
			model->setModified();
			model->modelChangeOperationCompleted("Updated Table Filters.");
		}
	}
	else
	{
		// No event handler found, save state (filter have changed)

		Model* model = Application::instance()->getModel();
		assert(model != nullptr);
		model->setModified();
		model->modelChangeOperationCompleted("Updated Table Filters.");
	}
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

	const std::string entityName = ot::EntityName::createUniqueEntityName(_folderName, _folderContent, name);
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

FileHandler::DialogExportEntities FileHandler::loadDialogEntities(const ot::PropertyDialogCfg& _dialogCfg, bool _isUserExport) {
	Model* model = Application::instance()->getModel();
	DialogExportEntities entities;

	// Get properties that are always present
	const ot::PropertyStringList* circuitModelProperty = dynamic_cast<const ot::PropertyStringList*>(
		_dialogCfg.getGridConfig().findPropertyByPath(m_circuitModelDialogGroup + "/Circuit Models"));
	const ot::PropertyStringList* pythonScriptProperty = dynamic_cast<const ot::PropertyStringList*>(
		_dialogCfg.getGridConfig().findPropertyByPath(m_pythonScriptDialogGroup + "/Python Script"));
	const ot::PropertyStringList* pythonManifestProperty = dynamic_cast<const ot::PropertyStringList*>(
		_dialogCfg.getGridConfig().findPropertyByPath(m_pythonScriptDialogGroup + "/Python Manifest"));

	// Determine export type
	entities.isPythonExport = (pythonScriptProperty != nullptr && !pythonScriptProperty->getCurrent().empty());
	entities.isCircuitExport = (circuitModelProperty != nullptr && !circuitModelProperty->getCurrent().empty());

	if (!entities.isPythonExport && !entities.isCircuitExport) {
		OT_LOG_D("No items selected for export.");
		return entities;
	}

	if (_isUserExport) {
		// User Library Export: Load entities and additional info from text fields
		if (entities.isCircuitExport) {
			entities.circuitModel = dynamic_cast<EntityFileText*>(loadEntityByPath(circuitModelProperty->getCurrent(), "Circuit model"));
			// For user export, meta file is not selected, so we can't load it here.
			// The additional info is handled during the creation of the UserLibraryElement.
		}
		if (entities.isPythonExport) {
			entities.pythonScript = dynamic_cast<EntityFileText*>(loadEntityByPath(pythonScriptProperty->getCurrent(), "Python script"));
			if (pythonManifestProperty && !pythonManifestProperty->getCurrent().empty()) {
				entities.pythonManifest = dynamic_cast<EntityPythonManifest*>(loadEntityByPath(pythonManifestProperty->getCurrent(), "Python manifest"));
			}
			// Meta files are not selected in this dialog.
		}
	}
	else {
		// Local Library Export: Load entities and their corresponding metadata files
		const ot::PropertyStringList* circuitModelMetaProperty = dynamic_cast<const ot::PropertyStringList*>(
			_dialogCfg.getGridConfig().findPropertyByPath(m_circuitModelDialogGroup + "/Circuit Model Metadata File"));
		const ot::PropertyStringList* pythonMetaProperty = dynamic_cast<const ot::PropertyStringList*>(
			_dialogCfg.getGridConfig().findPropertyByPath(m_pythonScriptDialogGroup + "/Python Metadata File"));
		const ot::PropertyStringList* manifestMetaProperty = dynamic_cast<const ot::PropertyStringList*>(
			_dialogCfg.getGridConfig().findPropertyByPath(m_pythonScriptDialogGroup + "/Manifest Metadata File"));

		if (entities.isCircuitExport) {
			entities.circuitModel = dynamic_cast<EntityFileText*>(loadEntityByPath(circuitModelProperty->getCurrent(), "Circuit model"));
			entities.circuitMetaFile = dynamic_cast<EntityFileText*>(loadEntityByPath(circuitModelMetaProperty->getCurrent(), "Circuit model metadata"));
			if (!entities.circuitModel || !entities.circuitMetaFile || !validateMetaDataFile(entities.circuitMetaFile)) {
				entities.isCircuitExport = false; 
			}
		}
		if (entities.isPythonExport) {
			entities.pythonScript = dynamic_cast<EntityFileText*>(loadEntityByPath(pythonScriptProperty->getCurrent(), "Python script"));
			entities.pythonMetaFile = dynamic_cast<EntityFileText*>(loadEntityByPath(pythonMetaProperty->getCurrent(), "Python metadata"));
			if (!entities.pythonScript || !entities.pythonMetaFile || !validateMetaDataFile(entities.pythonMetaFile)) {
				entities.isPythonExport = false; 
			}

			if (pythonManifestProperty && !pythonManifestProperty->getCurrent().empty()) {
				entities.pythonManifest = dynamic_cast<EntityPythonManifest*>(loadEntityByPath(pythonManifestProperty->getCurrent(), "Python manifest"));
				if (manifestMetaProperty && !manifestMetaProperty->getCurrent().empty()) {
					entities.manifestMetaFile = dynamic_cast<EntityFileText*>(loadEntityByPath(manifestMetaProperty->getCurrent(), "Manifest metadata"));
					if (!entities.pythonManifest || !entities.manifestMetaFile || !validateMetaDataFile(entities.manifestMetaFile)) {
						entities.pythonManifest = nullptr;
						entities.manifestMetaFile = nullptr;
					}
				}
			}
		}
	}

	return entities;
}

EntityBase* FileHandler::loadEntityByPath(const std::string& _entityPath, const std::string& _description) {
	if (_entityPath.empty()) {
		OT_LOG_E(_description + " path is empty.");
		return nullptr;
	}

	Model* model = Application::instance()->getModel();
	EntityBase* entity = model->findEntityFromName(_entityPath);

	if (!entity) {
		OT_LOG_E(_description + " entity not found for path: " + _entityPath);
		return nullptr;
	}

	return entity;
}

void FileHandler::handleCircuitExport(const DialogExportEntities& _entities, bool _exportToUserLibrary, const ot::PropertyDialogCfg& _dialogCfg) {
	if (_exportToUserLibrary) {
		try {
			std::list<ot::UserLibraryElement> elementsToExport;
			createLibraryElementsForCircuitModel(_entities.circuitModel, _dialogCfg, elementsToExport);
			exportLibraryElementsToUserLibrary(elementsToExport);
		}
		catch (const std::exception& _e) {
			OT_LOG_E(std::string("Circuit model export to user library failed: ") + _e.what());
			Application::instance()->getModel()->reportError("Circuit model export failed: " + std::string(_e.what()));
		}
	}
	else {
		// Export to Library (async)
		std::thread worker([this, circuitModel = _entities.circuitModel, circuitMetaFile = _entities.circuitMetaFile]() {
			try {
				auto uiComponent = Application::instance()->getUiComponent();
				ot::UILockWrapper uiLock(uiComponent, ot::LockType::ModelWrite);

				ProgressUpdater updater(uiComponent, "Exporting circuit model to library", false);
				updater.setTotalNumberOfSteps(1);

				std::string libDataPath = getLibraryDataPath();
				if (!libDataPath.empty()) {
					exportCircuitModel(circuitModel, circuitMetaFile, libDataPath);
					updater.triggerUpdate(1);
					uiComponent->displayMessage("Circuit model successfully exported to library.\n");
				}
			}
			catch (const std::exception& _e) {
				OT_LOG_E(std::string("Circuit model export failed: ") + _e.what());
			}
			});
		worker.detach();
	}
}


void FileHandler::handlePythonExport(const DialogExportEntities& _entities, bool _exportToUserLibrary, const ot::PropertyDialogCfg& _dialogCfg) {
	bool hasFullPythonExport = (_entities.pythonManifest != nullptr);

	if (_exportToUserLibrary) {
		try {
			std::list<ot::UserLibraryElement> elementsToExport;

			if (hasFullPythonExport) {
				createLibraryElementsForPythonManifest(_entities.pythonManifest, _dialogCfg, elementsToExport);
			}

			ot::UID pythonEnvironmentID = hasFullPythonExport ? _entities.pythonManifest->getManifestID() : ot::invalidUID;
			createLibraryElementsForPythonScript(_entities.pythonScript, pythonEnvironmentID, _dialogCfg, elementsToExport);

			exportLibraryElementsToUserLibrary(elementsToExport);
		}
		catch (const std::exception& _e) {
			OT_LOG_E(std::string("Python export to user library failed: ") + _e.what());
			Application::instance()->getModel()->reportError("Python export failed: " + std::string(_e.what()));
		}
	}
	else {
		// Export to Library (async)
		std::thread worker([this, pythonScript = _entities.pythonScript, pythonManifest = _entities.pythonManifest,
			pythonMetaFile = _entities.pythonMetaFile, manifestMetaFile = _entities.manifestMetaFile, hasFullPythonExport]() {
				if (hasFullPythonExport) {
					exportPythonScriptsToLibrary(
						pythonScript->getEntityID(),
						pythonManifest->getEntityID(),
						pythonMetaFile->getEntityID(),
						manifestMetaFile->getEntityID(),
						pythonManifest->getManifestID()
					);
				}
				else {
					exportPythonScriptsToLibrary(
						pythonScript->getEntityID(),
						ot::invalidUID,
						pythonMetaFile->getEntityID(),
						ot::invalidUID,
						ot::invalidUID
					);
				}
			});
		worker.detach();
	}
}

bool FileHandler::validateMetaDataFile(EntityFileText* _metaFile) {
	Model* model = Application::instance()->getModel();

	if (_metaFile == nullptr) {
		model->reportError("Metadata file is null.");
		return false;
	}

	std::string metaContent = _metaFile->getText();
	ot::JsonDocument metaDoc;

	try {
		metaDoc.fromJson(metaContent);
	}
	catch (const std::exception& _e) {
		model->reportError(
			"Metadata file \"" + _metaFile->getName() + "\" is not valid JSON: " + std::string(_e.what())
		);
		return false;
	}

	// Validate mandatory fields
	std::list<std::string> mandatoryFields = {"MetaData", "AdditionalInfos" };
	for (const auto& field : mandatoryFields) {
		if (!ot::json::exists(metaDoc, field)) {
			model->reportError(
				"Metadata file \"" + _metaFile->getName() + "\" is missing mandatory field: \"" + field + "\""
			);
			return false;
		}
	}

	if (!ot::json::isObject(metaDoc, "MetaData") || !ot::json::isObject(metaDoc, "AdditionalInfos")) {
		model->reportError(
			"Metadata file \"" + _metaFile->getName() + "\" has invalid field types. 'MetaData' and 'AdditionalInfos' must be objects."
		);
		return false;
	}

	return true;
}

std::string FileHandler::getLibraryDataPath() const {
	std::string path = ot::OperatingSystem::getEnvironmentVariableString("OPENTWIN_DEV_ROOT");

	if (!path.empty())
	{
		path += "/LibraryData";
	}

	return path;
}

bool FileHandler::ensureDirectoryExists(const std::string& _path) const {
	try {
		std::filesystem::create_directories(_path);
		return true;
	}
	catch (const std::exception& _e) {
		OT_LOG_E("Failed to create directory \"" + _path + "\": " + _e.what());
		return false;
	}
}

void FileHandler::writeFileToPath(const std::string& _filePath, const std::string& _content) const {
	std::ofstream outFile(_filePath, std::ios::binary);
	if (!outFile.is_open()) {
		throw std::runtime_error("Could not open file for writing: " + _filePath);
	}

	outFile.write(_content.data(), _content.size());
	if (!outFile) {
		throw std::runtime_error("Error writing to file: " + _filePath);
	}

	outFile.close();
}

std::string FileHandler::ensureFileExtension(const std::string& _fileName, const std::string& _extension) const {
	if (_fileName.size() >= _extension.size()) {
		std::string fileEnd = _fileName.substr(_fileName.size() - _extension.size());
		if (fileEnd == _extension) {
			return _fileName; 
		}
	}
	return _fileName + _extension;
}

void FileHandler::exportPythonScriptsToLibrary(ot::UID _scriptID, ot::UID _manifestID, ot::UID _pythonMetaID, ot::UID _manifestMetaID, ot::UID _environmentID) {
	auto uiComponent = Application::instance()->getUiComponent();
	ot::UILockWrapper uiLock(uiComponent, ot::LockType::ModelWrite);

	try {
		Model* model = Application::instance()->getModel();
		assert(model != nullptr);

		// Retrieve script and metadata entities (always required)
		EntityBase* scriptEntityBase = model->getEntityByID(_scriptID);
		EntityBase* pythonMetaEntityBase = model->getEntityByID(_pythonMetaID);

		if (scriptEntityBase == nullptr || pythonMetaEntityBase == nullptr) {
			OT_LOG_E("Error: Python script or metadata entity could not be retrieved.");
			return;
		}

		EntityFileText* scriptEntity = dynamic_cast<EntityFileText*>(scriptEntityBase);
		EntityFileText* pythonMetaEntity = dynamic_cast<EntityFileText*>(pythonMetaEntityBase);

		if (scriptEntity == nullptr || pythonMetaEntity == nullptr) {
			OT_LOG_E("Error: Failed to cast script or metadata entity.");
			return;
		}

		// Retrieve optional manifest entities
		EntityPythonManifest* manifestEntity = nullptr;
		EntityFileText* manifestMetaEntity = nullptr;

		if (_manifestID != ot::invalidUID) {
			EntityBase* manifestEntityBase = model->getEntityByID(_manifestID);
			if (manifestEntityBase != nullptr) {
				manifestEntity = dynamic_cast<EntityPythonManifest*>(manifestEntityBase);
			}
		}

		if (_manifestMetaID != ot::invalidUID) {
			EntityBase* manifestMetaEntityBase = model->getEntityByID(_manifestMetaID);
			if (manifestMetaEntityBase != nullptr) {
				manifestMetaEntity = dynamic_cast<EntityFileText*>(manifestMetaEntityBase);
			}
		}

		// Get base export path
		std::string libDataPath = getLibraryDataPath();
		if (libDataPath.empty()) {
			OT_LOG_E("Error: Could not determine OpenTwinDevRoot path.");
			return;
		}

		// Determine number of steps based on what we're exporting
		int totalSteps = 1; // At minimum, we export the script
		if (manifestEntity != nullptr && manifestMetaEntity != nullptr) {
			totalSteps = 2; // Export both manifest and script
		}

		ProgressUpdater updater(uiComponent, "Exporting files to library", false);
		updater.setTotalNumberOfSteps(totalSteps);

		// Export manifest if available
		if (manifestEntity != nullptr && manifestMetaEntity != nullptr) {
			exportPythonManifest(manifestEntity, manifestMetaEntity, libDataPath, _environmentID);
			updater.triggerUpdate(1);
		}

		// Export script
		exportPythonScript(scriptEntity, pythonMetaEntity, libDataPath, _environmentID);
		updater.triggerUpdate(totalSteps);

		uiComponent->displayMessage("Files successfully exported to library.\n");
	}
	catch (const std::exception& _e) {
		OT_LOG_E(std::string("Export failed: ") + _e.what());
	}
}

void FileHandler::exportPythonManifest(EntityPythonManifest* _manifestEntity, EntityFileText* _metaEntity, const std::string& _basePath, ot::UID _environmentID) {
	assert(_manifestEntity != nullptr && _metaEntity != nullptr);

	// Create PythonEnvironments directory
	std::string environmentPath = _basePath + "/PythonEnvironments";
	if (!ensureDirectoryExists(environmentPath)) {
		throw std::runtime_error("Could not create PythonEnvironments directory");
	}

	// Read and validate metadata
	std::string metaContent = _metaEntity->getText();
	ot::JsonDocument metaDoc;
	metaDoc.fromJson(metaContent);

	// Export manifest .txt file
	std::string manifestFileName = environmentPath + "/" + ensureFileExtension(_manifestEntity->getNameOnly(), ".txt");
	std::string manifestContent = _manifestEntity->getText();

	// Export metadata .otmeta.json file (WITHOUT dynamic parameters yet)
	std::string metaFileName = environmentPath + "/" + ensureFileExtension(_metaEntity->getNameOnly(), ".otmeta.json");
	std::string metaJson = ot::json::toJson(metaDoc);

	FileOverwriteStatus status = checkAndHandleFileOverwrite(manifestFileName, manifestContent, metaFileName, metaJson);

	// Check if files exist and if content changed (only checks, no prompt yet)
	if (status == FileOverwriteStatus::Write)
	{
		// File check passed - NOW add the dynamic parameters
		metaDoc.RemoveMember("LibraryElementID");
		metaDoc.AddMember("LibraryElementID", ot::JsonValue(_manifestEntity->getManifestID()), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Version");
		metaDoc.AddMember("Version", ot::JsonValue(1), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Name");
		metaDoc.AddMember("Name", ot::JsonString(_manifestEntity->getNameOnly(), metaDoc.GetAllocator()), metaDoc.GetAllocator());
		metaDoc.RemoveMember("FileName");
		metaDoc.AddMember("FileName", ot::JsonString(ensureFileExtension(_manifestEntity->getNameOnly(), ".txt"), metaDoc.GetAllocator()), metaDoc.GetAllocator());

		// Update metaJson with dynamic parameters
		metaJson = ot::json::toJson(metaDoc);

		writeFileToPath(manifestFileName, manifestContent);
		writeFileToPath(metaFileName, metaJson);
	}
	else if(status == FileOverwriteStatus::PromptUser)
	{
		// File check indicated changes - NOW add dynamic parameters and prompt
		metaDoc.RemoveMember("LibraryElementID");
		metaDoc.AddMember("LibraryElementID", ot::JsonValue(_manifestEntity->getManifestID()), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Version");
		metaDoc.AddMember("Version", ot::JsonValue(1), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Name");
		metaDoc.AddMember("Name", ot::JsonString(_manifestEntity->getNameOnly(), metaDoc.GetAllocator()), metaDoc.GetAllocator());
		metaDoc.RemoveMember("FileName");
		metaDoc.AddMember("FileName", ot::JsonString(ensureFileExtension(_manifestEntity->getNameOnly(), ".txt"), metaDoc.GetAllocator()), metaDoc.GetAllocator());

		// Update metaJson with dynamic parameters
		metaJson = ot::json::toJson(metaDoc);

		// NOW prompt with the updated content
		promptUserForOverwrite(manifestFileName, metaFileName, manifestContent, metaJson);
	}
	else if (status == FileOverwriteStatus::Skip) 
	{
		Application::instance()->getUiComponent()->displayMessage("Element already exists. Export skipped for \"" + _manifestEntity->getNameOnly() + "\".\n");
		return;
	}
}

void FileHandler::exportPythonScript(EntityFileText* _scriptEntity, EntityFileText* _metaEntity, const std::string& _basePath, ot::UID _environmentID) {
	assert(_scriptEntity != nullptr && _metaEntity != nullptr);

	// Create PythonScripts directory
	std::string scriptPath = _basePath + "/PythonScripts";
	if (!ensureDirectoryExists(scriptPath)) {
		throw std::runtime_error("Could not create PythonScripts directory");
	}

	// Read and validate metadata
	std::string metaContent = _metaEntity->getText();
	ot::JsonDocument metaDoc;
	metaDoc.fromJson(metaContent);

	// Export script .py file
	std::string scriptFileName = scriptPath + "/" + ensureFileExtension(_scriptEntity->getNameOnly(), ".py");
	std::string scriptContent = _scriptEntity->getText();

	// Export metadata .otmeta.json file (WITHOUT dynamic parameters yet)
	std::string metaFileName = scriptPath + "/" + ensureFileExtension(_metaEntity->getNameOnly(), ".otmeta.json");
	std::string metaJson = ot::json::toJson(metaDoc);

	FileOverwriteStatus status = checkAndHandleFileOverwrite(scriptFileName, scriptContent, metaFileName, metaJson);
	// Check if files exist and if content changed (only checks, no prompt yet!)
	if (status == FileOverwriteStatus::Write) {
		// File check passed - NOW add the dynamic parameters
		metaDoc.RemoveMember("LibraryElementID");
		metaDoc.AddMember("LibraryElementID", ot::JsonValue(_scriptEntity->getEntityID()), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Version");
		metaDoc.AddMember("Version", ot::JsonValue(1), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Name");
		metaDoc.AddMember("Name", ot::JsonString(_scriptEntity->getNameOnly(), metaDoc.GetAllocator()), metaDoc.GetAllocator());
		metaDoc.RemoveMember("FileName");
		metaDoc.AddMember("FileName", ot::JsonString(ensureFileExtension(_scriptEntity->getNameOnly(), ".py"), metaDoc.GetAllocator()), metaDoc.GetAllocator());

		// Update AdditionalInfos with dependency information
		if (metaDoc.HasMember("AdditionalInfos") && metaDoc["AdditionalInfos"].IsObject()) {
			metaDoc["AdditionalInfos"].RemoveMember("DependencyID");
			metaDoc["AdditionalInfos"].RemoveMember("DependencyCollection");
			metaDoc["AdditionalInfos"].AddMember("DependencyID", ot::JsonString(std::to_string(_environmentID), metaDoc.GetAllocator()), metaDoc.GetAllocator());
			metaDoc["AdditionalInfos"].AddMember("DependencyCollection", ot::JsonString("PythonEnvironments", metaDoc.GetAllocator()), metaDoc.GetAllocator());
		}

		// Update metaJson with dynamic parameters
		metaJson = ot::json::toJson(metaDoc);

		writeFileToPath(scriptFileName, scriptContent);
		writeFileToPath(metaFileName, metaJson);
	}
	else if (status == FileOverwriteStatus::PromptUser) {
		// File check indicated changes - NOW add dynamic parameters and prompt
		metaDoc.RemoveMember("LibraryElementID");
		metaDoc.AddMember("LibraryElementID", ot::JsonValue(_scriptEntity->getEntityID()), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Version");
		metaDoc.AddMember("Version", ot::JsonValue(1), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Name");
		metaDoc.AddMember("Name", ot::JsonString(_scriptEntity->getNameOnly(), metaDoc.GetAllocator()), metaDoc.GetAllocator());
		metaDoc.RemoveMember("FileName");
		metaDoc.AddMember("FileName", ot::JsonString(ensureFileExtension(_scriptEntity->getNameOnly(), ".py"), metaDoc.GetAllocator()), metaDoc.GetAllocator());

		// Update AdditionalInfos with dependency information
		if (metaDoc.HasMember("AdditionalInfos") && metaDoc["AdditionalInfos"].IsObject()) {
			metaDoc["AdditionalInfos"].RemoveMember("DependencyID");
			metaDoc["AdditionalInfos"].RemoveMember("DependencyCollection");
			metaDoc["AdditionalInfos"].AddMember("DependencyID", ot::JsonString(std::to_string(_environmentID), metaDoc.GetAllocator()), metaDoc.GetAllocator());
			metaDoc["AdditionalInfos"].AddMember("DependencyCollection", ot::JsonString("PythonEnvironments", metaDoc.GetAllocator()), metaDoc.GetAllocator());
		}

		// Update metaJson with dynamic parameters
		metaJson = ot::json::toJson(metaDoc);

		// NOW prompt with the updated content
		promptUserForOverwrite(scriptFileName, metaFileName, scriptContent, metaJson);
	} 
	else if(status == FileOverwriteStatus::Skip)
	{
		Application::instance()->getUiComponent()->displayMessage("Element already exists. Export skipped for \"" + _scriptEntity->getNameOnly() + "\".\n");
		return;
	}
}

void FileHandler::exportCircuitModel(EntityFileText* _modelEntity, EntityFileText* _metaEntity, const std::string& _basePath) {
	assert(_modelEntity != nullptr && _metaEntity != nullptr);

	// Create CircuitData directory
	std::string modelPath = _basePath + "/CircuitData";
	if (!ensureDirectoryExists(modelPath)) {
		throw std::runtime_error("Could not create CircuitData directory");
	}

	// Read and validate metadata
	std::string metaContent = _metaEntity->getText();
	ot::JsonDocument metaDoc;
	metaDoc.fromJson(metaContent);

	// Export circuit model file
	std::string modelFileName = modelPath + "/" + ensureFileExtension(_modelEntity->getNameOnly(), ".txt");
	std::string modelContent = _modelEntity->getText();

	// Export metadata .otmeta.json file
	std::string metaFileName = modelPath + "/" + ensureFileExtension(_metaEntity->getNameOnly(), ".otmeta.json");
	std::string metaJson = ot::json::toJson(metaDoc);

	FileOverwriteStatus status = checkAndHandleFileOverwrite(modelFileName, modelContent, metaFileName, metaJson);

	// Check if files exist and if content changed (only checks, no prompt yet!)
	if (status == FileOverwriteStatus::Write) {
		// File check passed - NOW add the dynamic parameters
		metaDoc.RemoveMember("LibraryElementID");
		metaDoc.AddMember("LibraryElementID", ot::JsonValue(_modelEntity->getEntityID()), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Version");
		metaDoc.AddMember("Version", ot::JsonValue(1), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Name");
		metaDoc.AddMember("Name", ot::JsonString(_modelEntity->getNameOnly(), metaDoc.GetAllocator()), metaDoc.GetAllocator());
		metaDoc.RemoveMember("FileName");
		metaDoc.AddMember("FileName", ot::JsonString(ensureFileExtension(_modelEntity->getNameOnly(), ".txt"), metaDoc.GetAllocator()), metaDoc.GetAllocator());

		// Update metaJson with dynamic parameters
		metaJson = ot::json::toJson(metaDoc);

		writeFileToPath(modelFileName, modelContent);
		writeFileToPath(metaFileName, metaJson);
	}
	else if (status == FileOverwriteStatus::PromptUser) {
		// File check indicated changes - NOW add dynamic parameters and prompt
		metaDoc.RemoveMember("LibraryElementID");
		metaDoc.AddMember("LibraryElementID", ot::JsonValue(_modelEntity->getEntityID()), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Version");
		metaDoc.AddMember("Version", ot::JsonValue(1), metaDoc.GetAllocator());

		metaDoc.RemoveMember("Name");
		metaDoc.AddMember("Name", ot::JsonString(_modelEntity->getNameOnly(), metaDoc.GetAllocator()), metaDoc.GetAllocator());
		metaDoc.RemoveMember("FileName");
		metaDoc.AddMember("FileName", ot::JsonString(ensureFileExtension(_modelEntity->getNameOnly(), ".txt"), metaDoc.GetAllocator()), metaDoc.GetAllocator());

		// Update metaJson with dynamic parameters
		metaJson = ot::json::toJson(metaDoc);

		// NOW prompt with the updated content
		promptUserForOverwrite(modelFileName, metaFileName, modelContent, metaJson);
	}
	else if (status == FileOverwriteStatus::Skip) {
		Application::instance()->getUiComponent()->displayMessage("Element already exists. Export skipped for \"" + _modelEntity->getNameOnly() + "\".\n");
		return;
	}
}

void FileHandler::clearBuffer() {
	m_entityVersionsData.clear();
	m_entityVersionsTopo.clear();
	m_entityIDsData.clear();
	m_entityIDsTopo.clear();
	m_forceVisible.clear();
}

FileHandler::FileOverwriteStatus FileHandler::checkAndHandleFileOverwrite(const std::string& _filePath, const std::string& _newContent,
	const std::string& _metaFilePath, const std::string& _metaNewContent) const {

	// Check if content file exists
	std::ifstream contentFile(_filePath, std::ios::binary | std::ios::ate);
	bool contentFileExists = contentFile.is_open();

	// Check if meta file exists
	std::ifstream metaFile(_metaFilePath, std::ios::binary | std::ios::ate);
	bool metaFileExists = metaFile.is_open();

	bool contentChanged = false;
	bool metaChanged = false;

	// Check if content has changed
	if (contentFileExists) {
		std::ifstream::pos_type fileSize = contentFile.tellg();
		contentFile.seekg(0, std::ios::beg);

		if (static_cast<size_t>(fileSize) != _newContent.size()) {
			contentChanged = true;
		}
		else {
			std::vector<char> fileContent(fileSize);
			contentFile.read(fileContent.data(), fileSize);
			contentFile.close();

			if (!std::equal(fileContent.begin(), fileContent.end(), _newContent.begin())) {
				contentChanged = true;
			}
		}
	}
	else {
		contentChanged = false;
	}

	// Check if metadata has changed - ONLY compare MetaData and AdditionalInfos
	if (metaFileExists) {
		std::ifstream::pos_type metaSize = metaFile.tellg();
		metaFile.seekg(0, std::ios::beg);

		std::vector<char> metaFileContent(metaSize);
		metaFile.read(metaFileContent.data(), metaSize);
		metaFile.close();

		std::string existingMetaContent(metaFileContent.begin(), metaFileContent.end());

		// Parse both metadata JSON documents
		ot::JsonDocument newMetaDoc;
		ot::JsonDocument existingMetaDoc;

		try {
			newMetaDoc.fromJson(_metaNewContent);
			existingMetaDoc.fromJson(existingMetaContent);
		}
		catch (const std::exception& _e) {
			OT_LOG_W("Failed to parse metadata JSON for comparison: " + std::string(_e.what()));
			// If parsing fails, fall back to full content comparison
			metaChanged = !std::equal(metaFileContent.begin(), metaFileContent.end(), _metaNewContent.begin());
		}

		// Extract and compare only MetaData and AdditionalInfos
		if (!metaChanged) {
			try {
				// Extract MetaData objects and convert directly to JSON strings
				std::string newMetaDataStr = ot::json::toJson(ot::json::getObject(newMetaDoc, "MetaData"));
				std::string existingMetaDataStr = ot::json::toJson(ot::json::getObject(existingMetaDoc, "MetaData"));

				// Extract AdditionalInfos objects
				ot::ConstJsonObject newAdditionalInfos = ot::json::getObject(newMetaDoc, "AdditionalInfos");
				ot::ConstJsonObject existingAdditionalInfos = ot::json::getObject(existingMetaDoc, "AdditionalInfos");

				// Create copies to remove dynamic fields
				ot::JsonDocument newAdditionalInfosClean;
				ot::JsonDocument existingAdditionalInfosClean;

				// Copy all fields except DependencyID and DependencyCollection
				for (auto it = newAdditionalInfos.MemberBegin(); it != newAdditionalInfos.MemberEnd(); ++it) {
					const std::string memberName = it->name.GetString();
					if (memberName != "DependencyID" && memberName != "DependencyCollection") {
						newAdditionalInfosClean.AddMember(
							ot::JsonString(memberName, newAdditionalInfosClean.GetAllocator()),
							ot::JsonValue(it->value, newAdditionalInfosClean.GetAllocator()),
							newAdditionalInfosClean.GetAllocator()
						);
					}
				}

				for (auto it = existingAdditionalInfos.MemberBegin(); it != existingAdditionalInfos.MemberEnd(); ++it) {
					const std::string memberName = it->name.GetString();
					if (memberName != "DependencyID" && memberName != "DependencyCollection") {
						existingAdditionalInfosClean.AddMember(
							ot::JsonString(memberName, existingAdditionalInfosClean.GetAllocator()),
							ot::JsonValue(it->value, existingAdditionalInfosClean.GetAllocator()),
							existingAdditionalInfosClean.GetAllocator()
						);
					}
				}

				// Convert cleaned objects to JSON strings for comparison
				std::string newAdditionalInfosStr = ot::json::toJson(newAdditionalInfosClean);
				std::string existingAdditionalInfosStr = ot::json::toJson(existingAdditionalInfosClean);

				// Only consider metadata changed if MetaData or AdditionalInfos differ
				metaChanged = (newMetaDataStr != existingMetaDataStr) || (newAdditionalInfosStr != existingAdditionalInfosStr);
			}
			catch (const std::exception& _e) {
				OT_LOG_W("Failed to extract MetaData or AdditionalInfos for comparison: " + std::string(_e.what()));
				// If extraction fails, fall back to full content comparison
				metaChanged = !std::equal(metaFileContent.begin(), metaFileContent.end(), _metaNewContent.begin());
			}
		}
	}
	else {
		metaChanged = false;
	}

	// If neither file exists, write both files
	if (!contentFileExists && !metaFileExists) {
		OT_LOG_D("Creating new files: \"" + _filePath + "\" and \"" + _metaFilePath + "\"");
		return FileOverwriteStatus::Write;
	}

	// Both files exist and content is identical - skip without any action
	if (contentFileExists && metaFileExists && !contentChanged && !metaChanged) {
		OT_LOG_D("Skipping file export: content and metadata are identical for \"" + _filePath + "\"");
		return FileOverwriteStatus::Skip;
	}

	// If either has changed, prompt user
	if (contentChanged || metaChanged) {
		OT_LOG_D("File changes detected - prompt required for \"" + _filePath + "\"");
		return FileOverwriteStatus::PromptUser;
	}

	// One file exists, the other doesn't - treat as changed and prompt
	OT_LOG_D("File mismatch detected - one file exists, other doesn't for \"" + _filePath + "\"");
	return FileOverwriteStatus::PromptUser;
}

void FileHandler::promptUserForOverwrite(const std::string& _contentFilePath, const std::string& _metaFilePath,
	const std::string& _contentNewContent, const std::string& _metaNewContent) const {
	
	// Store the pending file overwrites (both content and meta together)
	const_cast<FileHandler*>(this)->m_pendingFileOverwrites[_contentFilePath] = {
		_contentFilePath,
		_contentNewContent,
		_metaFilePath,
		_metaNewContent
	};

	if (!ot::Frontend::promptChoice(
		std::string(c_promptActionOverwriteFile),
		"OpenTwin",
		"The files for \"" + _contentFilePath + "\" have changed.\n\nDo you want to overwrite both the content and metadata files?",
		ot::MessageDialogCfg::Question,
		ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No,
		_contentFilePath
	))
	{
		OT_LOG_E("Failed to send prompt to frontend");
	}
}

std::string FileHandler::createIncrementedPath(const std::string& _filePath) {
	size_t lastSlash = _filePath.find_last_of("/\\");

	std::string directory = (lastSlash != std::string::npos) ? _filePath.substr(0, lastSlash + 1) : "";
	std::string filename = (lastSlash != std::string::npos) ? _filePath.substr(lastSlash + 1) : _filePath;

	std::string baseName;
	std::string extension;

	size_t dotInFilename = filename.find_last_of('.');
	if (dotInFilename != std::string::npos) {
		baseName = filename.substr(0, dotInFilename);
		extension = filename.substr(dotInFilename);
	}
	else {
		baseName = filename;
		extension = "";
	}

	// Special handling for .otmeta.json files - put counter before .otmeta.json
	if (extension == ".json" && baseName.size() >= 6) {
		std::string baseSuffix = baseName.substr(baseName.size() - 6);
		if (baseSuffix == ".otmeta") {
			baseName = baseName.substr(0, baseName.size() - 6); // Remove ".otmeta"
			extension = ".otmeta.json";
		}
	}

	// Find a unique filename with counter
	std::string newFilePath;
	int counter = 1;
	do {
		newFilePath = directory + baseName + "_" + std::to_string(counter) + extension;
		counter++;
	} while (std::filesystem::exists(newFilePath));

	return newFilePath;
}

void FileHandler::handleOverwriteResponse(const std::string& _filePath, bool _overwrite) {
	auto it = m_pendingFileOverwrites.find(_filePath);
	if (it == m_pendingFileOverwrites.end()) {
		OT_LOG_E("No pending file overwrite found for \"" + _filePath + "\"");
		return;
	}

	const PendingFileOverwrite& pending = it->second;

	if (_overwrite) {
		// Parse the metadata JSON to increment the version
		std::string metaContent = pending.metaNewContent;
		ot::JsonDocument metaDoc;
		try {
			metaDoc.fromJson(metaContent);

			// Increment the version
			int currentVersion = 1;
			if (ot::json::exists(metaDoc, "Version") && ot::json::isInt(metaDoc, "Version")) {
				currentVersion = ot::json::getInt(metaDoc, "Version");
			}

			// Remove and update with incremented version
			metaDoc.RemoveMember("Version");
			metaDoc.AddMember("Version", ot::JsonValue(currentVersion + 1), metaDoc.GetAllocator());

			// Update metaContent with the new version
			metaContent = ot::json::toJson(metaDoc);
		}
		catch (const std::exception& _e) {
			OT_LOG_W("Failed to increment version in metadata: " + std::string(_e.what()));
			// Continue with original metadata if version increment fails
		}

		writeFileToPath(pending.contentFilePath, pending.contentNewContent);
		writeFileToPath(pending.metaFilePath, metaContent);
		OT_LOG_D("Files overwritten with incremented version: \"" + pending.contentFilePath + "\" and \"" + pending.metaFilePath + "\"");
	}
	else {
		// Don't overwrite - add counter to both filenames
		std::string newContentPath = createIncrementedPath(pending.contentFilePath);
		std::string newMetaPath = createIncrementedPath(pending.metaFilePath);

		// Write both files with new names
		writeFileToPath(newContentPath, pending.contentNewContent);
		writeFileToPath(newMetaPath, pending.metaNewContent);
		OT_LOG_D("Files written with new names: \"" + newContentPath + "\" and \"" + newMetaPath + "\"");
	}

	// Remove from pending list
	m_pendingFileOverwrites.erase(it);
}

void FileHandler::handleExportFileDialog(const ot::PropertyDialogCfg& _dialogCfg, bool _exportToUserLibrary)
{
	DialogExportEntities entities = loadDialogEntities(_dialogCfg, _exportToUserLibrary);
	if (!entities.isCircuitExport && !entities.isPythonExport) {
		return;
	}

	if (entities.isCircuitExport) {
		handleCircuitExport(entities, _exportToUserLibrary, _dialogCfg);
	}
	else if (entities.isPythonExport) {
		handlePythonExport(entities, _exportToUserLibrary, _dialogCfg);
	}
}

void FileHandler::createLibraryElementsForCircuitModel(EntityFileText* _modelEntity, const ot::PropertyDialogCfg& _dialogCfg, std::list<ot::UserLibraryElement>& _elementsToExport) {
	assert(_modelEntity != nullptr);

	// Create LibraryElement
	ot::UserLibraryElement libraryElement;
	libraryElement.setName(_modelEntity->getNameOnly());
	libraryElement.setFileName(_modelEntity->getNameOnly() + ".txt");
	libraryElement.setVersion(1);
	libraryElement.setData(_modelEntity->getText());
	libraryElement.setLibraryElementID(_modelEntity->getEntityID());
	libraryElement.setCollectionName(m_circuitModelUserCollectionName);
	libraryElement.setOwner(Application::instance()->getLogInUserName());

	// Extract additional info and description from the dialog config
	const ot::PropertyString* circuitAdditionalInfoProp = dynamic_cast<const ot::PropertyString*>(
		_dialogCfg.getGridConfig().findPropertyByPath(m_circuitModelDialogGroup + "/Circuit Model Additional Info"));
	const ot::PropertyString* circuitDescriptionProp = dynamic_cast<const ot::PropertyString*>(
		_dialogCfg.getGridConfig().findPropertyByPath(m_circuitModelDialogGroup + "/Circuit Model Description"));

	if (circuitAdditionalInfoProp) {
		libraryElement.addAdditionalInfo("Info", circuitAdditionalInfoProp->getValue());
	}
	if (circuitDescriptionProp) {
		libraryElement.addMetaData("Description", circuitDescriptionProp->getValue());
	}

	_elementsToExport.push_back(libraryElement);
}

void FileHandler::createLibraryElementsForPythonScript(EntityFileText* _scriptEntity, ot::UID _environmentID, const ot::PropertyDialogCfg& _dialogCfg, std::list<ot::UserLibraryElement>& _elementsToExport) {
	assert(_scriptEntity != nullptr);

	// Create LibraryElement
	ot::UserLibraryElement libraryElement;
	libraryElement.setName(_scriptEntity->getNameOnly());
	libraryElement.setFileName(_scriptEntity->getNameOnly() + ".py");
	libraryElement.setVersion(1);
	libraryElement.setData(_scriptEntity->getText());
	libraryElement.setLibraryElementID(_scriptEntity->getEntityID());
	libraryElement.setCollectionName(m_pythonUserCollectionName);
	libraryElement.setOwner(Application::instance()->getLogInUserName());

	// Extract additional info and description from the dialog config
	const ot::PropertyString* pythonAdditionalInfoProp = dynamic_cast<const ot::PropertyString*>(
		_dialogCfg.getGridConfig().findPropertyByPath(m_pythonScriptDialogGroup + "/Python Script Additional Info"));
	const ot::PropertyString* pythonDescriptionProp = dynamic_cast<const ot::PropertyString*>(
		_dialogCfg.getGridConfig().findPropertyByPath(m_pythonScriptDialogGroup + "/Python Script Description"));

	if (pythonAdditionalInfoProp) {
		libraryElement.addAdditionalInfo("Info", pythonAdditionalInfoProp->getValue());
	}
	if (pythonDescriptionProp) {
		libraryElement.addMetaData("Description", pythonDescriptionProp->getValue());
	}

	// Add DependencyID and DependencyCollection if environment ID is valid
	if (_environmentID != ot::invalidUID) {
		libraryElement.addAdditionalInfo("DependencyID", std::to_string(_environmentID));
		libraryElement.addAdditionalInfo("DependencyCollection", m_environmentUserCollectionName);
	}

	_elementsToExport.push_back(libraryElement);
}

void FileHandler::createLibraryElementsForPythonManifest(EntityPythonManifest* _manifestEntity, const ot::PropertyDialogCfg& _dialogCfg, std::list<ot::UserLibraryElement>& _elementsToExport) {
	assert(_manifestEntity != nullptr);

	// Create LibraryElement
	ot::UserLibraryElement libraryElement;
	libraryElement.setName(_manifestEntity->getNameOnly());
	libraryElement.setFileName(_manifestEntity->getNameOnly() + ".txt");
	libraryElement.setVersion(1);
	libraryElement.setData(_manifestEntity->getText());
	libraryElement.setLibraryElementID(_manifestEntity->getManifestID());
	libraryElement.setCollectionName(m_environmentUserCollectionName);
	libraryElement.setOwner(Application::instance()->getLogInUserName());

	// Extract additional info and description from the dialog config
	const ot::PropertyString* manifestAdditionalInfoProp = dynamic_cast<const ot::PropertyString*>(
		_dialogCfg.getGridConfig().findPropertyByPath(m_pythonScriptDialogGroup + "/Manifest Additional Info"));
	const ot::PropertyString* manifestDescriptionProp = dynamic_cast<const ot::PropertyString*>(
		_dialogCfg.getGridConfig().findPropertyByPath(m_pythonScriptDialogGroup + "/Manifest Description"));

	if (manifestAdditionalInfoProp) {
		libraryElement.addAdditionalInfo("Info", manifestAdditionalInfoProp->getValue());
	}
	if (manifestDescriptionProp) {
		libraryElement.addMetaData("Description", manifestDescriptionProp->getValue());
	}

	_elementsToExport.push_back(libraryElement);
}

void FileHandler::exportLibraryElementsToUserLibrary(const std::list<ot::UserLibraryElement>& _elementsToExport) {

	// Create message to send to LMS with list of LibraryElements
	ot::JsonDocument doc;
	doc.AddMember(ot::JsonString(OT_ACTION_MEMBER, doc.GetAllocator()), ot::JsonString(OT_ACTION_CMD_LMS_AddUserLibraryElement, doc.GetAllocator()), doc.GetAllocator());

	// Serialize all LibraryElements to JSON array
	ot::JsonArray elementsArray;
	for (const ot::UserLibraryElement& element : _elementsToExport) {
		ot::JsonObject elementObj;
		element.addToJsonObject(elementObj, doc.GetAllocator());
		elementsArray.PushBack(elementObj, doc.GetAllocator());
	}

	doc.AddMember(ot::JsonString(OT_ACTION_PARAM_Config, doc.GetAllocator()), elementsArray, doc.GetAllocator());

	// Add database credentials
	doc.AddMember(ot::JsonString(OT_PARAM_DB_USERNAME, doc.GetAllocator()), ot::JsonString(DataBase::instance().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(ot::JsonString(OT_PARAM_DB_PASSWORD, doc.GetAllocator()), ot::JsonString(DataBase::instance().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(ot::JsonString(OT_ACTION_PARAM_DATABASE_URL, doc.GetAllocator()), ot::JsonString(DataBase::instance().getDataBaseServerURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(ot::JsonString(OT_ACTION_PARAM_SERVICE_URL, doc.GetAllocator()), ot::JsonString(Application::instance()->getUiComponent()->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	// Send message to LMS
	std::string response = Application::instance()->getLibraryManagementWrapper().requestCreateConfig(doc);
}