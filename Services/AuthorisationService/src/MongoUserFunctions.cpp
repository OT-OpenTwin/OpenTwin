#include "MongoUserFunctions.h"
#include "MongoRoleFunctions.h"
#include "MongoGroupFunctions.h"
#include "MongoProjectFunctions.h"
#include "MongoConstants.h"
#include "MongoURL.h"

#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"

#include <boost/uuid/namespaces.hpp>
#include <boost/uuid/name_generator_md5.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <set>

/*
	These functions must cover the following functionalities
	1) Register ✅
	2) Fetch User Data ✅
	3) Fetch All Users ✅
	4) Remove User ✅
	5) Update Username ✅ (Not fully working, the _id field CANNOT BE CHANGED)
	6) Change User Password  ✅
*/

std::set<std::string> authenticatedUserTokens;

namespace MongoUserFunctions
{
	bool authenticateUser(std::string username, std::string password, std::string databaseUrl, mongocxx::client& adminClient)
	{
		// This authentication may take some time. Therefore, we want to cache the credentials to speed up subsequent checks
		std::string token = username + "\n" + password + "\n" + databaseUrl;
		if (authenticatedUserTokens.find(token) != authenticatedUserTokens.end()) return true;

		auto userCollection = adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::USER_CATALOG_COLLECTION);

		value userFilter = document{}
			<< "user_name" << username
			<< finalize;

		auto userDocument = userCollection.find_one(userFilter.view());

		if (!userDocument)
		{
			return false; // User not found
		}

		const std::string storedPassword(userDocument->view()["user_pwd"].get_utf8().value.data());
		const std::string hashedPassword = hashPassword(password);

		if (storedPassword.empty())
		{
			changeUserPassword(username, password, adminClient);
		}
		else
		{
			if (storedPassword != hashedPassword) return false;
		}

		authenticatedUserTokens.insert(token);
		
		return true;
	}

	bool registerUser(std::string username, std::string password, mongocxx::client& adminClient, std::string oldSettingsCollectionName)
	{
		/*
			Creation of user will be performed through a helping collection that keeps the incrementing id numbers
			The ID will be fetched and the user will be created with a generated username (user + currentId)
			After this user is created, then his username will be updated to the real username that the user has defined
		*/

		/*
		"{\n" +
			"  \"createUser\": \"new_user\",\n" +
			"  \"pwd\": \"123\",    \n" +
			"  \"customData\": { \"profile_image\": \"test\" },\n" +
			"  \"roles\": [\n" +
			"    { \"role\": \"test_role\", \"db\":\"test_db\" } \n" +
			"  ]\n" +
			"}"
		*/

		// A user will be created and this user will have several initial roles
		// 1) Project Role - which gives him the rights to all project metadata (catalog)
		// 2) Group Role - which gives him the rights to groups
		// 3) ProjectTemplates Role
		// 4) ProjectsLargeDate role
		// 5) SystemDb role
		// 6) SettingsDb role

		// Check whether the user already exists
		if (doesUserExist(username, adminClient))
		{
			throw std::runtime_error("This user name is already in use.");
		}

		// The user will also have as custom data his settingsCollectionName (where the personalized settings are defined)

		std::string settingsCollectionName;

		if (oldSettingsCollectionName == "")
		{
			settingsCollectionName = generateUserSettingsCollectionName(adminClient);
		}
		else
		{
			// When re-registering a user through the change username workaround.
			settingsCollectionName = oldSettingsCollectionName;
		}

		// The user id will be automatically assigned by MongoDB based on the name of the user. Therefore, we first create
		// a unique id and create a user with this name. This will also assign the id. Afterward, we rename the user. This will
		// keep the unique id, but set the correct name.

		// Create a unique user id
		std::string uniqueName = "user" + settingsCollectionName.substr(std::string("Settings").length());
		std::string userRoleName = uniqueName + "_role";

		value userDocument = document{}
			<< "user_id" << uniqueName
			<< "user_name" << username
			<< "user_pwd" << hashPassword(password)
			<< "user_role_name" << userRoleName
			<< "settings_collection_name" << settingsCollectionName
			<< finalize;

		mongocxx::database userDB = adminClient.database(MongoConstants::USER_DB); 

		auto result = userDB.collection(MongoConstants::USER_CATALOG_COLLECTION).insert_one(userDocument.view());

		if (!result)
		{
			return false;
		}

		// Now we create a new role for this user without permissions
		MongoRoleFunctions::createUserRole(userRoleName, adminClient);

		// Now add the mandatory roles to this user role
		MongoRoleFunctions::addRoleToUserRole(MongoConstants::PROJECT_CATALOG_ROLE, userRoleName, adminClient);
		MongoRoleFunctions::addRoleToUserRole(MongoConstants::PROJECT_DB_LIST_COLLECTIONS_ROLE, userRoleName, adminClient);
		MongoRoleFunctions::addRoleToUserRole(MongoConstants::GROUP_ROLE, userRoleName, adminClient);
		MongoRoleFunctions::addRoleToUserRole(MongoConstants::PROJECT_TEMPLATES_ROLE, userRoleName, adminClient);
		MongoRoleFunctions::addRoleToUserRole(MongoConstants::PROJECTS_LARGE_DATA_ROLE, userRoleName, adminClient);
		MongoRoleFunctions::addRoleToUserRole(MongoConstants::SYSTEM_DB_ROLE, userRoleName, adminClient);
		MongoRoleFunctions::addRoleToUserRole(MongoConstants::SETTINGS_DB_ROLE, userRoleName, adminClient);

		return true;
	}

	std::string hashPassword(const std::string& password)
	{
		using namespace boost::uuids;

		name_generator_md5 gen(ns::x500dn());

		uuid u1 = gen(password);

		std::string result = boost::uuids::to_string(u1);
		return result;
	}

	std::string generateUserSettingsCollectionName(mongocxx::client& adminClient)
	{
		std::time_t currentTime = std::time(nullptr);
		struct tm currentDate;
		localtime_s(&currentDate, &currentTime);
		std::ostringstream collectionName;
		int random = rand() % 100;

		collectionName << "Settings-" << 1900 + currentDate.tm_year << currentDate.tm_mon + 1 << currentDate.tm_mday << "-"
			<< currentDate.tm_hour << currentDate.tm_min << currentDate.tm_sec << "-" << random;

		bool done = false;

		while (!done)
		{
			done = true;
			mongocxx::database settingsDb = adminClient.database(MongoConstants::SETTINGS_DB);

			auto cursor1 = settingsDb.list_collections();
			for (const bsoncxx::document::view& doc : cursor1)
			{
				bsoncxx::document::element ele = doc["name"];
				std::string name(ele.get_utf8().value.data());
				if (name == collectionName.str())
				{
					// This collection name is already in use
					random = rand() % 100;
					collectionName << random;

					done = false;
					break;
				}
			}
		}

		return collectionName.str();
	}

	User getUserDataThroughUsername(const std::string& username, mongocxx::client& adminClient)
	{
		auto userCollection = adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::USER_CATALOG_COLLECTION);

		value filter = document{}
			<< "user_name" << username
			<< finalize;

		auto userData = userCollection.find_one(filter.view());

		if (!userData.has_value())
		{
			throw std::runtime_error("User not found");

		}

		User user(userData.value());

		return user;
	}

	User getUserDataThroughId(std::string userId, mongocxx::client& adminClient)
	{
		auto userCollection = adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::USER_CATALOG_COLLECTION);

		value filter = document{}
			<< "user_id" << userId
			<< finalize;

		auto userData = userCollection.find_one(filter.view());

		if (!userData)
		{
			throw std::runtime_error("User not found");
		}

		User user(userData->view());

		return user;
	}

	std::vector<User> getAllUsers(mongocxx::client& adminClient)
	{
		auto cursor = adminClient.database(MongoConstants::USER_DB)
			.collection(MongoConstants::USER_CATALOG_COLLECTION)
			.find({});

		std::vector<User> userList{};

		for (view userData : cursor)
		{
			try
			{
				User tmpUser(userData);

				userList.push_back(std::move(tmpUser));
			}
			catch (std::runtime_error err)
			{
				// Something went wrong with this user -> may have incomplete information, so let's skip this user
			}
		}

		return userList;
	}


	size_t getAllUserCount(mongocxx::client& adminClient)
	{
		size_t count = adminClient.database(MongoConstants::USER_DB)
								.collection(MongoConstants::USER_CATALOG_COLLECTION)
								.count_documents({});

		return count;
	}


	bool removeUser(User& userToBeDeleted, mongocxx::client& adminClient)
	{
		std::string username = userToBeDeleted.username;

		// 1 remove the user from each project 
		value filter = document{} <<
			"users" << userToBeDeleted.userId
			<< finalize;

		value update = document{} <<
			"$pull"
			<< open_document
			<< "users" << userToBeDeleted.userId
			<< close_document
			<< finalize;


		auto project_result = adminClient.database(MongoConstants::PROJECTS_DB)
			.collection(MongoConstants::PROJECT_CATALOG_COLLECTION)
			.update_many(filter.view(), update.view());

		// 2 remove the user from each group

		auto group_result = adminClient.database(MongoConstants::PROJECTS_DB)
			.collection(MongoConstants::GROUPS_COLLECTION)
			.update_many(filter.view(), update.view());


		// Delete the user completely
		auto userFilter = document{} <<
			"user_id" << userToBeDeleted.userId
			<< finalize;

		adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::USER_CATALOG_COLLECTION).delete_one(userFilter.view());

		// Dropping this user's settings collection.
		adminClient[MongoConstants::SETTINGS_DB][userToBeDeleted.settingsCollectionName].drop();

		authenticatedUserTokens.clear();

		return true;
	}

	bool doesUserExist(const std::string &userName, mongocxx::client& adminClient)
	{
		value usernameFilter = document{}
			<< "user_name" << userName
			<< finalize;

		auto usersCollection = adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::USER_CATALOG_COLLECTION);
		auto userOptional = usersCollection.find_one(usernameFilter.view());

		if (userOptional)
		{
			return true;
		}

		return false;
	}

	bool updateUserUsernameById(std::string userId, std::string newUsername, mongocxx::client& adminClient)
	{
		if (doesUserExist(newUsername, adminClient))
		{
			return false;
		}

		value userFilter = document{}
			<< "user_id" << userId
			<< finalize;

		value userUpdate = document{}
			<< "$set"
			<< open_document
			<< "user_name" << newUsername
			<< close_document
			<< finalize;

		auto usersCollection = adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::USER_CATALOG_COLLECTION);
		auto result = usersCollection.update_one(userFilter.view(), userUpdate.view());

		authenticatedUserTokens.clear();

		if (!result.has_value())
		{
			throw std::exception("Update username failed.");
		}

		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();

		if (matchedCount == 1 && modifiedCount == 1)
		{
			return true;
		}

		return false;
	}

	bool updateUserUsernameByName(std::string oldUsername, std::string newUsername, mongocxx::client& adminClient)
	{
		if (doesUserExist(newUsername, adminClient))
		{
			return false;
		}

		value userFilter = document{}
			<< "user_name" << oldUsername
			<< finalize;

		value userUpdate = document{}
			<< "$set"
			<< open_document
			<< "user_name" << newUsername
			<< close_document
			<< finalize;

		auto usersCollection = adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::USER_CATALOG_COLLECTION);
		auto result = usersCollection.update_one(userFilter.view(), userUpdate.view());

		authenticatedUserTokens.clear();

		if (!result.has_value())
		{
			throw std::exception("Updata username failed.");
		}

		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();

		if (matchedCount == 1 && modifiedCount == 1)
		{
			return true;
		}

		return false;
	}

	bool changeUserPassword(std::string username, std::string newPassword, mongocxx::client& adminClient)
	{
		value userFilter = document{}
			<< "user_name" << username
			<< finalize;

		value userUpdate = document{}
			<< "$set"
			<< open_document
			<< "user_pwd" << hashPassword(newPassword)
			<< close_document
			<< finalize;

		auto usersCollection = adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::USER_CATALOG_COLLECTION);
		auto result = usersCollection.update_one(userFilter.view(), userUpdate.view());

		authenticatedUserTokens.clear();

		if (!result.has_value())
		{
			throw std::exception("Change userpassword failed.");
		}


		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();

		if (matchedCount == 1 && modifiedCount == 1)
		{
			return true;
		}

		return false;
	}

	bool updateUsername(std::string oldUsername, std::string newUsername, mongocxx::client& adminClient)
	{
		if (doesUserExist(newUsername, adminClient))
		{
			return false;
		}

		value userFilter = document{}
			<< "user_name" << oldUsername
			<< finalize;

		value userUpdate = document{}
			<< "$set"
			<< open_document
			<< "user_name" << newUsername
			<< close_document
			<< finalize;

		auto usersCollection = adminClient.database(MongoConstants::USER_DB).collection(MongoConstants::USER_CATALOG_COLLECTION);
		auto result = usersCollection.update_one(userFilter.view(), userUpdate.view());

		authenticatedUserTokens.clear();

		if (!result.has_value())
		{
			throw std::exception("Updata username failed.");
		}

		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();

		if (matchedCount == 1 && modifiedCount == 1)
		{
			return true;
		}

		return false;
	}

	std::string userToJson(User& user)
	{
		ot::JsonDocument json;
		json.AddMember("username", ot::JsonString(user.username, json.GetAllocator()), json.GetAllocator());
		json.AddMember("settingsCollectionName", ot::JsonString(user.settingsCollectionName, json.GetAllocator()), json.GetAllocator());

		return json.toJson();
	}

	std::string usersToJson(std::vector<User>& users)
	{
		std::list<std::string> jsonUsers{};

		for (auto user : users)
		{
			jsonUsers.push_back(userToJson(user));
		}

		ot::JsonDocument json;
		json.AddMember("users", ot::JsonArray(jsonUsers, json.GetAllocator()), json.GetAllocator());
		return json.toJson();
	}

	void createTmpUser(std::string userName, std::string userPWD, User &_loggedInUser, mongocxx::client& adminClient, ot::JsonDocument &json)
	{
		value new_user_command = document{}
			<< "createUser" << userName
			<< "pwd" << userPWD

			<< "roles"
			<< open_array

			<< open_document
			<< "role" << _loggedInUser.roleName
			<< "db" << "admin"
			<< close_document

			<< close_array
			<< finalize;

		mongocxx::database db = adminClient.database(MongoConstants::ADMIN_DB); // The admin db must be used

		value command_result = db.run_command(new_user_command.view());

		// Getting elements from this document:
		element el = command_result.view()["ok"];

		if (el.get_double() == 1)
		{
			json.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(userName, json.GetAllocator()), json.GetAllocator());
			json.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(userPWD, json.GetAllocator()), json.GetAllocator());
		}
	}

	void removeTmpUser(std::string userName, mongocxx::client & adminClient)
	{
		// Delete the user completely
		value drop_user_command = document{} <<
			"dropUser" << userName
			<< finalize;

		mongocxx::database db = adminClient.database(MongoConstants::ADMIN_DB); // The admin db must be used

		try
		{
			value command_result = db.run_command(drop_user_command.view());
		}
		catch (std::exception)
		{
			assert(0);
		}
	}
}

