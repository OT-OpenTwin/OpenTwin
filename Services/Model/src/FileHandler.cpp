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
#include <assert.h>

void FileHandler::addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName)
{
	const std::string groupName = "File Imports";

	_uiComponent->addMenuGroup(_pageName,groupName);
	m_buttonFileImport.SetDescription(_pageName, groupName, "Import Text File");
	m_buttonPythonImport.SetDescription(_pageName, groupName, "Import Python Script");

	_uiComponent->addMenuButton(m_buttonPythonImport, ot::LockModelWrite, "python");
	_uiComponent->addMenuButton(m_buttonFileImport, ot::LockModelWrite, "TextVisible");
}

bool FileHandler::handleAction(const std::string& _action, ot::JsonDocument& _doc)
{
	bool actionIsHandled= false;
	
	if (_action == m_buttonFileImport.GetFullDescription())
	{
		const std::string fileMask = ot::FileExtension::toFilterString({ ot::FileExtension::Text, ot::FileExtension::CSV, ot::FileExtension::AllFiles });
		const std::string fileDialogTitle = "Import Text File";
		const std::string subsequentFunction = "ImportTextFile";
		importFile(fileMask,fileDialogTitle,subsequentFunction);
		actionIsHandled = true;
	}
	else if (_action == m_buttonPythonImport.GetFullDescription())
	{
		const std::string fileMask = ot::FileExtension::toFilterString({ ot::FileExtension::Python, ot::FileExtension::AllFiles });
		const std::string fileDialogTitle = "Import Python Script";
		const std::string subsequentFunction = "ImportPythonScript";
		importFile(fileMask, fileDialogTitle, subsequentFunction);
		actionIsHandled = true;
	}
	else if (_action == "ImportTextFile")
	{
		std::thread worker(&FileHandler::storeTextFile,this, std::move(_doc), std::ref(ot::FolderNames::FilesFolder));
		worker.detach();
		actionIsHandled = true;
	}
	else if (_action == "ImportPythonScript")
	{
		std::thread worker(&FileHandler::storeTextFile, this, std::move(_doc), std::ref(ot::FolderNames::FilesFolder));
		worker.detach();
		actionIsHandled = true;
	}
	else if (_action == OT_ACTION_CMD_UI_TEXTEDITOR_SaveRequest)
	{
		handleChangedText(_doc);
		actionIsHandled = true;
	}
	else if (_action == OT_ACTION_CMD_UI_TABLE_SaveRequest)
	{
		handleChangedTable(_doc);
		actionIsHandled = true;
	}
	else
	{
		actionIsHandled = false;
	}
	return actionIsHandled;
}

void FileHandler::importFile(const std::string& _fileMask, const std::string& _dialogTitle, const std::string& _functionName)
{
	const std::string& serviceURL = Application::instance()->getServiceURL();
	const std::string serviceName =	Application::instance()->getServiceName();
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString(_dialogTitle, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(_fileMask, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(_functionName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent, ot::JsonValue(true), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_LoadMultiple, ot::JsonValue(true), doc.GetAllocator());
	std::string response;
	Application::instance()->uiComponent()->sendMessage(false, doc, response);
}


void FileHandler::storeTextFile(ot::JsonDocument&& _document, const std::string& _folderName)
{
	auto uiComponent =	Application::instance()->uiComponent();
	UILockWrapper uiLock(uiComponent, ot::LockModelWrite);
	std::list<std::string> contents = ot::json::getStringList(_document, OT_ACTION_PARAM_FILE_Content);
	std::list<int64_t> 	uncompressedDataLengths = ot::json::getInt64List(_document, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);
	std::list<std::string> fileNames = ot::json::getStringList(_document, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileFilter = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mask);

	assert(fileNames.size() == contents.size() && contents.size() == uncompressedDataLengths.size());
	{
		QueuingDatabaseWritingRAII queueDatabase;
		auto uncompressedDataLength = uncompressedDataLengths.begin();
		auto content = contents.begin();
		
		ProgressUpdater updater(uiComponent, "Importing files");
		updater.setTotalNumberOfSteps(fileNames.size());
		uint32_t counter(0);
		auto start = std::chrono::system_clock::now();
		for (std::string& fileName : fileNames)
		{
			counter++;

			std::string fileContent = ot::Encryption::decryptAndUnzipString(*content, *uncompressedDataLength);
		
			storeFileInDataBase(fileContent, fileName, _folderName, fileFilter);
			uncompressedDataLength++;
			content++;
			updater.triggerUpdate(counter);
		}
		auto end = std::chrono::system_clock::now();
		uint64_t passedTime =	std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		uiComponent->displayMessage("Storing documentsin database: " + std::to_string(passedTime) + " ms\n");
	}
	auto start = std::chrono::system_clock::now();
	addTextFilesToModel();
	clearBuffer();
	auto end = std::chrono::system_clock::now();
	uint64_t passedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	uiComponent->displayMessage("Model update: " + std::to_string(passedTime) + " ms\n");
}

void FileHandler::handleChangedTable(ot::JsonDocument& _doc)
{
	ot::TableCfg config;
	config.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_Config));
	const std::string entityName = config.getEntityName();
	
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);

	const auto entityIDsByName = model->getEntityNameToIDMap();
	auto entityIDByName = entityIDsByName.find(entityName);
	if (entityIDByName != entityIDsByName.end())
	{
		ot::UID entityID = entityIDByName->second;
		EntityBase* entityBase = model->getEntityByID(entityID);
		IVisualisationTable* tableVisualisationEntity = dynamic_cast<IVisualisationTable*>(entityBase);
		if (tableVisualisationEntity != nullptr)
		{
			ot::ContentChangedHandling changedHandling = tableVisualisationEntity->getTableContentChangedHandling();
			if (changedHandling == ot::ContentChangedHandling::ModelServiceSaves)
			{
				storeChangedTable(tableVisualisationEntity, config);
			}
			else if (changedHandling == ot::ContentChangedHandling::OwnerHandles)
			{
				const std::string& owner = entityBase->getOwningService();
				if (owner != OT_INFO_SERVICE_TYPE_MODEL)
				{
					std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(_doc), owner); //Potentially dangerous ? Usually the document should not be used afterwards.
					workerThread.detach();
				}
			}
			else if (changedHandling == ot::ContentChangedHandling::ModelServiceSavesNotifyOwner)
			{
				storeChangedTable(tableVisualisationEntity, config);
				const std::string& owner = entityBase->getOwningService();
				if (owner != OT_INFO_SERVICE_TYPE_MODEL)
				{
					ot::JsonDocument notify;
					notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteAction, notify.GetAllocator()), notify.GetAllocator());
					notify.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SetModified, notify.GetAllocator()), notify.GetAllocator());
					std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(notify), owner);
					workerThread.detach();
				}
			}
		}
		else
		{
			OT_LOG_E("Failed to visualise " + entityName + " since it does not support the corresponding visualisation interface.");
		}
	}
	else
	{
		OT_LOG_E("Failed to handle changed text request since the entity could not be found by name: " + entityName);
	}
}

void FileHandler::storeChangedTable(IVisualisationTable* _entity, ot::TableCfg& _cfg)
{
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);
	assert(_entity != nullptr);

	_entity->setTable(_cfg.createMatrix());
	model->setModified();
	model->modelChangeOperationCompleted("Updated Table.");
}

void FileHandler::handleChangedText(ot::JsonDocument& _doc)
{
	const std::string entityName = _doc[OT_ACTION_PARAM_TEXTEDITOR_Name].GetString();
	const std::string textContent = _doc[OT_ACTION_PARAM_TEXTEDITOR_Text].GetString();

	Model* model =	Application::instance()->getModel();
	assert(model != nullptr);

	const auto entityIDsByName = model->getEntityNameToIDMap();
	auto entityIDByName	= entityIDsByName.find(entityName);
	if(entityIDByName != entityIDsByName.end())
	{
		ot::UID entityID = entityIDByName->second;
		EntityBase* entityBase = model->getEntityByID(entityID);
		IVisualisationText* textVisualisationEntity = dynamic_cast<IVisualisationText*>(entityBase);
		if(textVisualisationEntity != nullptr)
		{
			ot::ContentChangedHandling changedHandling = textVisualisationEntity->getTextContentChangedHandling();
			if (changedHandling == ot::ContentChangedHandling::ModelServiceSaves)
			{
				
				storeChangedText(textVisualisationEntity, textContent);
			}
			else if (changedHandling == ot::ContentChangedHandling::OwnerHandles)
			{
				const std::string& owner =	entityBase->getOwningService();
				if (owner != OT_INFO_SERVICE_TYPE_MODEL)
				{
					std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(_doc), owner); //Potentially dangerous ? Usually the document should not be used afterwards.
					workerThread.detach();
				}
			}
			else if (changedHandling == ot::ContentChangedHandling::ModelServiceSavesNotifyOwner)
			{
				storeChangedText(textVisualisationEntity, textContent);
				
				const std::string& owner = entityBase->getOwningService();
				if (owner != OT_INFO_SERVICE_TYPE_MODEL)
				{
					ot::JsonDocument notify;
					notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteAction, notify.GetAllocator()), notify.GetAllocator());
					notify.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SetModified, notify.GetAllocator()), notify.GetAllocator());
					std::thread workerThread(&FileHandler::NotifyOwnerAsync, this, std::move(notify), owner);
					workerThread.detach();
				}
			}
		}
		else
		{
			OT_LOG_E("Failed to visualise " + entityName + " since it does not support the corresponding visualisation interface.");
		}
	}
	else
	{
		OT_LOG_E("Failed to handle changed text request since the entity could not be found by name: " + entityName);
	}
}

void FileHandler::storeChangedText(IVisualisationText* _entity, const std::string _text)
{
	Model* model = Application::instance()->getModel();
	assert(model != nullptr);
	assert(_entity != nullptr);

	_entity->setText(_text);
	model->setModified();
	model->modelChangeOperationCompleted("Updated Text.");
}

void FileHandler::NotifyOwnerAsync(ot::JsonDocument&& _doc, const std::string _owner)
{
	std::string response;
	Application::instance()->sendMessage(true, _owner, _doc, response);
}

void FileHandler::storeFileInDataBase(const std::string& _text, const std::string& _fileName, const std::string& _folderName, const std::string& _fileFilter)
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
		textFile.reset(new EntityFileCSV (entIDTopo, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_MODEL));
	}
	else
	{
		textFile.reset(new EntityFileText(entIDTopo, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_MODEL));
	}

	EntityBinaryData fileContent(entIDData, textFile.get(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_MODEL);
	fileContent.setData(_text.data(), _text.size());
	fileContent.StoreToDataBase();

	textFile->setData(fileContent.getEntityID(),fileContent.getEntityStorageVersion());

	ot::EncodingGuesser guesser;
	textFile->setFileProperties(path, name,type);

	textFile->setFileFilter(_fileFilter);

	std::list<std::string> folderEntities = model->getListOfFolderItems(_folderName, true);
	const std::string entityName = CreateNewUniqueTopologyName(folderEntities, _folderName, name);;
	textFile->setName(entityName);
	
	textFile->setTextEncoding(guesser(_text.data(), _text.size()));
	textFile->StoreToDataBase();
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
	model->addEntitiesToModel(m_entityIDsTopo, m_entityVersionsTopo, m_forceVisible, m_entityIDsData, m_entityVersionsData, m_entityIDsTopo, "Added File", true, false);
}