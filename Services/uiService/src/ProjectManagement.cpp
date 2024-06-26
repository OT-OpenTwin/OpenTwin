/*
 * ProjectManagement.h
 *
 *	Author: Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

#include "ProjectManagement.h"
#include "UserManagement.h"
#include "AppBase.h"
#include "DataBase.h"

#include <iomanip>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pipeline.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "DocumentAPI.h"
#include "Connection\ConnectionAPI.h"
#include "Document\DocumentAccess.h"
#include "Document\DocumentAccessBase.h"
#include "Helper\QueryBuilder.h"
#include "Helper\BsonValuesHelper.h"

#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTCore/ReturnMessage.h"

ProjectManagement::ProjectManagement() :
	isConnected(false),
	dataBaseName("Projects"),
	projectCatalogCollectionName("Catalog")
{

}

ProjectManagement::ProjectManagement(const LoginData& _data) 
	: isConnected(false),
	dataBaseName("Projects"),
	projectCatalogCollectionName("Catalog"),
	databaseURL(_data.getDatabaseUrl()),
	authServerURL(_data.getAuthorizationUrl())
{

}

ProjectManagement::~ProjectManagement()
{

}

void ProjectManagement::setDataBaseURL(const std::string &url)
{
	databaseURL = url;
}

void ProjectManagement::setAuthServerURL(const std::string &url)
{
	authServerURL = url;
}

bool ProjectManagement::createProject(const std::string &projectName, const std::string& projectType, const std::string &userName, const std::string &defaultSettingTemplate)
{
	assert(!authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CREATE_PROJECT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_TYPE, ot::JsonString(projectType, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::string collectionName;

	try
	{
		collectionName = ot::json::getString(responseDoc, OT_PARAM_AUTH_PROJECT_COLLECTION);
	}
	catch (std::exception)
	{
	}

	return createNewCollection(collectionName, defaultSettingTemplate);
}

bool ProjectManagement::deleteProject(const std::string &projectName)
{
	assert(!authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_REMOVE_PROJECT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}

	return hasSuccessful(response);
}

bool ProjectManagement::renameProject(const std::string &oldProjectName, const std::string &newProjectName)
{
	assert(!authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CHANGE_PROJECT_NAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(oldProjectName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_NEW_PROJECT_NAME, ot::JsonString(newProjectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}

	return !hasError(response);
}

bool ProjectManagement::projectExists(const std::string &projectName, bool &canBeDeleted)
{
	assert(!authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
 	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}
		
	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);
	if(responseMessage == ot::ReturnMessage::Failed)
	{
		return false;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(responseMessage.getWhat());

	canBeDeleted = false;
	try
	{
		std::string owner = ot::json::getString(responseDoc, OT_PARAM_AUTH_OWNER);

		if (owner == app->getCurrentLoginData().getUserName())
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
	assert(!authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return "";
	}

	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);
	if (responseMessage == ot::ReturnMessage::Failed)
	{
		return "";
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(responseMessage.getWhat());

	std::string collectionName;

	try
	{
		collectionName = ot::json::getString(responseDoc, OT_PARAM_AUTH_PROJECT_COLLECTION);
	}
	catch (std::exception)
	{
	}

	return collectionName;
}

std::string ProjectManagement::getProjectType(const std::string& projectName)
{
	assert(!authServerURL.empty());

	AppBase* app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		OT_LOG_E("Request failed");
		return "";
	}

	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);
	if (responseMessage == ot::ReturnMessage::Failed)
	{
		OT_LOG_E(responseMessage.getWhat());
		return "";
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(responseMessage.getWhat());

	std::string projectType;

	try
	{
		projectType = ot::json::getString(responseDoc, OT_PARAM_AUTH_PROJECT_TYPE);
	}
	catch (const std::exception& _e)
	{
		OT_LOG_E(_e.what());
	}

	return projectType;
}

bool ProjectManagement::findProjectNames(const std::string &projectNameFilter, int maxNumberOfResults, std::list<std::string> &projectsFound, bool &maxLengthExceeded)
{
	assert(!authServerURL.empty());

	projectsFound.clear();
	projectAuthorMap.clear();

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_USER_PROJECTS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_FILTER, ot::JsonString(projectNameFilter, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_LIMIT, maxNumberOfResults + 1, doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	const rapidjson::Value& projectArray = responseDoc["projects"];
	assert(projectArray.IsArray());

	for (rapidjson::Value::ConstValueIterator itr = projectArray.Begin(); itr != projectArray.End(); ++itr)
	{
		const rapidjson::Value& project = *itr;
		std::string projectData = project.GetString();

		ot::JsonDocument projectDoc;
		projectDoc.fromJson(projectData);

		std::string projectName = projectDoc[OT_PARAM_AUTH_NAME].GetString();
		std::string owner = projectDoc[OT_PARAM_AUTH_OWNER].GetString();

		projectsFound.push_back(projectName);
		projectAuthorMap[projectName] = owner;
	}

	if (projectsFound.size() > maxNumberOfResults)
	{
		maxLengthExceeded = true;
		projectsFound.pop_back();
	}

	return (!projectsFound.empty());
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

		DataStorageAPI::DocumentAccess docManager(dataBaseName, collectionName);

		DataStorageAPI::DataStorageResponse res = docManager.InsertDocumentToDatabase(doc.view(), false);
		assert(res.getSuccess());
	}

	return true;
}

bool ProjectManagement::InitializeConnection(void)
{
	if (isConnected) return true;

	try
	{
		AppBase * app{ AppBase::instance() };
		DataStorageAPI::ConnectionAPI::establishConnection(databaseURL, "1", app->getCurrentLoginData().getSessionUser(), app->getCurrentLoginData().getSessionPassword());

		// Now we run a command on the server and check whether its is really responding to us (the following command throws an exception if not)
		isConnected = DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(dataBaseName, projectCatalogCollectionName);

		if (!isConnected)
		{
			// It might be that the catalog has not been created yet. Try to create the catalog.
			DataStorageAPI::ConnectionAPI::getInstance().createCollection(dataBaseName, projectCatalogCollectionName);
			isConnected = DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(dataBaseName, projectCatalogCollectionName);
		}

		return isConnected; 
	}
	catch (std::exception&)
	{
		return false; // Connection failed
	}
}

bool ProjectManagement::getProjectAuthor(const std::string &projectName, std::string &author)
{
	if (projectAuthorMap.count(projectName) > 0)
	{
		author = projectAuthorMap[projectName];
	}

	return true;
}

bool ProjectManagement::readProjectAuthor(std::list<std::string> &projects)
{
	assert(!authServerURL.empty());

	projectAuthorMap.clear();

	AppBase * app{ AppBase::instance() };

	std::list<std::string> validProjects;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_PROJECT_OWNERS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAMES, ot::JsonArray(projects, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	const rapidjson::Value& projectArray = responseDoc[ "projects" ];
	assert(projectArray.IsArray());

	for (rapidjson::Value::ConstValueIterator itr = projectArray.Begin(); itr != projectArray.End(); ++itr)
	{
		const rapidjson::Value& project = *itr;
		std::string projectData = project.GetString();

		ot::JsonDocument projectDoc;
		projectDoc.fromJson(projectData);

		std::string projectName = projectDoc[OT_PARAM_AUTH_NAME].GetString();
		std::string owner = projectDoc[OT_PARAM_AUTH_OWNER].GetString();

		validProjects.push_back(projectName);

		projectAuthorMap[projectName] = owner;
	}

	// Now loop through all projects and check the validity

	UserManagement uManager;
	bool uManagerInitialized = false;

	for (auto projectName : projects)
	{
		if (projectAuthorMap.count(projectName) == 0)
		{
			// Remove the project from the recent projects list
			if (!uManagerInitialized)
			{
				uManager.setAuthServerURL(authServerURL);
				uManager.setDatabaseURL(databaseURL);
				bool checkConnection = uManager.checkConnection(); assert(checkConnection); // Connect and check

				uManagerInitialized = true;
			}

			uManager.removeRecentProject(projectName);
		}
	}

	projects = validProjects;

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

	// Get the collection pointers for both projects
	auto sourceCollection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(dataBaseName, sourceProjectCollection);

	// Copy the content by using the collection.aggregate method

	mongocxx::pipeline stages;

	stages.match({});
	stages.out(destinationProjectCollection);

	auto cursor = sourceCollection.aggregate(stages, mongocxx::options::aggregate{});
	auto count = std::distance(cursor.begin(), cursor.end());

	return true;
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
	if (!DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(dataBaseName, collectionName)) return "Unable to access the project data to be exported";

	auto collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(dataBaseName, collectionName);

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

	size_t fileVersion = 2;
	exportFile.write((const char *)&fileVersion, sizeof(size_t));

	// In export file version 2, we also export the project type
	std::string projectType = getProjectType(projectName);

	size_t numberCharacters = projectType.size();
	exportFile.write((const char*) &numberCharacters, sizeof(size_t));
	exportFile.write((const char*)projectType.c_str(), numberCharacters);

	// Write the number of data documents
	size_t numberDocuments = collection.count_documents({});
	exportFile.write((const char *)&numberDocuments, sizeof(size_t));

	char internalStorage = 1;
	char gridStorage     = 2;
	char gridStorageAsc = 3;

	size_t documentCount = 0;

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

			int percent = std::min(100, (int)(100.0 * documentCount / numberDocuments));
			QMetaObject::invokeMethod(parent, "setProgressBarValue", Qt::QueuedConnection, Q_ARG(int, percent));
		}
	}
	catch (std::exception &e)
	{
		exportFile.close();
		return "Unable to write data to the export file (" + std::string(e.what()) + ")";
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
		if (fileVersion > 2) return "Wrong file version of the import data file: " + std::to_string(fileVersion) + " (only supported up to 1)";

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

		auto collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(dataBaseName, collectionName);
	
		// Import the data from the file

		size_t numberDocuments = collection.count_documents({});
		importFile.read((char *)&numberDocuments, sizeof(size_t));

		// Read the items in the import file until we reach the end of the file

		char internalStorage = 1;
		char gridStorage = 2;
		char gridStorageAsc = 3;

		size_t docCount = 0;

		while (!importFile.eof())
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

			int percent = std::min(100, (int)(100.0 * docCount / numberDocuments));
			QMetaObject::invokeMethod(parent, "setProgressBarValue", Qt::QueuedConnection, Q_ARG(int, percent));
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

bool ProjectManagement::canAccessProject(const std::string &projectCollection)
{
	if (projectCollection.empty()) return false;

	try
	{
		auto collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(dataBaseName, projectCollection);
		size_t numberDocuments = collection.count_documents({});
	}
	catch (std::exception &e)
	{
		return false;
	}

	return true;
}
