/*
 * UserManagement.cpp
 *
 *  Created on: September 28, 2020
 *	Author: Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

// uiService Header
#include "UserManagement.h"		// Corresponding header
#include "AppBase.h"

// bson and mongo DB header
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/database.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "DocumentAPI.h"
#include "Connection\ConnectionAPI.h"
#include "Document\DocumentAccess.h"
#include "Document\DocumentAccessBase.h"
#include "Helper\QueryBuilder.h"
#include "Helper\BsonValuesHelper.h"

#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

std::string UserManagement::userSettingsCollection;

UserManagement::UserManagement() :
	maxNumberRecentProjects(10),
	isConnected(false),
	settingsDataBaseName("UserSettings")
{

}

UserManagement::UserManagement(const LoginData& _loginData)
	: maxNumberRecentProjects(10),
	isConnected(false),
	settingsDataBaseName("UserSettings"),
	authServerURL(_loginData.getAuthorizationUrl()),
	databaseURL(_loginData.getDatabaseUrl())
{

}

UserManagement::~UserManagement()
{

}

void UserManagement::setAuthServerURL(const std::string &url)
{
	authServerURL = url;
}

void UserManagement::setDatabaseURL(const std::string &url)
{
	databaseURL = url;
}

bool UserManagement::checkConnection(void) const {
	AppBase * app{ AppBase::instance() };

	if (!checkConnectionAuthorizationService()) return false;
	if (!checkConnectionDataBase(app->getCurrentLoginData().getSessionUser(), app->getCurrentLoginData().getSessionPassword())) return false;

	return true;
}

bool UserManagement::checkConnectionAuthorizationService(void) const {
	assert(!authServerURL.empty());

	// Here send a ping to the authorization service to check whether the server is up and running
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_PING, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}

	return true;
}

bool UserManagement::checkConnectionDataBase(const std::string &userName, const std::string &password) const {
	assert(!databaseURL.empty());

	if (!isConnected)
	{
		try
		{
			DataStorageAPI::ConnectionAPI::establishConnection(databaseURL, "1", userName, password);
		}
		catch (std::exception e)
		{
			OT_LOG_E("Error connecting to database: " + string(e.what()));
			return false; // Connection failed
		}
	}

	// Now we check whether the data base server is actually running
	if (!DataStorageAPI::ConnectionAPI::getInstance().checkServerIsRunning())
	{
		return false;
	}

	return true;
}
void UserManagement::initializeNewSession(void)
{
	userSettingsCollection.clear();
}

bool UserManagement::addUser(const std::string &userName, const std::string &password)
{
	assert(!authServerURL.empty());

	// Here we register a new user by sending a message to the authorization service

	// Now we try to register the new user
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_REGISTER, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(userName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(password, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}

	// Now we check the response document
	return !hasError(response);
}

bool UserManagement::deleteUser(const std::string &userName)
{
	assert(!authServerURL.empty());

	// Here we delete a user by sending a message to the authorization service

	// First, we check whether the user exists, if not the function fails.
	if (!checkUserName(userName))
	{
		return false;
	}

	// Now we try to delete the user

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_DELETE_USER, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(userName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}

	// Now we check the response document
	assert(0);
	return true;
}

//bool UserManagement::changePassword(const std::string &oldPassword, const std::string &newPassword)
//{
//	// We always expect the old and the new password to be unencrypted.
//
//	assert(!authServerURL.empty());
//
//	// Here we change a user password by sending a message to the authorization service
//	AppBase * app{ AppBase::instance() };
//
//	// First, we check whether the old password is correct
//	std::string validPassword, validEncryptedPassword;
//
//	if (!checkPassword(app->getCredentialUserName(), oldPassword, false, validPassword, validEncryptedPassword))
//	{
//		return false;
//	}	
//
//	// Now we try to change the password
//	ot::JsonDocument doc;
//	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CHANGE_USER_PASSWORD, doc.GetAllocator()), doc.GetAllocator());
//	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCredentialUserName(), doc.GetAllocator()), doc.GetAllocator());
//	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCredentialUserPasswordClear(), doc.GetAllocator()), doc.GetAllocator());
//	doc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(newPassword, doc.GetAllocator()), doc.GetAllocator());
//
//	std::string response;
//	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
//	{
//		return false;
//	}
//
//	// Now we check the response document
//	assert(0);
//	return true;
//}


bool UserManagement::checkUserName(const std::string &userName) const {
	assert(!authServerURL.empty());

	// Here we check whether a user exists by getting its data from the authorization service

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_USER_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(userName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}

	// Now we check the response document
	return hasSuccessful(response);
}

bool UserManagement::checkPassword(const std::string &userName, const std::string &password, bool isEncryptedPassword, std::string &sessionUser, std::string& sessionPassword, std::string &validPassword, std::string &validEncryptedPassword) const {
	assert(!authServerURL.empty());

	// Here we check whether a user exists by getting its data from the authorization service

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_LOGIN, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(userName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(password, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_ENCRYPTED_PASSWORD, isEncryptedPassword, doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		return false;
	}

	// Now we check the response document
	if (hasSuccessful(response))
	{
		ot::JsonDocument responseDoc;
		responseDoc.fromJson(response);

		// Login attempt successful -> get encrypted and unencrypted passwords
		sessionUser            = ot::json::getString(responseDoc, OT_PARAM_DB_USERNAME);
		sessionPassword        = ot::json::getString(responseDoc, OT_PARAM_DB_PASSWORD);
		validPassword          = ot::json::getString(responseDoc, OT_PARAM_AUTH_PASSWORD);
		validEncryptedPassword = ot::json::getString(responseDoc, OT_PARAM_AUTH_ENCRYPTED_PASSWORD);

		return true; // Successful
	}
	
	return false; // Login attempt unsuccessful
}

bool UserManagement::hasError(const std::string &response)
{
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	// Check whether the document has an error flag
	return ot::json::exists(responseDoc, OT_ACTION_AUTH_ERROR);
}

bool UserManagement::hasSuccessful(const std::string& _response) const {
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(_response);

	try
	{
		bool success = ot::json::getBool(responseDoc, OT_ACTION_AUTH_SUCCESS);
		return success;
	}
	catch (std::exception)
	{
		return false; // The return document does not have a success flag
	}
}

bool UserManagement::initializeDatabaseConnection(void)
{
	if (isConnected) return true;

	try
	{
		AppBase * app{ AppBase::instance() };
		DataStorageAPI::ConnectionAPI::establishConnection(databaseURL, "1", app->getCurrentLoginData().getSessionUser(), app->getCurrentLoginData().getSessionPassword());

		isConnected = true;

		return isConnected; 
	}
	catch (std::exception&)
	{
		return false; // Connection failed
	}
}

bool UserManagement::storeSetting(const std::string &settingName, const std::string &settingString)
{
	if (!ensureSettingsCollectionCanBeAccessed()) return false;

	// First, check whether the settings is already there 
	DataStorageAPI::DocumentAccess docManager(settingsDataBaseName, userSettingsCollection);

	// Now we search for the document with the given name

	auto queryDoc = bsoncxx::builder::basic::document{};
	queryDoc.append(bsoncxx::builder::basic::kvp("SettingName", settingName));

	auto filterDoc = bsoncxx::builder::basic::document{};

	auto result = docManager.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));

	if (!result.getSuccess())
	{
		// The setting does not yet exist -> write a new one
		auto newDoc = bsoncxx::builder::basic::document{};
		newDoc.append(bsoncxx::builder::basic::kvp("SettingName", settingName));
		newDoc.append(bsoncxx::builder::basic::kvp("Data", settingString));

		docManager.InsertDocumentToDatabase(newDoc.extract(), false);
	}
	else
	{
		// The setting already exists -> replace the settings

		try
		{
			// Find the entry corresponding to the project in the collection
			auto doc_find = bsoncxx::builder::stream::document{} << "SettingName" << settingName << bsoncxx::builder::stream::finalize;

			auto doc_modify = bsoncxx::builder::stream::document{}
				<< "$set" << bsoncxx::builder::stream::open_document
				<< "Data" << settingString
				<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

			mongocxx::collection collection = DataStorageAPI::ConnectionAPI::getInstance().getCollection(settingsDataBaseName, userSettingsCollection);

			bsoncxx::stdx::optional<mongocxx::result::update> result = collection.update_many(doc_find.view(), doc_modify.view());
		}
		catch (std::exception)
		{
			assert(0); // Could not update the documents
		}
	}

	return true;
}

std::string UserManagement::restoreSetting(const std::string &settingName)
{
	if (!ensureSettingsCollectionCanBeAccessed()) return "";

	DataStorageAPI::DocumentAccess docManager(settingsDataBaseName, userSettingsCollection);

	// Now we search for the document with the given name

	auto queryDoc = bsoncxx::builder::basic::document{};
	queryDoc.append(bsoncxx::builder::basic::kvp("SettingName", settingName));

	auto filterDoc = bsoncxx::builder::basic::document{};

	auto result = docManager.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));

	if (!result.getSuccess()) return "";

	try
	{
		bsoncxx::builder::basic::document doc;
		doc.append(bsoncxx::builder::basic::kvp("Found", result.getBsonResult().value()));

		auto doc_view = doc.view()["Found"].get_document().view();

		std::string value = doc_view["Data"].get_utf8().value.data();

		return value;
	}
	catch (std::exception)
	{
	}

	return "";
}

bool UserManagement::ensureSettingsCollectionCanBeAccessed(void)
{
	if (userSettingsCollection.empty())
	{
		userSettingsCollection = getUserSettingsCollection();
		if (userSettingsCollection.empty()) return false;
	}

	if (!initializeDatabaseConnection()) return false;

	bool collectionExists = false;

	try
	{
		// Now we run a command on the server and check whether its is really responding to us (the following command throws an exception if not)
		collectionExists = DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(settingsDataBaseName, userSettingsCollection);

		if (!collectionExists)
		{
			DataStorageAPI::ConnectionAPI::getInstance().createCollection(settingsDataBaseName, userSettingsCollection);
			collectionExists = DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists(settingsDataBaseName, userSettingsCollection);
		}
	}
	catch (std::exception&)
	{
		collectionExists = false;
	}

	return collectionExists;
}

std::string UserManagement::getUserSettingsCollection(void)
{
	assert(!authServerURL.empty());

	// Here we check whether a user exists by getting its data from the authorization service

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_USER_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		OT_LOG_E("Failed to send message");
		return "ERROR: Failed to send message";
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::string collectionName;

	try
	{
		collectionName = ot::json::getString(responseDoc, OT_ACTION_SETTINGS_COLLECTION_NAME);
	}
	catch (std::exception)
	{
	}

	return collectionName;
}

bool UserManagement::addRecentProject(const std::string &projectName)
{
	// First get the current list of recent projects
	std::list<std::string> recentProjects;
	getListOfRecentProjects(recentProjects);

	// Now check whether the first project in the list is already the current one
	if (!recentProjects.empty())
	{
		if (recentProjects.front() == projectName) return true;  // In this case, we don't need to do anything

		// The current project may already be in the list at a later position. Delete it.
		recentProjects.remove(projectName);
	}

	// Now add the current project to the top of the list
	recentProjects.push_front(projectName);

	// Now the list may be to long, so we need to delete the last entries
	while (recentProjects.size() > maxNumberRecentProjects)
	{
		recentProjects.pop_back();
	}

	// Now store the project list in the data base
	return storeListOfRecentProjects(recentProjects);
}

bool UserManagement::removeRecentProject(const std::string &projectName)
{
	// First get the current list of recent projects
	std::list<std::string> recentProjects;
	getListOfRecentProjects(recentProjects);

	// Delete the specified project from the list of recent projects
	recentProjects.remove(projectName);

	// Now store the project list in the data base
	return storeListOfRecentProjects(recentProjects);

	return true;
}

void UserManagement::getListOfRecentProjects(std::list<std::string> &recentProjectList)
{
	recentProjectList.clear();

	std::string recentProjects = restoreSetting("RecentProjects");
	if (recentProjects.empty()) return;

	try
	{
		ot::JsonDocument doc;
		doc.fromJson(recentProjects);
		recentProjectList = ot::json::getStringList(doc, "Names");
	}
	catch (std::exception)
	{
	}
}

bool UserManagement::storeListOfRecentProjects(std::list<std::string> &recentProjectList)
{
	ot::JsonDocument doc;
	doc.AddMember("Names", ot::JsonArray(recentProjectList, doc.GetAllocator()), doc.GetAllocator());

	storeSetting("RecentProjects", doc.toJson());

	return true;
}
