/*
 * ProjectManagement.h
 *
 *	Author: Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

// Frontend header
#include "ProjectManagement.h"
#include "UserManagement.h"
#include "AppBase.h"
#include "DataBase.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTCore/ReturnMessage.h"

// DB header
#include "DocumentAPI.h"
#include "Connection\ConnectionAPI.h"
#include "Document\DocumentAccess.h"
#include "Document\DocumentAccessBase.h"
#include "Helper\QueryBuilder.h"
#include "Helper\BsonValuesHelper.h"

// MongoDB header
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pipeline.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>

// std header
#include <iomanip>

ProjectManagement::ProjectManagement() :
	m_isConnected(false),
	m_dataBaseName("Projects"),
	m_projectCatalogCollectionName("Catalog")
{

}

ProjectManagement::ProjectManagement(const LoginData& _data) 
	: m_isConnected(false),
	m_dataBaseName("Projects"),
	m_projectCatalogCollectionName("Catalog"),
	m_databaseURL(_data.getDatabaseUrl()),
	m_authServerURL(_data.getAuthorizationUrl())
{

}

ProjectManagement::~ProjectManagement()
{

}

void ProjectManagement::setDataBaseURL(const std::string &url)
{
	m_databaseURL = url;
}

void ProjectManagement::setAuthServerURL(const std::string &url)
{
	m_authServerURL = url;
}

bool ProjectManagement::createProject(const std::string &projectName, const std::string& projectType, const std::string &userName, const std::string &defaultSettingTemplate)
{
	assert(!m_authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CREATE_PROJECT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_TYPE, ot::JsonString(projectType, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->slotShowErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return false;
	}

	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);
	if (!responseMessage.isOk()) {
		return false;
	}

	ot::JsonDocument projDoc;
	projDoc.fromJson(responseMessage.getWhat());

	ot::ProjectInformation info(projDoc.getConstObject());
	return createNewCollection(info.getCollectionName(), defaultSettingTemplate);
}

bool ProjectManagement::deleteProject(const std::string &projectName)
{
	assert(!m_authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_REMOVE_PROJECT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->slotShowErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return false;
	}

	return hasSuccessful(response);
}

bool ProjectManagement::renameProject(const std::string &oldProjectName, const std::string &newProjectName)
{
	assert(!m_authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CHANGE_PROJECT_NAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(oldProjectName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_NEW_PROJECT_NAME, ot::JsonString(newProjectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->slotShowErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return false;
	}

	ot::ReturnMessage ret = ot::ReturnMessage::fromJson(response);
	return ret.isOk();
}

bool ProjectManagement::projectExists(const std::string &projectName, bool &canBeDeleted)
{
	canBeDeleted = false;

	assert(!m_authServerURL.empty());
	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
 	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->slotShowErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return false;
	}
		
	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);
	if (!responseMessage.isOk()) {
		return false;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(responseMessage.getWhat());

	try
	{
		ot::ProjectInformation info(responseDoc.getConstObject());

		if (info.getUserName() == app->getCurrentLoginData().getUserName())
		{
			canBeDeleted = true;
		}
	}
	catch (std::exception)
	{
	}

	return true;
}

bool ProjectManagement::hasError(const std::string &response)
{
	ot::JsonDocument doc;
	doc.fromJson(response);

	// Check whether the document has an error flag
	return ot::json::exists(doc, OT_ACTION_AUTH_ERROR);
}

bool ProjectManagement::hasSuccessful(const std::string &response)
{
	ot::JsonDocument doc;
	doc.fromJson(response);

	try
	{
		bool success = ot::json::getBool(doc, OT_ACTION_AUTH_SUCCESS);
		return success;
	}
	catch (std::exception)
	{
		return false; // The return document does not have a success flag
	}
}

std::string ProjectManagement::getProjectCollection(const std::string &projectName)
{
	assert(!m_authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->slotShowErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return "";
	}

	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);
	if (!responseMessage.isOk()) {
		return "";
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(responseMessage.getWhat());

	ot::ProjectInformation info(responseDoc.getConstObject());
	return info.getCollectionName();
}

std::string ProjectManagement::getProjectType(const std::string& projectName)
{
	assert(!m_authServerURL.empty());

	AppBase* app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->slotShowErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return "";
	}

	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);
	if (!responseMessage.isOk())
	{
		OT_LOG_E(responseMessage.getWhat());
		return "";
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(responseMessage.getWhat());

	ot::ProjectInformation info(responseDoc.getConstObject());
	return info.getProjectType();
}

bool ProjectManagement::updateAdditionalInformation(const ot::ProjectInformation& projectInfo) {
	assert(!m_authServerURL.empty());

	AppBase* app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_UPDATE_PROJECT_ADDITIONALINFO, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, ot::JsonObject(projectInfo, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->slotShowErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return false;
	}

	return ot::ReturnMessage::fromJson(response).isOk();
}

bool ProjectManagement::findProjects(const std::string& _projectNameFilter, int _maxNumberOfResults, std::list<ot::ProjectInformation>& _projectsFound, bool& _maxLengthExceeded)
{
	assert(!m_authServerURL.empty());

	_projectsFound.clear();
	m_projectInfoMap.clear();

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_USER_PROJECTS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_FILTER, ot::JsonString(_projectNameFilter, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_LIMIT, _maxNumberOfResults + 1, doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->slotShowErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return false;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	for (const ot::ConstJsonObject& projObj : ot::json::getObjectList(responseDoc, OT_ACTION_PARAM_List))
	{
		ot::ProjectInformation newInfo(projObj);
		_projectsFound.push_back(newInfo);
		m_projectInfoMap[newInfo.getProjectName()] = newInfo;
	}

	size_t size = _projectsFound.size();
	while (size > _maxNumberOfResults) {
		_maxLengthExceeded = true;
		_projectsFound.pop_back();
		size--;
	}

	return (!_projectsFound.empty());
}

bool ProjectManagement::createNewCollection(const std::string &collectionName, const std::string &defaultSettingTemplate)
{
	if (!InitializeConnection()) return false;

	// Store the default setting template (if any)
	if (!defaultSettingTemplate.empty())
	{
		auto doc = bsoncxx::builder::basic::document{};

		doc.append(bsoncxx::builder::basic::kvp("SchemaType", "DefaultTemplate"));
		doc.append(bsoncxx::builder::basic::kvp("TemplateName", defaultSettingTemplate));

		DataStorageAPI::DocumentAccess docManager(m_dataBaseName, collectionName);

		DataStorageAPI::DataStorageResponse res = docManager.InsertDocumentToDatabase(doc.view(), false);
		assert(res.getSuccess());
	}

	return true;
}

bool ProjectManagement::InitializeConnection(void)
{
	if (m_isConnected) {
		return true;
	}

	try {
		AppBase * app{ AppBase::instance() };
		DataStorageAPI::ConnectionAPI::establishConnection(m_databaseURL, app->getCurrentLoginData().getSessionUser(), app->getCurrentLoginData().getSessionPassword());

		// Now we run a command on the server and check whether its is really responding to us (the following command throws an exception if not)
		m_isConnected = DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(m_dataBaseName, m_projectCatalogCollectionName);

		if (!m_isConnected) {
			// It might be that the catalog has not been created yet. Try to create the catalog.
			DataStorageAPI::ConnectionAPI::getInstance().createCollection(m_dataBaseName, m_projectCatalogCollectionName);
			m_isConnected = DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(m_dataBaseName, m_projectCatalogCollectionName);
		}

		return m_isConnected;
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
		return false; // Connection failed
	}
}

ot::ProjectInformation ProjectManagement::getProjectInformation(const std::string& _projectName) {
	auto it = m_projectInfoMap.find(_projectName);
	if (it == m_projectInfoMap.end()) {
		return ot::ProjectInformation();
	}
	else {
		return it->second;
	}
}

bool ProjectManagement::readProjectsInfo(std::list<std::string>& _projects) {
	assert(!m_authServerURL.empty());

	m_projectInfoMap.clear();

	AppBase * app{ AppBase::instance() };

	std::list<std::string> validProjects;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_PROJECT_INFO, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAMES, ot::JsonArray(_projects, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->slotShowErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return false;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	for (const ot::ConstJsonObject& projObj : ot::json::getObjectList(responseDoc, OT_ACTION_PARAM_List))
	{
		ot::ProjectInformation newInfo(projObj);
		validProjects.push_back(newInfo.getProjectName());
		m_projectInfoMap[newInfo.getProjectName()] = newInfo;
	}

	// Now loop through all projects and check the validity

	UserManagement uManager;
	bool uManagerInitialized = false;

	for (const std::string& projectName : _projects) {
		if (m_projectInfoMap.find(projectName) == m_projectInfoMap.end()) {
			// Remove the project from the recent projects list
			if (!uManagerInitialized) {
				uManager.setAuthServerURL(m_authServerURL);
				uManager.setDatabaseURL(m_databaseURL);
				
				bool checkConnection = uManager.checkConnection();
				assert(checkConnection); // Connect and check

				uManagerInitialized = true;
			}

			uManager.removeRecentProject(projectName);
		}
	}

	_projects = validProjects;

	return true;
}

bool ProjectManagement::copyProject(const std::string &sourceProjectName, const std::string &destinationProjectName, const std::string &userName)
{
	assert(sourceProjectName != destinationProjectName);

	std::string sourceProjectType = getProjectType(sourceProjectName);

	// Create a new project with the given name and the same type as the source project
	if (!createProject(destinationProjectName, sourceProjectType, userName, "")) return false;

	// Now get the collection names of the source and destination projects
	std::string sourceProjectCollection = getProjectCollection(sourceProjectName);
	std::string destinationProjectCollection = getProjectCollection(destinationProjectName);

	// Copy the project collection
	copyCollection(sourceProjectCollection, destinationProjectCollection);

	// Copy large data collections
	copyCollection(sourceProjectCollection + ".files", destinationProjectCollection + ".files");
	copyCollection(sourceProjectCollection + ".chunks", destinationProjectCollection + ".chunks");

	// Copy result collection
	copyCollection(sourceProjectCollection + ".results", destinationProjectCollection + ".results");

	return true;
}

void ProjectManagement::copyCollection(const std::string& sourceCollectionName, const std::string& destinationCollectionName)
{
	if (!DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(m_dataBaseName, sourceCollectionName)) return;

	// Get the collection pointers for both projects
	auto sourceCollection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(m_dataBaseName, sourceCollectionName);

	// Copy the content by using the collection.aggregate method

	mongocxx::pipeline stages;

	stages.match({});
	stages.out(destinationCollectionName);

	auto cursor = sourceCollection.aggregate(stages, mongocxx::options::aggregate{});
	auto count = std::distance(cursor.begin(), cursor.end());
}

std::vector<std::string> ProjectManagement::getDefaultTemplateList(void)
{
	std::vector<std::string> templateList;
	if (!InitializeConnection()) return templateList;

	mongocxx::database db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("ProjectTemplates");

	templateList = db.list_collection_names();

	return templateList;
}

void deleteData(std::uint8_t* mem)
{
	delete[] mem;
}

std::string ProjectManagement::exportProject(const std::string &projectName, const std::string &exportFileName, AppBase *parent)
{
	if (!InitializeConnection()) return "Unable to connect to the database";

	// Get the project collection
	std::string collectionName = getProjectCollection(projectName);
	if (collectionName.empty()) return "Unable to determine the project data to be exported";  // The collection name could not be found, so we cannot continue

																							   // Get the collection
	if (!DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(m_dataBaseName, collectionName)) return "Unable to access the project data to be exported";

	auto collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(m_dataBaseName, collectionName);

	size_t numberResultDocuments = 0;
	if (DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(m_dataBaseName, collectionName + ".results"))
	{
		auto resultCollection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(m_dataBaseName, collectionName + ".results");
		numberResultDocuments = resultCollection.count_documents({});
	}

	// Now we read all data base items one by one and write them in binary mode to the output file.

	std::ofstream exportFile;

	try
	{
		exportFile.open(exportFileName, std::ios::binary);
	}
	catch (std::exception &e)
	{
		return "Unable to open the export file for writing (" + std::string(e.what()) + ")";
	}

	size_t fileVersion = 3;
	exportFile.write((const char *)&fileVersion, sizeof(size_t));

	// In export file version 2, we also export the project type
	std::string projectType = getProjectType(projectName);

	size_t numberCharacters = projectType.size();
	exportFile.write((const char*) &numberCharacters, sizeof(size_t));
	exportFile.write((const char*)projectType.c_str(), numberCharacters);

	// Write the number of data documents
	size_t numberDocuments = collection.count_documents({});
	exportFile.write((const char*)&numberDocuments, sizeof(size_t));

	// In export file version 3, we also export the number of result documents (and the documents themselves later)
	exportFile.write((const char*)&numberResultDocuments, sizeof(size_t));

	char internalStorage = 1;
	char gridStorage     = 2;
	char gridStorageAsc = 3;

	size_t documentCount = 0;
	int oldPercent = -1;

	auto cursor = collection.find({});

	try
	{
		for (auto&& doc : cursor)
		{
			try
			{
				auto insertType = doc["InsertType"].get_int32().value;

				if (InsertType(insertType) == InsertType::GridFS)
				{
					// The data is stored in the grid fs

					exportFile.write((const char *)&gridStorage, sizeof(char));

					// Now we store the grid fs data first
					auto fileId = doc["FileId"].get_value();

					std::uint8_t *data = nullptr;
					size_t length = 0;

					DataStorageAPI::DocumentAPI fsDoc;
					fsDoc.GetDocumentUsingGridFs(fileId, data, length);

					// Now store the length and the data in the export file
					exportFile.write((const char *)&length, sizeof(size_t));
					exportFile.write((const char *)data, length);

					// afterward we store the item
					const uint8_t *buffer = reinterpret_cast<const uint8_t *>(doc.data());
					size_t size = doc.length();

					exportFile.write((const char *)&size, sizeof(size_t));
					exportFile.write((const char *)buffer, size);
				}
				else if (InsertType(insertType) == InsertType::GridFSAsc)
				{
					// The data is stored in the grid fs

					exportFile.write((const char *)&gridStorageAsc, sizeof(char));

					// Now we store the grid fs data first
					auto fileId = doc["FileId"].get_value();

					std::uint8_t *data = nullptr;
					size_t length = 0;

					DataStorageAPI::DocumentAPI fsDoc;
					fsDoc.GetDocumentUsingGridFs(fileId, data, length, collectionName);

					// Now store the length and the data in the export file
					exportFile.write((const char *)&length, sizeof(size_t));
					exportFile.write((const char *)data, length);

					// afterward we store the item
					const uint8_t *buffer = reinterpret_cast<const uint8_t *>(doc.data());
					size_t size = doc.length();

					exportFile.write((const char *)&size, sizeof(size_t));
					exportFile.write((const char *)buffer, size);
				}
				else
				{
					// The data is stored in the data base
					const uint8_t *buffer = reinterpret_cast<const uint8_t *>(doc.data());
					size_t size = doc.length();

					exportFile.write((const char *)&internalStorage, sizeof(char));
					exportFile.write((const char *)&size, sizeof(size_t));
					exportFile.write((const char *)buffer, size);
				}
			}
			catch (std::exception)
			{
				// We do not have an insert type, so the data is stored in the data base
				const uint8_t *buffer = reinterpret_cast<const uint8_t *>(doc.data());
				size_t size = doc.length();

				exportFile.write((const char *)&internalStorage, sizeof(char));
				exportFile.write((const char *)&size, sizeof(size_t));
				exportFile.write((const char *)buffer, size);
			}

			documentCount++;

			int percent = std::min(100, (int)(100.0 * documentCount / (numberDocuments + numberResultDocuments)));
			if (percent > oldPercent)
			{
				oldPercent = percent;
				QMetaObject::invokeMethod(parent, &AppBase::slotSetProgressBarValue, Qt::QueuedConnection, percent);
			}
		}
	}
	catch (std::exception &e)
	{
		exportFile.close();
		return "Unable to write data to the export file (" + std::string(e.what()) + ")";
	}

	// Now we export the result data collection, if any
	if (numberResultDocuments > 0)
	{
		auto collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(m_dataBaseName, collectionName + ".results");

		auto cursor = collection.find({});

		try
		{
			for (auto&& doc : cursor)
			{
				// The data is always stored in the data base
				const uint8_t* buffer = reinterpret_cast<const uint8_t*>(doc.data());
				size_t size = doc.length();

				exportFile.write((const char*)&size, sizeof(size_t));
				exportFile.write((const char*)buffer, size);
			}

			documentCount++;

			int percent = std::min(100, (int)(100.0 * documentCount / (numberDocuments + numberResultDocuments)));
			if (percent > oldPercent)
			{
				oldPercent = percent;
				QMetaObject::invokeMethod(parent, &AppBase::slotSetProgressBarValue, Qt::QueuedConnection, percent);
			}
		}
		catch (std::exception& e)
		{
			exportFile.close();
			return "Unable to write result data to the export file (" + std::string(e.what()) + ")";
		}
	}

	exportFile.close();
	return "";
}

std::string ProjectManagement::importProject(const std::string &projectName, const std::string &userName, const std::string &importFileName, AppBase *parent)
{
	// Open the import file
	std::ifstream importFile;

	try
	{
		importFile.open(importFileName, std::ios::binary);
	}
	catch (std::exception &e)
	{
		return "Unable to open the project file file for reading (" + std::string(e.what()) + ")";
	}

	try
	{
		size_t fileVersion = 0;
		importFile.read((char *)&fileVersion, sizeof(size_t));
		if (fileVersion > 3) return "Wrong file version of the import data file: " + std::to_string(fileVersion) + " (only supported up to 3)";

		std::string projectType = OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT;

		if (fileVersion >= 2)
		{
			// Read the project type from the data file
			size_t numberCharacters = 0;
			importFile.read((char*)&numberCharacters, sizeof(size_t));

			char* buffer = new char[numberCharacters + 1];
			importFile.read(buffer, numberCharacters);
			buffer[numberCharacters] = 0;

			projectType = buffer;
		}

		if (!createProject(projectName, projectType, userName, std::string(""))) return "Unable to create the new project: " + projectName;

		// Now get the collection
		// Get the project collection
		std::string collectionName = getProjectCollection(projectName);
		if (collectionName.empty()) return "Unable to determine the project data location for import";  // The collection name could not be found, so we cannot continue

		auto collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(m_dataBaseName, collectionName);
	
		// Import the data from the file

		size_t numberDocuments = collection.count_documents({});
		importFile.read((char *)&numberDocuments, sizeof(size_t));

		size_t numberResultDocuments = 0;
		if (fileVersion >= 3)
		{
			importFile.read((char*)&numberResultDocuments, sizeof(size_t));
		}

		// Read the items in the import file until we reach the end of the file

		char internalStorage = 1;
		char gridStorage = 2;
		char gridStorageAsc = 3;

		size_t docCount = 0;
		int oldPercent = -1;

		for (size_t index = 0; index < numberDocuments; index++)
		{
			char dataType = 0;
			importFile.read((char *)&dataType, sizeof(char));

			if (dataType == internalStorage)
			{
				// The document is stored directly in the data base
				size_t length = 0;
				importFile.read((char *)&length, sizeof(size_t));

				std::uint8_t *data = new std::uint8_t[length];
				importFile.read((char *)data, length);

				bsoncxx::document::value dataDoc(data, length, deleteData);

				collection.insert_one(dataDoc.view());
			}
			else if (dataType == gridStorage)
			{
				// The document needs to be stored in grid fs
				size_t dataLength = 0;
				importFile.read((char *)&dataLength, sizeof(size_t));

				// Load the data block
				std::uint8_t *data = new std::uint8_t[dataLength];
				importFile.read((char *)data, dataLength);

				bsoncxx::document::value dataDoc(data, dataLength, deleteData);

				// Now read the document
				size_t metaLength = 0;
				importFile.read((char *)&metaLength, sizeof(size_t));

				std::uint8_t *metaData = new std::uint8_t[metaLength];
				importFile.read((char *)metaData, metaLength);

				bsoncxx::document::value metaDoc(metaData, metaLength, deleteData);

				// Now we need to store the data in the grid fs
				DataStorageAPI::DocumentAPI fsDoc;
				bsoncxx::types::value result = fsDoc.InsertDocumentUsingGridFs(dataDoc.view(), collectionName);

				// Next we modify the fileId in the document

				auto fileMetaData = bsoncxx::builder::basic::document{};
				
				long long entityID = metaDoc.view()["EntityID"].get_int64();
				long long entityVersion;

				bsoncxx::document::element versionElement = metaDoc.view()["Version"];
				if (versionElement.type() == bsoncxx::type::k_int32) {
					entityVersion = (long long)versionElement.get_int32();
				}
				else {
					entityVersion = versionElement.get_int64();
				}
				
				auto objectID = metaDoc.view()["_id"].get_oid();

				fileMetaData.append(
					bsoncxx::builder::basic::kvp("_id", objectID),
					bsoncxx::builder::basic::kvp("EntityID", (long long)entityID),
					bsoncxx::builder::basic::kvp("Version", (long long)entityVersion),
					bsoncxx::builder::basic::kvp("InsertType", static_cast<int32_t>(InsertType::GridFS)),
					bsoncxx::builder::basic::kvp("FileId", result.get_oid()));

				// Finally, store the document
				collection.insert_one(fileMetaData.view());
			}
			else if (dataType == gridStorageAsc)
			{
				// The document needs to be stored in grid fs
				size_t dataLength = 0;
				importFile.read((char *)&dataLength, sizeof(size_t));

				// Load the data block
				std::uint8_t *data = new std::uint8_t[dataLength];
				importFile.read((char *)data, dataLength);

				bsoncxx::document::value dataDoc(data, dataLength, deleteData);

				// Now read the document
				size_t metaLength = 0;
				importFile.read((char *)&metaLength, sizeof(size_t));

				std::uint8_t *metaData = new std::uint8_t[metaLength];
				importFile.read((char *)metaData, metaLength);

				bsoncxx::document::value metaDoc(metaData, metaLength, deleteData);

				auto fileMetaData = bsoncxx::builder::basic::document{};
				
				// Now we need to store the data in the grid fs
				DataStorageAPI::DocumentAPI fsDoc;
				bsoncxx::types::value result = fsDoc.InsertDocumentUsingGridFs(dataDoc.view(), collectionName, fileMetaData);
				
				// Next we modify the fileId in the document
				long long entityID = metaDoc.view()["EntityID"].get_int64();
				long long entityVersion;

				bsoncxx::document::element versionElement = metaDoc.view()["Version"];
				if (versionElement.type() == bsoncxx::type::k_int32) {
					entityVersion = (long long)versionElement.get_int32();
				}
				else {
					entityVersion = versionElement.get_int64();
				}

				auto objectID = metaDoc.view()["_id"].get_oid();

				fileMetaData.append(
					bsoncxx::builder::basic::kvp("_id", objectID),
					bsoncxx::builder::basic::kvp("EntityID", (long long)entityID),
					bsoncxx::builder::basic::kvp("Version", (long long)entityVersion),
					bsoncxx::builder::basic::kvp("InsertType", static_cast<int32_t>(InsertType::GridFSAsc)),
					bsoncxx::builder::basic::kvp("FileId", result.get_oid()));

				// Finally, store the document
				collection.insert_one(fileMetaData.view());
			}
			else
			{
				if (!importFile.eof())
				{
					// Unknown data type
					return "Unknown data identifier in the import file (file corrupted?)";
				}
			}

			docCount++;

			int percent = std::min(100, (int)(100.0 * docCount / (numberDocuments + numberResultDocuments)));
			if (percent > oldPercent)
			{
				oldPercent = percent;
				QMetaObject::invokeMethod(parent, &AppBase::slotSetProgressBarValue, Qt::QueuedConnection, percent);
			}
		}

		if (numberResultDocuments > 0)
		{
			// Try to load the result data
			auto collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(m_dataBaseName, collectionName + ".results");

			std::list<bsoncxx::builder::basic::document*> cachedDocuments;

			for (size_t index = 0; index < numberResultDocuments; index++)
			{
				// The document is stored directly in the data base
				size_t length = 0;
				importFile.read((char*)&length, sizeof(size_t));

				std::uint8_t* data = new std::uint8_t[length];
				importFile.read((char*)data, length);

				bsoncxx::document::value dataDoc(data, length, deleteData);

				bsoncxx::builder::basic::document* doc = new bsoncxx::builder::basic::document{};
				doc->append(bsoncxx::builder::concatenate(dataDoc.view()));
				cachedDocuments.push_back(doc);

				if (cachedDocuments.size() == 100)
				{
					flushCachedDocuments(cachedDocuments, collection);
				}

				docCount++;

				int percent = std::min(100, (int)(100.0 * docCount / (numberDocuments + numberResultDocuments)));
				if (percent > oldPercent)
				{
					oldPercent = percent;
					QMetaObject::invokeMethod(parent, &AppBase::slotSetProgressBarValue, Qt::QueuedConnection, percent);
				}
			}

			flushCachedDocuments(cachedDocuments,collection);
		}
	}
	catch (std::exception &e)
	{
		importFile.close();
		return "Unable to store data from import file in database (" + std::string(e.what()) + ")";
	}

	importFile.close();
	return "";
}

void ProjectManagement::flushCachedDocuments(std::list<bsoncxx::builder::basic::document*>& cachedDocuments, mongocxx::collection &collection)
{
	if (cachedDocuments.empty()) return;

	std::vector<bsoncxx::document::value> documents;
	documents.reserve(cachedDocuments.size());

	for (auto doc : cachedDocuments)
	{
		documents.push_back(doc->extract());
	}

	collection.insert_many(documents);

	for (auto doc : cachedDocuments)
	{
		delete doc;
	}

	cachedDocuments.clear();
}

bool ProjectManagement::canAccessProject(const std::string &projectCollection) {
	if (projectCollection.empty()) {
		return false;
	}

	try {
		auto collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(m_dataBaseName, projectCollection);
		size_t numberDocuments = collection.count_documents({});
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
		return false;
	}

	return true;
}
