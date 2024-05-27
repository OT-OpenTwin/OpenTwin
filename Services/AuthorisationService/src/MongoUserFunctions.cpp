#include "MongoUserFunctions.h"
#include "MongoGroupFunctions.h"
#include "MongoProjectFunctions.h"
#include "MongoURL.h"
#include "OTCore/JSON.h"

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
	bool authenticateUser(std::string username, std::string password, std::string databaseUrl)
	{
		// This authentication may take some time. Therefore, we want to cache the credentials to speed up subsequent checks
		std::string token = username + "\n" + password + "\n" + databaseUrl;
		if (authenticatedUserTokens.find(token) != authenticatedUserTokens.end()) return true;

		std::string uriStr = getMongoURL(databaseUrl, username, password);

		mongocxx::uri uri(uriStr);
		mongocxx::client currentUserClient = mongocxx::client(uri);

		mongocxx::database db = currentUserClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection PROJECT_CATALOG_COLLECTION = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		try
		{
			// Querying one document to check the credentials
			bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
				PROJECT_CATALOG_COLLECTION.find_one({});
		}
		catch (std::runtime_error err)
		{
			std::cout << err.what() << std::endl;

			return false;
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

		value new_user_command = document{}
			<< "createUser" << uniqueName
			<< "pwd" << password

			<< "customData" 
			<< open_document
			<< "settingsCollectionName" << settingsCollectionName
			<< close_document

			<< "roles"
			<< open_array

			<< open_document
			<< "role" << MongoConstants::PROJECT_CATALOG_ROLE
			<< "db" << "admin"
			<< close_document

			<< open_document
			<< "role" << MongoConstants::PROJECT_DB_LIST_COLLECTIONS_ROLE
			<< "db" << "admin"
			<< close_document

			<< open_document
			<< "role" << MongoConstants::GROUP_ROLE
			<< "db" << "admin"
			<< close_document

			<< open_document
			<< "role" << MongoConstants::PROJECT_TEMPLATES_ROLE
			<< "db" << "admin"
			<< close_document

			<< open_document
			<< "role" << MongoConstants::PROJECTS_LARGE_DATA_ROLE
			<< "db" << "admin"
			<< close_document

			<< open_document
			<< "role" << MongoConstants::SYSTEM_DB_ROLE
			<< "db" << "admin"
			<< close_document

			<< open_document
			<< "role" << MongoConstants::SETTINGS_DB_ROLE
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
			if (!updateUsername(uniqueName, username, adminClient))
			{
				auto user = getUserDataThroughUsername(uniqueName, adminClient);
				removeUser(user, adminClient);
				return false;
			}

			return true;
		}

		return false;
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
				std::string name = ele.get_utf8().value.to_string();
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




	User getUserDataThroughUsername(std::string username, mongocxx::client& adminClient)
	{
		mongocxx::database adminDb = adminClient.database(MongoConstants::ADMIN_DB);

		value filter = document{}
			<< "user" << username
			<< finalize;

		User user{};

		try
		{
			auto userDataOptional = adminClient.database(MongoConstants::ADMIN_DB)
				.collection(MongoConstants::SYSTEM_USERS_COLLECTION)
				.find_one(filter.view());

			if (!userDataOptional)
			{
				throw std::runtime_error("User not found");
			}

			user._id = userDataOptional.get().view()["_id"].get_utf8().value.to_string();
			user.username = username;

			auto settingsCollOpt = userDataOptional.get().view()["customData"]["settingsCollectionName"];

			if (settingsCollOpt)
			{
				user.settingsCollectionName =settingsCollOpt.get_utf8().value.to_string();
			}

			user.setDocumentValue(userDataOptional.get());
		}
		catch (std::runtime_error err)
		{
			std::string error = err.what();
		}

		return user;
	}

	User getUserDataThroughId(bsoncxx::types::b_binary& id, mongocxx::client& adminClient)
	{
		value filter = document{}
			<< "userId" << id
			<< finalize;

		auto userDataOptional = adminClient.database(MongoConstants::ADMIN_DB)
			.collection(MongoConstants::SYSTEM_USERS_COLLECTION)
			.find_one(filter.view());

		User user{};
		user._id = userDataOptional.get().view()["_id"].get_utf8().value.to_string();
		user.username = userDataOptional.get().view()["user"].get_utf8().value.to_string();

		auto settingsCollOpt = userDataOptional.get().view()["customData"]["settingsCollectionName"];

		if (settingsCollOpt)
		{
			user.settingsCollectionName = settingsCollOpt.get_utf8().value.to_string();
		}

		user.setDocumentValue(userDataOptional.get());

		return user;
	}


	std::vector<User> getAllUsers(mongocxx::client& adminClient)
	{
		value filter = document{} << finalize;


		auto cursor = adminClient.database(MongoConstants::ADMIN_DB)
			.collection(MongoConstants::SYSTEM_USERS_COLLECTION)
			.find(filter.view());

		std::vector<User> userList{};


		for (view userData : cursor)
		{
			std::string userId = userData["_id"].get_utf8().value.to_string();

			// Skip the Admin
			if (userId == "admin.admin")
			{
				continue;
			}

			try
			{
				User tmpUser;
				tmpUser._id = userId;
				tmpUser.username = userData["user"].get_utf8().value.to_string();
				tmpUser.settingsCollectionName = userData["customData"]["settingsCollectionName"].get_utf8().value.to_string();

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
		size_t count = adminClient.database(MongoConstants::ADMIN_DB)
								.collection(MongoConstants::SYSTEM_USERS_COLLECTION)
								.count_documents({});

		return count-1;  // We need to subtract the admin user since we only want to get the number of the "normal" users.
	}


	bool removeUser(User& userToBeDeleted, mongocxx::client& adminClient)
	{
		std::string username = userToBeDeleted.username;


		// 1 remove the user from each project 
		value filter = document{} <<
			"users" << userToBeDeleted.getUserId()
			<< finalize;

		value update = document{} <<
			"$pull"
			<< open_document
			<< "users" << userToBeDeleted.getUserId()
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
		value drop_user_command = document{} <<
			"dropUser" << username
			<< finalize;

		auto command_result = adminClient[MongoConstants::ADMIN_DB].run_command(drop_user_command.view());

		// Dropping this user's settings collection.
		adminClient[MongoConstants::SETTINGS_DB][userToBeDeleted.settingsCollectionName].drop();

		authenticatedUserTokens.clear();

		return true;
	}

	bool doesUserExist(const std::string &userName, mongocxx::client& adminClient)
	{
		value usernameFilter = document{}
			<< "user" << userName
			<< finalize;
		auto usersCollection = adminClient.database(MongoConstants::ADMIN_DB).collection(MongoConstants::SYSTEM_USERS_COLLECTION);
		auto userOptional = usersCollection.find_one(usernameFilter.view());

		if (userOptional)
		{
			return true;
		}

		return false;
	}

	bool updateUserUsername(bsoncxx::types::b_binary userId, std::string oldPassword, std::string newUsername, mongocxx::client& adminClient)
	{
		value usernameFilter = document{}
			<< "user" << newUsername
			<< finalize;
		auto usersCollection = adminClient.database(MongoConstants::ADMIN_DB).collection(MongoConstants::SYSTEM_USERS_COLLECTION);
		auto userOptional = usersCollection.find_one(usernameFilter.view());

		if (userOptional)
		{
			throw std::runtime_error("This username is already taken! Please choose another.");
		}

		User oldUser = getUserDataThroughId(userId, adminClient);

		bool registerSuccessful = registerUser(newUsername, oldPassword, adminClient, oldUser.settingsCollectionName);
		if (!registerSuccessful)
		{
			return false;
		}

		User newUser = getUserDataThroughUsername(newUsername, adminClient);

		// The new user is created, now the old user's data must be given to him.
		// First we check every group where this user is part of, or is the owner of.

		std::vector<Group> oldUserGroups = MongoGroupFunctions::getAllUserGroups(oldUser, adminClient);

		for (auto gr : oldUserGroups)
		{
			if (gr.getOwnerId() == oldUser.getUserId()) // If the old User is the owner, make as owner the new one
			{
				MongoGroupFunctions::changeGroupOwner(gr, oldUser, newUser, adminClient);
			}
			else
			{
				MongoGroupFunctions::addUserToGroup(newUser, gr.name, adminClient); // If he's not the owner, then just add him to the group
			}
			MongoGroupFunctions::removeUserFromGroup(oldUser, gr.name, adminClient); // Remove the old one from the group since his ID will be completely deleted
		}


		// Now we deal with all the projects that this user has
		std::vector<Project> oldUserProjects = MongoProjectFunctions::getAllUserProjects(oldUser,"", -1, adminClient);


		for (auto pr : oldUserProjects)
		{
			if (pr.creatingUser.getUserId() == oldUser.getUserId())
			{
				MongoProjectFunctions::changeProjectCreator(pr._id, oldUser, newUser, adminClient);
			}
		}

		removeUser(oldUser, adminClient);

		authenticatedUserTokens.clear();

		return true;
	}

	bool updateUserUsername(std::string oldUsername, std::string oldPassword, std::string newUsername, mongocxx::client& adminClient)
	{

		value usernameFilter = document{}
			<< "user" << newUsername
			<< finalize;
		auto usersCollection = adminClient.database(MongoConstants::ADMIN_DB).collection(MongoConstants::SYSTEM_USERS_COLLECTION);
		auto userOptional = usersCollection.find_one(usernameFilter.view());

		if (userOptional)
		{
			throw std::runtime_error("This username is already taken! Please choose another.");
		}

		User oldUser = getUserDataThroughUsername(oldUsername, adminClient);

		bool registerSuccessful = registerUser(newUsername, oldPassword, adminClient, oldUser.settingsCollectionName);
		if (!registerSuccessful)
		{
			return false;
		}

		User newUser = getUserDataThroughUsername(newUsername, adminClient);

		// The new user is created, now the old user's data must be given to him.
		// First we check every group where this user is part of, or is the owner of.

		std::vector<Group> oldUserGroups = MongoGroupFunctions::getAllUserGroups(oldUser, adminClient);

		for (auto gr : oldUserGroups)
		{
			if (gr.getOwnerId() == oldUser.getUserId()) // If the old User is the owner, make as owner the new one
			{
				MongoGroupFunctions::changeGroupOwner(gr, oldUser, newUser, adminClient);
			}
			else
			{
				MongoGroupFunctions::addUserToGroup(newUser, gr.name, adminClient); // If he's not the owner, then just add him to the group
			}
			MongoGroupFunctions::removeUserFromGroup(oldUser, gr.name, adminClient); // Remove the old one from the group since his ID will be completely deleted
		}


		// Now we deal with all the projects that this user has
		std::vector<Project> oldUserProjects = MongoProjectFunctions::getAllUserProjects(oldUser, "", -1, adminClient);


		for (auto pr : oldUserProjects)
		{
			if (pr.creatingUser.getUserId() == oldUser.getUserId())
			{
				MongoProjectFunctions::changeProjectCreator(pr._id, oldUser, newUser, adminClient);
			}
		}

		removeUser(oldUser, adminClient);

		authenticatedUserTokens.clear();

		return true;

	}

	bool changeUserPassword(std::string username, std::string newPassword, mongocxx::client& adminClient)
	{
		// db.updateUser("root", { pwd: "NewRootAdmin" })

		value changePasswordCommand = document{}
			<< "updateUser" << username
			<< "pwd" << newPassword
			<< finalize;

		mongocxx::database db = adminClient.database(MongoConstants::ADMIN_DB); // The admin db must be used

		value command_result = db.run_command(changePasswordCommand.view());

		// Getting elements from this document:
		element el = command_result.view()["ok"];

		authenticatedUserTokens.clear();

		if (el.get_double() == 1)
		{
			return true;
		}

		return false;
	}

	bool updateUsername(std::string oldUsername, std::string newUsername, mongocxx::client& adminClient)
	{
		value usernameFilter = document{}
			<< "user" << newUsername
			<< finalize;
		auto usersCollection = adminClient.database(MongoConstants::ADMIN_DB).collection(MongoConstants::SYSTEM_USERS_COLLECTION);
		auto userOptional = usersCollection.find_one(usernameFilter.view());

		if (userOptional)
		{
			throw std::runtime_error("This username is already taken! Please choose another.");
		}

		value filter = document{}
			<< "user" << oldUsername
			<< finalize;

		value changes = document{}
			<< "$set"
			<< open_document
			<< "user" << newUsername
			<< close_document
			<< finalize;

		auto result = usersCollection.update_one(filter.view(), changes.view());

		int32_t matchedCount = result.get().matched_count();
		int32_t modifiedCount = result.get().modified_count();

		authenticatedUserTokens.clear();

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
}
