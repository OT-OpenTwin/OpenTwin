#include "MongoProjectFunctions.h"
#include "MongoGroupFunctions.h"
#include "MongoRoleFunctions.h"
#include "MongoConstants.h"
#include "MongoUserFunctions.h"



/*
* The following functionalities have to be implemented
* 1) Create Project
* 2) Get all user projects
* 3) Get all group projects
* 6) Add group to project
* 7) Remove group from project
* 8) Delete project
*/
namespace MongoProjectFunctions
{

	Project createProject(std::string projectName, User& creatingUser, mongocxx::client& adminClient)
	{

		mongocxx::database db = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);


		// Create the Collection for the project data
		std::string collectionName = generateProjectCollectionName(adminClient);
		std::string roleName = collectionName + "_role";





		// Step one creating the Project Document and inserting it in the Projects collection
		value new_project_document = document{}
			<< "project_name" << projectName
			<< "created_on" << bsoncxx::types::b_date(std::chrono::system_clock::now())
			<< "created_by" << creatingUser.getUserId()
			<< "collection_name" << collectionName
			<< "project_role_name" << roleName
			<< "groups" << open_array << close_array
			<< "version" << 1
			<< "last_accessed_on" << bsoncxx::types::b_date(std::chrono::system_clock::now())
			<< finalize;

		auto result = projectCollection.insert_one(new_project_document.view());

		// Project was inserted

		// Create a role for this project. This role will give the rights to a specific user to have access to this Project Collection (not catalog)

		// Role creation
		MongoRoleFunctions::createProjectRole(roleName, collectionName, adminClient);

		// adding created Role to this user
		MongoRoleFunctions::addRoleToUser(roleName, creatingUser.username, adminClient);

		return getProject(projectName, adminClient);
	}

	std::string generateProjectCollectionName(mongocxx::client& adminClient)
	{
		std::time_t currentTime = std::time(nullptr);
		struct tm currentDate;
		localtime_s(&currentDate, &currentTime);
		std::ostringstream collectionName;
		int random = rand() % 100;

		collectionName << "Data-" << 1900 + currentDate.tm_year << currentDate.tm_mon + 1 << currentDate.tm_mday << "-"
			<< currentDate.tm_hour << currentDate.tm_min << currentDate.tm_sec << "-" << random;

		bool done = false;

		while (!done)
		{
			try
			{
				getProject(collectionName.str(), adminClient);

				// This collection name is already in use
				random = rand() % 100;
				collectionName << random;

			}
			catch (std::runtime_error err)
			{
				done = true;
			}
		}

		return collectionName.str();
	}


	Project getProject(bsoncxx::oid& projectId, mongocxx::client& userClient)
	{
		mongocxx::database secondaryDb = userClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectDataCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		value filter = document{}
			<< "_id" << projectId
			<< finalize;

		auto optional = projectDataCollection.find_one(filter.view());

		Project tmpProject{};

		if (!optional)
		{
			return tmpProject;
		}

		value projectValue = optional.get();

		tmpProject._id = projectValue.view()["_id"].get_oid().value;
		tmpProject.name = projectValue.view()["project_name"].get_utf8().value.to_string();
		tmpProject.roleName = projectValue.view()["project_role_name"].get_utf8().value.to_string();
		tmpProject.collectionName = projectValue.view()["collection_name"].get_utf8().value.to_string();
		tmpProject.createdOn = projectValue.view()["created_on"].get_date();
		tmpProject.lastAccessedOn = projectValue.view()["last_accessed_on"].get_date();
		tmpProject.version = projectValue.view()["version"].get_int32().value;


		bsoncxx::types::b_binary userId = projectValue.view()["created_by"].get_binary();

		tmpProject.creatingUser = MongoUserFunctions::getUserDataThroughId(userId, userClient);

		auto groupsArr = projectValue.view()["groups"].get_array().value;

		for (auto groupId : groupsArr)
		{
			bsoncxx::oid id = groupId.get_oid().value;

			Group currentGroup = MongoGroupFunctions::getGroupData(id, userClient);

			tmpProject.groups.push_back(currentGroup);
		}

		return tmpProject;
	}

	Project getProject(std::string projectName, mongocxx::client& userClient)
	{
		mongocxx::database secondaryDb = userClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectDataCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		value filter = document{}
			<< "project_name" << projectName
			<< finalize;

		auto optional = projectDataCollection.find_one(filter.view());

		if (!optional)
		{
			throw std::runtime_error("Project was not found");
		}

		value projectValue = optional.get();

		Project tmpProject{};

		tmpProject._id = projectValue.view()["_id"].get_oid().value;
		tmpProject.name = projectValue.view()["project_name"].get_utf8().value.to_string();
		tmpProject.roleName = projectValue.view()["project_role_name"].get_utf8().value.to_string();
		tmpProject.collectionName = projectValue.view()["collection_name"].get_utf8().value.to_string();
		tmpProject.createdOn = projectValue.view()["created_on"].get_date();
		tmpProject.lastAccessedOn = projectValue.view()["last_accessed_on"].get_date();
		tmpProject.version = projectValue.view()["version"].get_int32(); // TODO: CHECK THIS 

		bsoncxx::types::b_binary userId = projectValue.view()["created_by"].get_binary();

		tmpProject.creatingUser = MongoUserFunctions::getUserDataThroughId(userId, userClient);

		auto groupsArr = projectValue.view()["groups"].get_array().value;

		for (auto groupId : groupsArr)
		{
			bsoncxx::oid id = groupId.get_oid().value;

			Group currentGroup = MongoGroupFunctions::getGroupData(id, userClient);

			tmpProject.groups.push_back(currentGroup);
		}

		return tmpProject;
	}

	std::string getProjectOwners(std::list<std::string> projectNames, mongocxx::client& adminClient)
	{
		auto array_builder = bsoncxx::builder::basic::array{};

		for (const auto& projectName : projectNames) {
			array_builder.append(projectName);
		}

		auto inArr = document{} << "$in" << array_builder << finalize;

		auto filter = document{} <<
			"project_name" << inArr
			<< finalize;

		mongocxx::database db = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		mongocxx::cursor cursor = projectsCollection.find(filter.view());

		std::vector<Project> projects{};
		for (auto doc : cursor)
		{
			// Research std::move() 

			Project tmpProject{};
			tmpProject._id = doc["_id"].get_oid().value;
			tmpProject.name = doc["project_name"].get_utf8().value.to_string();
			tmpProject.roleName = doc["project_role_name"].get_utf8().value.to_string();
			tmpProject.collectionName = doc["collection_name"].get_utf8().value.to_string();
			tmpProject.createdOn = doc["created_on"].get_date();

			bsoncxx::types::b_binary creatingUserId = doc["created_by"].get_binary();
			tmpProject.creatingUser = MongoUserFunctions::getUserDataThroughId(creatingUserId, adminClient);

			projects.push_back(std::move(tmpProject));
		}

		return projectsToJson(projects);

	}

	std::vector<Project> getAllUserProjects(User& loggedInUser, std::string filter, int limit, mongocxx::client& userClient)
	{
		std::vector<Group> userGroups = MongoGroupFunctions::getAllUserGroups(loggedInUser, userClient);

		auto array_builder = bsoncxx::builder::basic::array{};

		for (const auto& group : userGroups) {
			array_builder.append(group._id);
		}

		auto inArr = document{} << "$in" << array_builder << finalize;


		std::string regexFilter = ".*" + filter + ".*";
		auto regex = bsoncxx::types::b_regex(regexFilter, "");

		
		value filterDoc = document{}
			<< "$and"
			<< open_array

			<< open_document 
			<< "project_name" << regex
			<< close_document
			
			<< open_document

			<< "$or"
			<< open_array
			<< open_document
			<< "created_by" << loggedInUser.getUserId()
			<< close_document

			<< open_document
			<< "groups" << inArr
			<< close_document

			<< close_array
			<< close_document

			<< close_array
			<< finalize;


		mongocxx::database db = userClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		mongocxx::pipeline p{};
		p.match(filterDoc.view());
		if (limit > 0)
		{
			p.limit(limit);
		}


		mongocxx::cursor cursor = projectsCollection.aggregate(p);

		std::vector<Project> projects{};
		for (auto doc : cursor)
		{
			// Research std::move() 

			Project tmpProject{};
			tmpProject._id = doc["_id"].get_oid().value;
			tmpProject.name = doc["project_name"].get_utf8().value.to_string();
			tmpProject.roleName = doc["project_role_name"].get_utf8().value.to_string();
			tmpProject.collectionName = doc["collection_name"].get_utf8().value.to_string();
			tmpProject.createdOn = doc["created_on"].get_date();

			auto creatingUserId = doc["created_by"].get_binary();
			tmpProject.creatingUser = MongoUserFunctions::getUserDataThroughId(creatingUserId, userClient);

			auto groupsArr = doc["groups"].get_array().value;

			for (auto groupId : groupsArr)
			{

				bsoncxx::oid id = groupId.get_oid().value;

				Group currentGroup = MongoGroupFunctions::getGroupData(id, userClient);

				tmpProject.groups.push_back(currentGroup);
			}

			projects.push_back(std::move(tmpProject));
		}

		return std::move(projects);
	}

	std::vector<Project> getAllProjects(User& loggedInUser, std::string filter, int limit, mongocxx::client& userClient)
	{
		std::string regexFilter = ".*" + filter + ".*";
		auto regex = bsoncxx::types::b_regex(regexFilter, "");

		value filterDoc = document{}
			<< "project_name" << regex
			<< finalize;

		mongocxx::database db = userClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		mongocxx::pipeline p{};
		p.match(filterDoc.view());
		if (limit > 0)
		{
			p.limit(limit);
		}


		mongocxx::cursor cursor = projectsCollection.aggregate(p);

		std::vector<Project> projects{};
		for (auto doc : cursor)
		{
			// Research std::move() 

			Project tmpProject{};
			tmpProject._id = doc["_id"].get_oid().value;
			tmpProject.name = doc["project_name"].get_utf8().value.to_string();
			tmpProject.roleName = doc["project_role_name"].get_utf8().value.to_string();
			tmpProject.collectionName = doc["collection_name"].get_utf8().value.to_string();
			tmpProject.createdOn = doc["created_on"].get_date();

			auto creatingUserId = doc["created_by"].get_binary();
			tmpProject.creatingUser = MongoUserFunctions::getUserDataThroughId(creatingUserId, userClient);

			auto groupsArr = doc["groups"].get_array().value;

			for (auto groupId : groupsArr)
			{

				bsoncxx::oid id = groupId.get_oid().value;

				Group currentGroup = MongoGroupFunctions::getGroupData(id, userClient);

				tmpProject.groups.push_back(currentGroup);
			}

			projects.push_back(std::move(tmpProject));
		}

		return std::move(projects);
	}

	size_t getAllProjectCount(User& loggedInUser, mongocxx::client& userClient)
	{
		mongocxx::database db = userClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		size_t count = projectsCollection.count_documents({});

		return count;
	}

	std::vector<Project> getAllGroupProjects(Group& group, mongocxx::client& userClient)
	{

		value filter = document{}
			<< "groups" << group._id
			<< finalize;


		mongocxx::database db = userClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		mongocxx::cursor cursor = projectsCollection.find(filter.view());

		std::vector<Project> projects{};
		for (auto doc : cursor)
		{

			Project tmpProject{};
			tmpProject._id = doc["_id"].get_oid().value;
			tmpProject.name = doc["project_name"].get_utf8().value.to_string();
			tmpProject.roleName = doc["project_role_name"].get_utf8().value.to_string();
			tmpProject.collectionName = doc["collection_name"].get_utf8().value.to_string();
			tmpProject.createdOn = doc["created_on"].get_date();

			bsoncxx::types::b_binary userId = doc["created_by"].get_binary();

			tmpProject.creatingUser = MongoUserFunctions::getUserDataThroughId(userId, userClient);

			auto groupsArr = doc["groups"].get_array().value;

			for (auto groupId : groupsArr)
			{
				bsoncxx::oid currId = groupId.get_oid().value;

				Group currentGroup = MongoGroupFunctions::getGroupData(currId, userClient);

				tmpProject.groups.push_back(currentGroup);
			}


			projects.push_back(std::move(tmpProject));
		}

		return std::move(projects);
	}

	Project changeProjectName(Project& project, std::string newName, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		value filter = document{}
			<< "_id" << project._id
			<< finalize;

		value update = document{}
			<< "$set"
			<< open_document
			<< "project_name" << newName
			<< close_document
			<< finalize;

		auto result = projectsCollection.update_one(filter.view(), update.view());
		int32_t matchedCount = result.get().matched_count();
		int32_t modifiedCount = result.get().modified_count();


		if (matchedCount == 1 && modifiedCount == 1)
		{
			project.name = newName;
			return project;
		}

		throw std::runtime_error("Couldn't change the project name!");
	}

	Project changeProjectOwner(Project& project, User& newOwner, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		value filter = document{}
			<< "_id" << project._id
			<< finalize;

		value update = document{}
			<< "$set"
			<< open_document
			<< "created_by" << newOwner.getUserId()
			<< close_document
			<< finalize;

		auto result = projectsCollection.update_one(filter.view(), update.view());
		int32_t matchedCount = result.get().matched_count();
		int32_t modifiedCount = result.get().modified_count();


		if (matchedCount == 1 && modifiedCount == 1)
		{
			MongoRoleFunctions::addRoleToUser(project.roleName, newOwner.username, adminClient);
			MongoRoleFunctions::removeRoleFromUser(project.roleName, project.creatingUser.username, adminClient);

			project.creatingUser = newOwner;
			return project;
		}

		throw std::runtime_error("Couldn't change the project owner!");
	}


	bool addGroupToProject(Group& group, Project& project, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		// checking whether it is already contained
		for (auto alreadyContainedGroup : project.groups)
		{
			if (alreadyContainedGroup._id == group._id)
			{
				return false;
			}
		}

		// Adding project role to this group role
		MongoRoleFunctions::addRoleToGroupRole(project.roleName, group, adminClient);


		// Adding this group to the group list of this project
		value filter = document{}
			<< "_id" << project._id
			<< finalize;

		value change = document{}
			<< "$push"
			<< open_document
			<< "groups" << group._id
			<< close_document
			<< finalize;

		auto result = projectCollection.update_one(filter.view(), change.view());

		int32_t matchedCount = result.get().matched_count();
		int32_t modifiedCount = result.get().modified_count();

		if (matchedCount == 1 && modifiedCount == 1)
		{
			return true;
		}

		return false;

	}


	bool removeGroupFromProject(Group& group, Project& project, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		bool isContained = false;
		// checking whether it is already contained
		for (auto alreadyContainedGroup : project.groups)
		{
			if (alreadyContainedGroup._id == group._id)
			{
				isContained = true;
				break;
			}
		}

		if (!isContained)
		{
			return false;
		}

		MongoRoleFunctions::removeRoleFromGroupRole(project.roleName, group, adminClient);


		// Adding this group to the group list of this project
		value filter = document{}
			<< "_id" << project._id
			<< finalize;

		value change = document{}
			<< "$pull"
			<< open_document
			<< "groups" << group._id
			<< close_document
			<< finalize;

		auto result = projectCollection.update_one(filter.view(), change.view());

		int32_t matchedCount = result.get().matched_count();
		int32_t modifiedCount = result.get().modified_count();

		if (matchedCount == 1 && modifiedCount == 1)
		{
			return true;
		}

		return false;
	}

	bool removeProject(Project& project, mongocxx::client& adminClient)
	{
		// First remove the project role
		// then remove project collection
		// then remove the project from the catalog

		MongoRoleFunctions::removeRole(project.roleName, adminClient);

		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectDataCollection = secondaryDb.collection(project.collectionName);
		mongocxx::collection projectFilesCollection = secondaryDb.collection(project.collectionName + ".files");
		mongocxx::collection projectChunksCollection = secondaryDb.collection(project.collectionName + ".chunks");
		mongocxx::collection projectResultsCollection = secondaryDb.collection(project.collectionName + ".results");

		projectDataCollection.drop();
		projectFilesCollection.drop();
		projectChunksCollection.drop();
		projectResultsCollection.drop();

		mongocxx::collection projectsCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		value filter = document{}
			<< "_id" << project._id
			<< finalize;

		auto result = projectsCollection.delete_one(filter.view());


		if (result.get().deleted_count() == 1)
		{
			return true;
		}

		return false;

	}


	bool changeProjectCreator(bsoncxx::oid& projectId, User& oldOwner, User& newOwner, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		Project project = getProject(projectId, adminClient);

		if (project.creatingUser.getUserId() != oldOwner.getUserId())
		{
			throw std::runtime_error("The logged in user is not the creator of this project! He cannot make the requested changes!");
		}

		value filter = document{}
			<< "_id" << projectId
			<< finalize;

		value update = document{}
			<< "$set"
			<< open_document
			<< "created_by" << newOwner.getUserId()
			<< close_document
			<< finalize;


		auto result = projectsCollection.update_one(filter.view(), update.view());
		int32_t matchedCount = result.get().matched_count();
		int32_t modifiedCount = result.get().modified_count();


		if (matchedCount == 1 && modifiedCount == 1)
		{
			MongoRoleFunctions::addRoleToUser(project.roleName, newOwner.username, adminClient);
			return true;
		}

		return false;
	}

	bool checkForCollectionExistence(const std::string& collectionName, mongocxx::client& userClient)
	{
		mongocxx::database secondaryDb = userClient.database(MongoConstants::PROJECTS_DB);
		return secondaryDb.has_collection(collectionName);
	}

	std::string projectToJson(Project& project)
	{
		OT_rJSON_createDOC(json);
		ot::rJSON::add(json, "id", project._id.to_string());
		ot::rJSON::add(json, "name", project.name);
		ot::rJSON::add(json, "collectionName", project.collectionName);
		ot::rJSON::add(json, "creatingUser", project.creatingUser.username);

		std::list<std::string> groups{};

		for (auto group : project.groups)
		{
			groups.push_back(group.name);
		}
		ot::rJSON::add(json, "groups", groups);

		return ot::rJSON::toJSON(json);
	}

	std::string projectsToJson(std::vector<Project>& projects)
	{
		std::list<std::string> jsonProjects{};

		for (auto project : projects)
		{
			jsonProjects.push_back(MongoProjectFunctions::projectToJson(project));
		}

		
		OT_rJSON_createDOC(json);
		ot::rJSON::add(json, "projects", jsonProjects);
		return ot::rJSON::toJSON(json);
	}
}