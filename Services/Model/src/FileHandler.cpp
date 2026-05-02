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
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityFileCSV.h"
#include "OTModelEntities/EntityFileText.h"
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
#include "OTDataStorage/DocumentAPI.h"
#include "OTDataStorage/GridFSFileInfo.h"
#include <assert.h>
#include "OTModelEntities/IEventHandler.h"
#include "OTServiceFoundation/Python/PythonServiceInterface.h"
#include "OTCore/Python/PyhonParameterBuilderValueComparisons.h"
#include "OTModelEntities/EntityPythonManifest.h"
#include <filesystem>


FileHandler::FileHandler() {
	const std::string pageName = Application::getToolBarPageName();

	m_buttonPythonImport = ot::ToolBarButtonCfg(pageName, c_groupName, "Import Python Script", "Default/python");
	m_buttonPythonImport.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonFileImport = ot::ToolBarButtonCfg(pageName, c_groupName, "Import Text File", "Default/TextVisible");
	m_buttonFileImport.setButtonLockFlags(ot::LockType::ModelWrite);
	m_buttonExportFileToLibrary = ot::ToolBarButtonCfg(pageName, c_groupName, "Export File to Library", "Default/Export");
	m_buttonExportFileToLibrary.setButtonLockFlags(ot::LockType::ModelWrite);

	m_buttonHandler.connectToolBarButton(m_buttonFileImport, this, &FileHandler::handleImportTextFileButton);
	m_buttonHandler.connectToolBarButton(m_buttonPythonImport, this, &FileHandler::handleImportPythonScriptButton);
	m_buttonHandler.connectToolBarButton(m_buttonExportFileToLibrary, this, &FileHandler::handleExportFilesToLibrary);

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
	_uiComponent->addMenuButton(m_buttonExportFileToLibrary);
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

	ot::UIDList selectedEntities = Application::instance()->getSelectionHandler().getSelectedEntityIDs();
	if (selectedEntities.empty()) {
		Application::instance()->getNotifier()->reportError("Please select at least one entity and one meta file to export to the library.");
		return;
	}

	// Collect the entities to export
	EntityFileText* pythonScript = nullptr;
	EntityPythonManifest* pythonManifest = nullptr;
	EntityFileText* pythonMetaFile = nullptr;
	EntityFileText* manifestMetaFile = nullptr;
	EntityFileText* circuitModel = nullptr;
	EntityFileText* circuitMetaFile = nullptr;

	for (auto entityID : selectedEntities) {
		EntityBase* entity = Application::instance()->getModel()->getEntityByID(entityID);
		if (entity == nullptr) continue;

		// Check if it's a PythonScript or metadata file
		EntityFileText* fileEntity = dynamic_cast<EntityFileText*>(entity);
		if (fileEntity != nullptr) {
			EntityPropertiesBase* exportTypeProperty = entity->getProperties().getProperty("ExportType");
			if (exportTypeProperty != nullptr) {
				EntityPropertiesString* selectionProperty = dynamic_cast<EntityPropertiesString*>(exportTypeProperty);
				if (selectionProperty != nullptr) {
					std::string exportType = selectionProperty->getValue();
					if (exportType == "PythonScript") {
						pythonScript = fileEntity;
					}
					else if (exportType == "PythonMeta") {
						pythonMetaFile = fileEntity;
					}
					else if (exportType == "ManifestMeta") {
						manifestMetaFile = fileEntity;
					}
					else if (exportType == "CircuitModel") {
						circuitModel = fileEntity;
					}
					else if (exportType == "CircuitMeta") {
						circuitMetaFile = fileEntity;
					}
				}
			}
			else {

				EntityPropertiesBase* fileType = fileEntity->getProperties().getProperty("FileType");
				if (!fileType) {
					Application::instance()->getNotifier()->reportError(
						"Selected file entity " + entity->getName() + " does not have a valid ExportType or FileType property. Please ensure that the file entities have the correct properties set for export."
					);
					return;
				}
				EntityPropertiesString* fileTypeSelection = dynamic_cast<EntityPropertiesString*>(fileType);
				if (fileTypeSelection) {
					std::string fileTypeValue = fileTypeSelection->getValue();
					if (fileTypeValue == "py") {
						pythonScript = fileEntity;
					}
					else {
						Application::instance()->getNotifier()->reportError(
							"Selected file entity " + entity->getName() + " has an unsupported file type for export. Please ensure that the file entities have the correct properties set for export."
						);
						return;
					}
				}		
			}
		}

		// Check if it's a PythonManifest
		EntityPythonManifest* manifestEntity = dynamic_cast<EntityPythonManifest*>(entity);
		if (manifestEntity != nullptr) {
			pythonManifest = manifestEntity;
		}
	}

	// Determine what to export (Python or Circuit)
	bool hasPythonExport = (pythonScript != nullptr || pythonManifest != nullptr || pythonMetaFile != nullptr || manifestMetaFile != nullptr);
	bool hasCircuitExport = (circuitModel != nullptr && circuitMetaFile != nullptr);

	if (!hasPythonExport && !hasCircuitExport) {
		Application::instance()->getNotifier()->reportError(
			"Please select either:\n"
			"- PythonScript, PythonManifest, PythonMeta, and ManifestMeta files, or\n"
			"- CircuitModel and CircuitModelMeta files."
		);
		return;
	}

	// Validate Python export if selected
	if (hasPythonExport) {
		if (pythonScript == nullptr || pythonManifest == nullptr || pythonMetaFile == nullptr || manifestMetaFile == nullptr) {
			Application::instance()->getNotifier()->reportError(
				"For Python export: Please select PythonScript, PythonManifest, PythonMeta, and ManifestMeta files."
			);
			return;
		}

		if (!validateMetaDataFile(pythonMetaFile) || !validateMetaDataFile(manifestMetaFile)) {
			return;
		}

		// Perform Python export on separate thread
		std::thread worker(&FileHandler::exportFilesToLibraryAsync, this,
			pythonScript->getEntityID(),
			pythonManifest->getEntityID(),
			pythonMetaFile->getEntityID(),
			manifestMetaFile->getEntityID(),
			pythonManifest->getManifestID()
		);
		worker.detach();
	}

	// Validate and perform Circuit export if selected
	if (hasCircuitExport) {

		Application::instance()->getNotifier()->reportError(
			"Circuit export is currently not supported. Please select Python files for export."
		);
		//if (!validateMetaDataFile(circuitMetaFile)) {
		//	return;
		//}

		//// Perform Circuit export on separate thread
		//std::thread worker(&FileHandler::exportCircuitModelsAsync, this,
		//	circuitModel->getEntityID(),
		//	circuitMetaFile->getEntityID()
		//);
		//worker.detach();
	}
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

				};
			std::thread worker(asyncRequest);
			worker.detach();
		}
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
	const char* devRoot = std::getenv("OPENTWIN_DEV_ROOT");

	if (devRoot == nullptr) {
		return "";
	}

	std::string path(devRoot);
	path += "/LibraryData";
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

void FileHandler::exportCircuitModelsAsync(ot::UID _modelID, ot::UID _metaID) {
	auto uiComponent = Application::instance()->getUiComponent();
	ot::UILockWrapper uiLock(uiComponent, ot::LockType::ModelWrite);

	try {
		Model* model = Application::instance()->getModel();
		assert(model != nullptr);

		// Retrieve entities
		EntityBase* modelEntityBase = model->getEntityByID(_modelID);
		EntityBase* metaEntityBase = model->getEntityByID(_metaID);

		if (modelEntityBase == nullptr || metaEntityBase == nullptr) {
			OT_LOG_E("Error: Circuit model or metadata entity could not be retrieved.");
			model->reportError("Error: Circuit model or metadata entity could not be retrieved.");
			return;
		}

		EntityFileText* modelEntity = dynamic_cast<EntityFileText*>(modelEntityBase);
		EntityFileText* metaEntity = dynamic_cast<EntityFileText*>(metaEntityBase);

		if (modelEntity == nullptr || metaEntity == nullptr) {
			OT_LOG_E("Error: One or more circuit model entities could not be retrieved.");
			model->reportError("Error: Circuit model entities could not be retrieved.");
			return;
		}

		// Get base export path
		std::string libDataPath = getLibraryDataPath();
		if (libDataPath.empty()) {
			OT_LOG_E("Error: Could not determine OpenTwinDevRoot path.");
			model->reportError("Error: Could not determine library data path.");
			return;
		}

		ProgressUpdater updater(uiComponent, "Exporting circuit model to library", false);
		updater.setTotalNumberOfSteps(1);

		exportCircuitModel(modelEntity, metaEntity, libDataPath);
		updater.triggerUpdate(1);

		uiComponent->displayMessage("Circuit model successfully exported to library.\n");
		model->displayMessage("Circuit model exported successfully.");
	}
	catch (const std::exception& _e) {
		OT_LOG_E(std::string("Circuit model export failed: ") + _e.what());
	}
}

void FileHandler::exportFilesToLibraryAsync(ot::UID _scriptID, ot::UID _manifestID, ot::UID _pythonMetaID, ot::UID _manifestMetaID, ot::UID _environmentID) {
	auto uiComponent = Application::instance()->getUiComponent();
	ot::UILockWrapper uiLock(uiComponent, ot::LockType::ModelWrite);

	try {
		Model* model = Application::instance()->getModel();
		assert(model != nullptr);

		// Retrieve entities
		EntityBase* scriptEntityBase = model->getEntityByID(_scriptID);
		EntityBase* manifestEntityBase = model->getEntityByID(_manifestID);
		EntityBase* pythonMetaEntityBase = model->getEntityByID(_pythonMetaID);
		EntityBase* manifestMetaEntityBase = model->getEntityByID(_manifestMetaID);

		if (scriptEntityBase == nullptr || manifestEntityBase == nullptr || pythonMetaEntityBase == nullptr || manifestMetaEntityBase == nullptr) {
			OT_LOG_E("Error: One or more entities could not be retrieved.");
			return;
		}

		EntityFileText* scriptEntity = dynamic_cast<EntityFileText*>(scriptEntityBase);
		EntityPythonManifest* manifestEntity = dynamic_cast<EntityPythonManifest*>(manifestEntityBase);
		EntityFileText* pythonMetaEntity = dynamic_cast<EntityFileText*>(pythonMetaEntityBase);
		EntityFileText* manifestMetaEntity = dynamic_cast<EntityFileText*>(manifestMetaEntityBase);

		if (scriptEntity == nullptr || manifestEntity == nullptr || pythonMetaEntity == nullptr || manifestMetaEntity == nullptr) {
			OT_LOG_E("Error: Failed to cast one or more entities.");
			return;
		}

		// Get base export path
		std::string libDataPath = getLibraryDataPath();
		if (libDataPath.empty()) {
			OT_LOG_E("Error: Could not determine OpenTwinDevRoot path.");
			return;
		}

		ProgressUpdater updater(uiComponent, "Exporting files to library", false);
		updater.setTotalNumberOfSteps(2);

		exportPythonManifest(manifestEntity, manifestMetaEntity, libDataPath, _environmentID);
		updater.triggerUpdate(1);

		exportPythonScript(scriptEntity, pythonMetaEntity, libDataPath, _environmentID);
		updater.triggerUpdate(2);

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

	// Update LibraryElementID to manifest ID
	metaDoc.RemoveMember("LibraryElementID");
	metaDoc.AddMember("LibraryElementID", ot::JsonValue(_manifestEntity->getManifestID()), metaDoc.GetAllocator());

	// Update Version 
	metaDoc.RemoveMember("Version");
	metaDoc.AddMember("Version", ot::JsonValue(1), metaDoc.GetAllocator());

	// Update name and filename
	metaDoc.RemoveMember("Name");
	metaDoc.AddMember("Name", ot::JsonString(_manifestEntity->getNameOnly(), metaDoc.GetAllocator()), metaDoc.GetAllocator());
	metaDoc.RemoveMember("FileName");
	metaDoc.AddMember("FileName", ot::JsonString(ensureFileExtension(_manifestEntity->getNameOnly(), ".txt"), metaDoc.GetAllocator()), metaDoc.GetAllocator());

	// Export manifest .txt file
	std::string manifestFileName = environmentPath + "/" + ensureFileExtension(_manifestEntity->getNameOnly(), ".txt");
	std::string manifestContent = _manifestEntity->getText();
	
	// Check if file exists and handle accordingly
	if (!checkAndHandleFileOverwrite(manifestFileName, manifestContent)) {
		return; // User cancelled or file is identical
	}
	
	writeFileToPath(manifestFileName, manifestContent);

	// Export metadata .otmeta.json file
	std::string metaFileName = environmentPath + "/" + ensureFileExtension(_metaEntity->getNameOnly(), ".otmeta.json");
	std::string metaJson = ot::json::toJson(metaDoc);
	
	// Check if file exists and handle accordingly
	if (!checkAndHandleFileOverwrite(metaFileName, metaJson)) {
		return; // User cancelled or file is identical
	}
	
	writeFileToPath(metaFileName, metaJson);
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

	// Update LibraryElementID to script entity ID
	metaDoc.RemoveMember("LibraryElementID");
	metaDoc.AddMember("LibraryElementID", ot::JsonValue(_scriptEntity->getEntityID()), metaDoc.GetAllocator());

	// Update Version
	metaDoc.RemoveMember("Version");
	metaDoc.AddMember("Version", ot::JsonValue(1), metaDoc.GetAllocator());

	// Update name and filename
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

	// Export script .py file
	std::string scriptFileName = scriptPath + "/" + ensureFileExtension(_scriptEntity->getNameOnly(), ".py");
	std::string scriptContent = _scriptEntity->getText();
	
	// Check if file exists and handle accordingly
	if (!checkAndHandleFileOverwrite(scriptFileName, scriptContent)) {
		return; // User cancelled or file is identical
	}
	
	writeFileToPath(scriptFileName, scriptContent);

	// Export metadata .otmeta.json file
	std::string metaFileName = scriptPath + "/" + ensureFileExtension(_metaEntity->getNameOnly(), ".otmeta.json");
	std::string metaJson = ot::json::toJson(metaDoc);
	
	// Check if file exists and handle accordingly
	if (!checkAndHandleFileOverwrite(metaFileName, metaJson)) {
		return; // User cancelled or file is identical
	}
	
	writeFileToPath(metaFileName, metaJson);
}

void FileHandler::exportCircuitModel(EntityFileText* _modelEntity, EntityFileText* _metaEntity, const std::string& _basePath) {
	assert(_modelEntity != nullptr && _metaEntity != nullptr);

	// Create CircuitData directory
	std::string modelPath = _basePath + "/CircuitData";
	if (!ensureDirectoryExists(modelPath)) {
		throw std::runtime_error("Could not create CircuitModels directory");
	}

	// Read and validate metadata
	std::string metaContent = _metaEntity->getText();
	ot::JsonDocument metaDoc;
	metaDoc.fromJson(metaContent);

	// Update LibraryElementID to model entity ID
	metaDoc.RemoveMember("LibraryElementID");
	metaDoc.AddMember("LibraryElementID", ot::JsonValue(_modelEntity->getEntityID()), metaDoc.GetAllocator());

	// Update Version
	metaDoc.RemoveMember("Version");
	metaDoc.AddMember("Version", ot::JsonValue(1), metaDoc.GetAllocator());

	// Update name and filename
	metaDoc.RemoveMember("Name");
	metaDoc.AddMember("Name", ot::JsonString(_modelEntity->getNameOnly(), metaDoc.GetAllocator()), metaDoc.GetAllocator());
	metaDoc.RemoveMember("FileName");
	metaDoc.AddMember("FileName", ot::JsonString(ensureFileExtension(_modelEntity->getNameOnly(), ".txt"), metaDoc.GetAllocator()), metaDoc.GetAllocator());

	// Export circuit model .txt file
	std::string modelFileName = modelPath + "/" + ensureFileExtension(_modelEntity->getNameOnly(), ".txt");
	std::string modelContent = _modelEntity->getText();
	
	// Check if file exists and handle accordingly
	if (!checkAndHandleFileOverwrite(modelFileName, modelContent)) {
		return; // User cancelled or file is identical
	}
	
	writeFileToPath(modelFileName, modelContent);

	// Export metadata .otmeta.json file
	std::string metaFileName = modelPath + "/" + ensureFileExtension(_metaEntity->getNameOnly(), ".otmeta.json");
	std::string metaJson = ot::json::toJson(metaDoc);
	
	// Check if file exists and handle accordingly
	if (!checkAndHandleFileOverwrite(metaFileName, metaJson)) {
		return; // User cancelled or file is identical
	}
	
	writeFileToPath(metaFileName, metaJson);
}

void FileHandler::clearBuffer() {
	m_entityVersionsData.clear();
	m_entityVersionsTopo.clear();
	m_entityIDsData.clear();
	m_entityIDsTopo.clear();
	m_forceVisible.clear();
}

bool FileHandler::checkAndHandleFileOverwrite(const std::string& _filePath, const std::string& _newContent) const {
	// Check if file exists
	std::ifstream file(_filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		// File does not exist, can proceed
		return true;
	}

	// File exists, check if content is identical
	std::ifstream::pos_type fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Compare file sizes first
	if (static_cast<size_t>(fileSize) != _newContent.size()) {
		// Different size, ask user for confirmation
		return promptUserForOverwrite(_filePath);
	}

	// Files have same size, compare content
	std::vector<char> fileContent(fileSize);
	file.read(fileContent.data(), fileSize);
	file.close();

	if (std::equal(fileContent.begin(), fileContent.end(), _newContent.begin())) {
		// Content is identical, skip writing
		OT_LOG_D("Skipping file export: content is identical for \"" + _filePath + "\"");
		return false;
	}

	// Content is different, ask user for confirmation
	return promptUserForOverwrite(_filePath);
}

bool FileHandler::promptUserForOverwrite(const std::string& _filePath) const {
	//// Create a dialog to ask the user for confirmation
	//ot::MessageDialogCfg cfg;
	//cfg.setTitle("File Already Exists");
	//cfg.setText("The file \"" + _filePath + "\" already exists with different content.\n\n"
	//	"Do you want to overwrite it?");
	//cfg.setButtons(ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No);
	//cfg.setIcon(ot::MessageDialogCfg::Question);

	////ot::MessageDialogCfg::BasicButton result = Application::instance()->getUiComponent()->sendMessage()

	//return result == ot::MessageDialogCfg::Yes;
	return true; // For now, we will just overwrite without asking
}
