/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"

// OpenTwin header
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTCommunication/ActionDispatcher.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

#include "Connection\ConnectionAPI.h"
#include "Document\DocumentAccessBase.h"

#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

// Application specific includes
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "EntityResultText.h"
#include "EntityUnits.h"
#include "EntityMaterial.h"
#include "EntityProperties.h"
#include "EntityGeometry.h"
#include "EntityFile.h"
#include "EntityBinaryData.h"

#include "InfoFileManager.h"
#include "Result1DManager.h"
#include "ParametricResult1DManager.h"
#include "ParametricCombination.h"
#include "LTSpiceRawReader.h"

#include "boost/algorithm/string.hpp"

#include <thread>
#include <map>
#include <sstream>
#include <filesystem>
#include <regex>

#include "zlib.h"
#include "base64.h"

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_LTSPICE

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_LTSPICE

Application::Application()
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier())
{

}

Application::~Application()
{

}

// ##################################################################################################################################

// Required functions

std::string Application::processAction(const std::string & _action,  ot::JsonDocument& _doc)
{
	if (_action == OT_ACTION_CMD_UI_LTS_UPLOAD_NEEDED)
	{
		uploadNeeded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_LTS_DOWNLOAD_NEEDED)
	{
		downloadNeeded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_LTS_FILES_UPLOADED)
	{
		filesUploaded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_LTS_GET_LOCAL_FILENAME)
	{
		std::string hostName = ot::json::getString(_doc, OT_ACTION_PARAM_HOSTNAME);

		return getLocalFileName(hostName);
	}
	else if (_action == OT_ACTION_CMD_UI_LTS_SET_LOCAL_FILENAME)
	{
		std::string hostName = ot::json::getString(_doc, OT_ACTION_PARAM_HOSTNAME);
		std::string fileName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Name);

		setLocalFileName(hostName, fileName);
	}
	else if (_action == OT_ACTION_CMD_UI_LTS_GET_SIMPLE_FILENAME)
	{
		return getSimpleFileName();
	}

	return OT_ACTION_RETURN_UnknownAction;
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();
	_ui->addMenuPage("Project");
	
	_ui->addMenuGroup("Project", "Local Project");
	_ui->addMenuGroup("Project", "Versions");

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_ui->addMenuButton("Project", "Local Project", "Import", "Import", modelWrite, "Import", "Default");
	_ui->addMenuButton("Project", "Local Project", "Set File", "Set File", modelWrite, "ProjectSaveAs", "Default");
	_ui->addMenuButton("Project", "Versions", "Information", "Information", modelWrite, "Information", "Default");
	_ui->addMenuButton("Project", "Versions", "Commit", "Commit", modelWrite, "AddSolver", "Default");
	_ui->addMenuButton("Project", "Versions", "Checkout", "Checkout", modelWrite, "ArrowGreenDown", "Default");

	connectToolBarButton("Project:Local Project:Import", this, &Application::importProject);
	connectToolBarButton("Project:Local Project:Set File", this, &Application::setLTSpiceFile);
	connectToolBarButton("Project:Versions:Information", this, &Application::showInformation);
	connectToolBarButton("Project:Versions:Commit", this, &Application::commitChanges);
	connectToolBarButton("Project:Versions:Checkout", this, &Application::getChanges);

	modelSelectionChanged();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

// ##################################################################################################################################

void Application::modelSelectionChanged()
{
	//if (isUiConnected()) {
	//	std::list<std::string> enabled;
	//	std::list<std::string> disabled;

	//	if (selectedEntities.size() > 0)
	//	{
	//		enabled.push_back("ElmerFEM:Solver:Run Solver");
	//	}
	//	else
	//	{
	//		disabled.push_back("ElmerFEM:Solver:Run Solver");
	//	}

	//	m_uiComponent->setControlsEnabledState(enabled, disabled);
	//}
}

void Application::importProject(void)
{
	getModelComponent()->clearNewEntityList();

	// Check whether the project has already been initialized
	if (isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has already been initialized.");
		return;
	}
	 
	// Send the import message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_IMPORT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::setLTSpiceFile(void)
{
	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import an LTSpice project file first.");
		return;
	}

	// Send the set file message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_SETLTSPICEFILE, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::commitChanges(void)
{
	getModelComponent()->clearNewEntityList();

	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import an LTSpice project file first.");
		return;
	}

	// Send the commit message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_COMMIT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::showInformation(void)
{
	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import an LTSpice project file first.");
		return;
	}

	std::string currentVersion = ot::ModelServiceAPI::getCurrentModelVersion();

	// Send the information message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_INFORMATION, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(currentVersion, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::getChanges(void)
{
	getModelComponent()->clearNewEntityList();

	// Check whether the project has already been initialized
	if (!isProjectInitialized())
	{
		getUiComponent()->displayErrorPrompt("This project has not yet been initialized. Please import an LTSpice project file first.");
		return;
	}

	// Get the current model version
	std::string version = ot::ModelServiceAPI::getCurrentModelVersion();

	// Send the commit message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_GET, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(version, doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::uploadNeeded(ot::JsonDocument& _doc)
{
	size_t count = ot::json::getInt64(_doc, OT_ACTION_PARAM_COUNT);

	std::string fileName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Name);
	std::string hostName= ot::json::getString(_doc, OT_ACTION_PARAM_HOSTNAME);

	std::string simpleFileName;
	std::list<std::pair<std::string, std::string>> hostNamesAndFileNames;

	readProjectInformation(simpleFileName, hostNamesAndFileNames);

	addHostNameAndFileName(hostName, fileName, hostNamesAndFileNames);

	std::filesystem::path path(fileName);
	if (!simpleFileName.empty())
	{
		assert(simpleFileName == path.filename().string());
	}

	simpleFileName = path.filename().string();

	writeProjectInformation(simpleFileName, hostNamesAndFileNames);

	ot::UIDList entityID, versionID;

	for (size_t i = 0; i < count; i++)
	{
		entityID.push_back(this->getModelComponent()->createEntityUID());
		versionID.push_back(this->getModelComponent()->createEntityUID());
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_UPLOAD, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(versionID, doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::downloadNeeded(ot::JsonDocument& _doc)
{
	// Determine all files in the Files folder
	std::list<std::string> fileNames = ot::ModelServiceAPI::getListOfFolderItems("Files", true);

	std::list<ot::EntityInformation> fileInfo;
	ot::ModelServiceAPI::getEntityInformation(fileNames, fileInfo);

	ot::UIDList entityID, versionID;

	for (auto file : fileInfo)
	{
		if (file.getEntityType() == "EntityFile" && file.getEntityName() != "Files/Information")
		{
			entityID.push_back(file.getEntityID());
			versionID.push_back(file.getEntityVersion());
		}
	}

	// Get the current model version
	std::string version = ot::ModelServiceAPI::getCurrentModelVersion();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_DOWNLOAD, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(versionID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(version, doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	getUiComponent()->sendMessage(true, doc, tmp);
}

void Application::filesUploaded(ot::JsonDocument& _doc)
{
	std::string changeMessage = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);

	std::list<std::string> modifiedNameList = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Name);
	std::list<ot::UID> fileEntityIDList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityID);
	std::list<ot::UID> fileVersionList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityVersion);
	std::list<ot::UID> dataEntityIDList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_DataID);
	std::list<ot::UID> dataVersionList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_DataVersion);

	std::list<std::string> deletedNameList = ot::json::getStringList(_doc, OT_ACTION_CMD_MODEL_DeleteEntity);


	// Now we need to send the model change to the model service 
	for (auto item : modifiedNameList)
	{
		ot::UID fileEntityID = fileEntityIDList.front(); fileEntityIDList.pop_front();
		ot::UID fileVersion  = fileVersionList.front();  fileVersionList.pop_front();
		ot::UID dataEntityID = dataEntityIDList.front(); dataEntityIDList.pop_front();
		ot::UID dataVersion  = dataVersionList.front();  dataVersionList.pop_front();

		getModelComponent()->addNewTopologyEntity(fileEntityID, fileVersion, false);
		getModelComponent()->addNewDataEntity(dataEntityID, dataVersion, fileEntityID);
	}

	ot::ModelServiceAPI::deleteEntitiesFromModel(deletedNameList, false);

	getModelComponent()->storeNewEntities("", false, false);

	extractResults();

	getModelComponent()->storeNewEntities("LTSpice project uploaded", false);

	// Determine the new version
	std::string newVersion = ot::ModelServiceAPI::getCurrentModelVersion();

	// Set the label for the new version
	ot::JsonDocument doc1;
	doc1.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SetVersionLabel, doc1.GetAllocator()), doc1.GetAllocator());
	doc1.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(newVersion, doc1.GetAllocator()), doc1.GetAllocator());
	doc1.AddMember(OT_ACTION_PARAM_MODEL_VersionLabel, ot::JsonString(changeMessage, doc1.GetAllocator()), doc1.GetAllocator());

	std::string tmp;
	getModelComponent()->sendMessage(true, doc1, tmp);

	// Finally we send the new version to the frontend 
	ot::JsonDocument doc2;
	doc2.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_COPY, doc2.GetAllocator()), doc2.GetAllocator());
	doc2.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(newVersion, doc2.GetAllocator()), doc2.GetAllocator());

	getUiComponent()->sendMessage(true, doc2, tmp);
}

void Application::writeProjectInformation(const std::string &simpleFileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames)
{
	assert(!simpleFileName.empty());

	std::stringstream data;
	data << simpleFileName << std::endl;
	for (auto item : hostNamesAndFileNames)
	{
		data << item.first << std::endl;
		data << item.second << std::endl;
	}

	std::string stringData = data.str();

	mongocxx::collection collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection("Projects", DataBase::GetDataBase()->getProjectName());

	long long modelVersion = getCurrentModelEntityVersion();

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << "Model"
		<< "Version" << modelVersion
		<< bsoncxx::builder::stream::finalize;

	auto modifyDoc = bsoncxx::builder::stream::document{}
		<< "$set" << bsoncxx::builder::stream::open_document
		<< "ProjectInformation" << stringData
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	collection.update_one(queryDoc.view(), modifyDoc.view());
}

long long Application::getCurrentModelEntityVersion(void)
{
	// We search for the last model entity in the database and determine its version
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << "Model"
		<< bsoncxx::builder::stream::finalize;

	auto emptyFilterDoc = bsoncxx::builder::basic::document{};

	auto sortDoc = bsoncxx::builder::basic::document{};
	sortDoc.append(bsoncxx::builder::basic::kvp("$natural", -1));

	auto result = docBase.GetDocument(std::move(queryDoc), std::move(emptyFilterDoc.extract()), std::move(sortDoc.extract()));
	if (!result) return 0;  // No model entity found

	return result->view()["Version"].get_int64();
}

bool Application::readProjectInformation(std::string &simpleFileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames)
{
	// Get the project information string from the model entity
	DataStorageAPI::DocumentAccessBase docBase("Projects", DataBase::GetDataBase()->getProjectName());

	auto queryDoc = bsoncxx::builder::stream::document{}
		<< "SchemaType" << "Model"
		<< bsoncxx::builder::stream::finalize;

	auto emptyFilterDoc = bsoncxx::builder::basic::document{};

	auto sortDoc = bsoncxx::builder::basic::document{};
	sortDoc.append(bsoncxx::builder::basic::kvp("$natural", -1));

	auto result = docBase.GetDocument(std::move(queryDoc), std::move(emptyFilterDoc.extract()), std::move(sortDoc.extract()));
	if (!result) return false;  // No model entity found (this project is not yet initialized)

	std::string projectInformation;

	try
	{
		projectInformation = result->view()["ProjectInformation"].get_utf8().value.data();
	}
	catch (std::exception)
	{
		return false; // This project does not have project information yet (not initialized)
	}

	std::stringstream content(projectInformation);

	std::getline(content, simpleFileName);
	while (!content.eof())
	{
		std::string hostName, fileName;
		std::getline(content, hostName);
		std::getline(content, fileName);

		if (!hostName.empty() && !fileName.empty())
		{
			hostNamesAndFileNames.push_back(std::pair<std::string, std::string>(hostName, fileName));
		}
	}

	return true;
}

bool Application::isProjectInitialized()
{
	std::string simpleFileName;
	std::list<std::pair<std::string, std::string>> hostNamesAndFileNames;

	return readProjectInformation(simpleFileName, hostNamesAndFileNames);
}

std::string Application::getLocalFileName(const std::string &hostName)
{
	std::string simpleFileName;
	std::list<std::pair<std::string, std::string>> hostNamesAndFileNames;

	if (!readProjectInformation(simpleFileName, hostNamesAndFileNames))
	{
		return ""; // This project has not yet been initialized
	}

	// Try to find the Hostname in the list
	for (auto item : hostNamesAndFileNames)
	{
		if (item.first == hostName)
		{
			// We have found this hostName
			return item.second;
		}
	}

	return ""; // We could not find the host name in the list
}

std::string Application::getSimpleFileName()
{
	std::string simpleFileName;
	std::list<std::pair<std::string, std::string>> hostNamesAndFileNames;

	if (!readProjectInformation(simpleFileName, hostNamesAndFileNames))
	{
		return ""; // This project has not yet been initialized
	}

	return simpleFileName;
}

void Application::addHostNameAndFileName(const std::string& hostName, const std::string& fileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames)
{
	// First, we delete an previously existing file for the given host
	for (auto item : hostNamesAndFileNames)
	{
		if (item.first == hostName)
		{
			// We have found this hostName
			hostNamesAndFileNames.remove(item);
			break;
		}
	}

	// Now we add the new file for the given host name
	hostNamesAndFileNames.push_back(std::pair<std::string, std::string>(hostName, fileName));
}

void Application::setLocalFileName(const std::string& hostName, const std::string& fileName)
{
	std::string simpleFileName;
	std::list<std::pair<std::string, std::string>> hostNamesAndFileNames;

	if (readProjectInformation(simpleFileName, hostNamesAndFileNames))
	{
		addHostNameAndFileName(hostName, fileName, hostNamesAndFileNames);

		std::filesystem::path path(fileName);
		if (!simpleFileName.empty())
		{
			assert(simpleFileName == path.filename().string());
		}

		writeProjectInformation(simpleFileName, hostNamesAndFileNames);
	}
	else
	{
		assert(0);
	}
}

void Application::extractResults()
{
	std::string projectFileName = "Files/" + std::filesystem::path(getSimpleFileName()).stem().string();
	if (projectFileName.empty()) return;

	// First, we get a list of all parametric runs and their parameter combinations from the .log file
	std::list<ParametricCombination> parameterRuns;
	getParametricCombinations(projectFileName + ".log", parameterRuns);

	// Now we read the curves from the .raw file
	std::string rawFileName = projectFileName + ".raw";
	ot::EntityInformation entityInfo;
	ot::ModelServiceAPI::getEntityInformation(rawFileName, entityInfo);

	EntityFile* fileEntity = dynamic_cast<EntityFile*>(ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion()));
	if (fileEntity == nullptr) return;

	std::shared_ptr<EntityBinaryData> data = fileEntity->getData();
	if (data == nullptr) return;

	std::vector<char> rawFileData = data->getData();

	std::stringstream dataStream(std::ios::in | std::ios::out | std::ios::binary);
	dataStream.write(rawFileData.data(), rawFileData.size());

	ltspice::AutoRawReader reader;
	ltspice::RawData resultData = reader.read(dataStream);

	// Finally, we store the parametric data of the curves
	// TODO


}

void Application::getParametricCombinations(const std::string& logFileName, std::list<ParametricCombination>& parameterRuns)
{
	parameterRuns.clear();

	// Read the data of the log file entity into a string logFileContent
	ot::EntityInformation entityInfo;
	ot::ModelServiceAPI::getEntityInformation(logFileName, entityInfo);
	
	EntityFile *fileEntity = dynamic_cast<EntityFile*>(ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion()));
	if (fileEntity == nullptr) return;

	std::shared_ptr<EntityBinaryData> data = fileEntity->getData();
	if (data == nullptr) return;

	std::vector<char> logFileData = data->getData();
	std::string logFileContent(logFileData.begin(), logFileData.end());

	// Now extract all lines with a step message from the logfile data
	std::vector<std::string> stepLines;
	std::istringstream stream(logFileContent);
	std::string line;

	while (std::getline(stream, line)) {
		std::string lowerStr = line;
		std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
		if (lowerStr.find(".step") != std::string::npos) {
			stepLines.push_back(line);
		}
	}

	// Finally, we need to process each line and extract its parameter combinations
	std::vector<std::pair<std::string, double>> params;
	std::regex stepRegex(R"((\w+)=([-+]?\d*\.?\d+))"); // Regex f�r Parameter=Wert
	std::smatch match;

	for (auto line : stepLines)
	{
		ParametricCombination parameterCombination;

		auto searchStart = line.cbegin();
		while (std::regex_search(searchStart, line.cend(), match, stepRegex)) {
			std::string param = match[1].str();           // parameter name
			double value = std::stod(match[2].str());     // parameter value
			parameterCombination.addParameter(param, value);
			searchStart = match.suffix().first;
		}

		parameterRuns.push_back(parameterCombination);
	}
}
