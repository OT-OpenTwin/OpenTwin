#include "MongoProjectFunctions.h"
#include "MongoGroupFunctions.h"
#include "MongoRoleFunctions.h"
#include "MongoConstants.h"
#include "MongoUserFunctions.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"

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

	Project createProject(std::string projectName, std::string projectType, User& creatingUser, mongocxx::client& adminClient)
	{
		mongocxx::database db = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		// Create the Collection for the project data
		std::string collectionName = generateProjectCollectionName(adminClient);
		std::string roleName = collectionName + "_role";

		// Step one creating the Project Document and inserting it in the Projects collection
		value new_project_document = document{}
			<< "project_name" << projectName
			<< "project_type" << projectType
			<< "created_on" << bsoncxx::types::b_date(std::chrono::system_clock::now())
			<< "created_by" << creatingUser.userId
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

		
		if (!optional.has_value())
		{
			return Project();
		}

		value projectValue = optional.value();
		
		return Project(projectValue.view(), userClient);
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

		value projectValue = optional.value();
		return Project(projectValue.view(), userClient);
	}

	std::string getProjectsInfo(const std::list<std::string>& _projectNames, mongocxx::client& _adminClient)
	{
		auto array_builder = bsoncxx::builder::basic::array{};

		for (const auto& projectName : _projectNames) {
			array_builder.append(projectName);
		}

		auto inArr = document{} << "$in" << array_builder << finalize;

		auto filter = document{} <<
			"project_name" << inArr
			<< finalize;

		mongocxx::database db = _adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		mongocxx::cursor cursor = projectsCollection.find(filter.view());

		std::vector<Project> projects{};
		for (auto doc : cursor) {
			projects.push_back(Project(doc, _adminClient));
		}

		return projectsToJson(projects);
	}

	std::vector<Project> getAllUserProjects(User& loggedInUser, std::string filter, int limit, mongocxx::client& userClient)
	{
		OT_LOG_D("Searching projects of user: " + loggedInUser.username);

		std::vector<Group> userGroups = MongoGroupFunctions::getAllUserGroups(loggedInUser, userClient);

		auto array_builder = bsoncxx::builder::basic::array{};

		for (const auto& group : userGroups) {
			array_builder.append(group.id);
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
			<< "created_by" << loggedInUser.userId
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

			Project tmpProject(doc);
			const std::string creatingUserId = std::string(doc["created_by"].get_utf8().value.data());
			tmpProject.setUser(MongoUserFunctions::getUserDataThroughId(creatingUserId, userClient));

			auto groupsArr = doc["groups"].get_array().value;

			for (const auto& groupId : groupsArr)
			{
				std::string id = std::string(groupId.get_utf8().value.data());

				Group currentGroup = MongoGroupFunctions::getGroupDataById(id, userClient);

				tmpProject.addGroup(currentGroup);
			}

			projects.push_back(tmpProject);
		}

		return projects;
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
		if (limit > 0) {
			p.limit(limit);
		}

		mongocxx::cursor cursor = projectsCollection.aggregate(p);

		std::vector<Project> projects{};
		for (auto doc : cursor) {
			projects.push_back(Project(doc, userClient));
		}

		return projects;
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
			<< "groups" << group.id
			<< finalize;

		mongocxx::database db = userClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		mongocxx::cursor cursor = projectsCollection.find(filter.view());

		std::vector<Project> projects{};
		for (auto doc : cursor)
		{
			Project newProject(doc, userClient);
			projects.push_back(std::move(newProject));
		}

		return projects;
	}

	Project changeProjectName(Project& project, std::string newName, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		value filter = document{}
			<< "_id" << project.getId()
			<< finalize;

		value update = document{}
			<< "$set"
			<< open_document
			<< "project_name" << newName
			<< close_document
			<< finalize;

		auto result = projectsCollection.update_one(filter.view(), update.view());

		if (!result.has_value())
		{
			throw std::exception("Change project name failed.");
		}
		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();

		if (matchedCount == 1 && modifiedCount == 1)
		{
			project.setName(newName);
			return project;
		}

		throw std::runtime_error("Couldn't change the project name!");
	}

	Project changeProjectOwner(Project& project, User& newOwner, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectsCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		value filter = document{}
			<< "_id" << project.getId()
			<< finalize;

		value update = document{}
			<< "$set"
			<< open_document
			<< "created_by" << newOwner.userId
			<< close_document
			<< finalize;

		auto result = projectsCollection.update_one(filter.view(), update.view());
		if (!result.has_value())
		{
			throw std::exception("Change project owner failed.");
		}

		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();

		if (matchedCount == 1 && modifiedCount == 1)
		{
			MongoRoleFunctions::addRoleToUser(project.getRoleName(), newOwner.username, adminClient);
			MongoRoleFunctions::removeRoleFromUser(project.getRoleName(), project.getUser().username, adminClient);

			project.setUser(newOwner);
			return project;
		}

		throw std::runtime_error("Couldn't change the project owner!");
	}


	bool addGroupToProject(Group& group, Project& project, mongocxx::client& adminClient)
	{
		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		// checking whether it is already contained
		for (auto alreadyContainedGroup : project.getGroups())
		{
			if (alreadyContainedGroup.id == group.id)
			{
				return false;
			}
		}

		// Adding project role to this group role
		MongoRoleFunctions::addRoleToGroupRole(project.getRoleName(), group, adminClient);


		// Adding this group to the group list of this project
		value filter = document{}
			<< "_id" << project.getId()
			<< finalize;

		value change = document{}
			<< "$push"
			<< open_document
			<< "groups" << group.id
			<< close_document
			<< finalize;

		auto result = projectCollection.update_one(filter.view(), change.view());

		if (!result.has_value())
		{
			throw std::exception("Add group to project failed.");
		}

		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();

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
		for (auto alreadyContainedGroup : project.getGroups())
		{
			if (alreadyContainedGroup.id == group.id)
			{
				isContained = true;
				break;
			}
		}

		if (!isContained)
		{
			return false;
		}

		MongoRoleFunctions::removeRoleFromGroupRole(project.getRoleName(), group, adminClient);


		// Adding this group to the group list of this project
		value filter = document{}
			<< "_id" << project.getId()
			<< finalize;

		value change = document{}
			<< "$pull"
			<< open_document
			<< "groups" << group.id
			<< close_document
			<< finalize;

		auto result = projectCollection.update_one(filter.view(), change.view());

		if (!result.has_value())
		{
			throw std::exception("Remove role from group role failed.");
		}

		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();

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

		MongoRoleFunctions::removeRole(project.getRoleName(), adminClient);

		mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
		mongocxx::collection projectDataCollection = secondaryDb.collection(project.getCollectionName());
		mongocxx::collection projectFilesCollection = secondaryDb.collection(project.getCollectionName() + ".files");
		mongocxx::collection projectChunksCollection = secondaryDb.collection(project.getCollectionName() + ".chunks");
		mongocxx::collection projectResultsCollection = secondaryDb.collection(project.getCollectionName() + ".results");

		projectDataCollection.drop();
		projectFilesCollection.drop();
		projectChunksCollection.drop();
		projectResultsCollection.drop();

		mongocxx::collection projectsCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

		value filter = document{}
			<< "_id" << project.getId()
			<< finalize;

		auto result = projectsCollection.delete_one(filter.view());

		if (!result.has_value())
		{
			throw std::exception("Remove project failed.");
		}
		if (result.value().deleted_count() == 1)
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

		if (project.getUser().userId != oldOwner.userId)
		{
			throw std::runtime_error("The logged in user is not the creator of this project! He cannot make the requested changes!");
		}

		value filter = document{}
			<< "_id" << projectId
			<< finalize;

		value update = document{}
			<< "$set"
			<< open_document
			<< "created_by" << newOwner.userId
			<< close_document
			<< finalize;


		auto result = projectsCollection.update_one(filter.view(), update.view());

		if (!result.has_value())
		{
			throw std::exception("Change project creator failed.");
		}

		int32_t matchedCount = result.value().matched_count();
		int32_t modifiedCount = result.value().modified_count();


		if (matchedCount == 1 && modifiedCount == 1)
		{
			MongoRoleFunctions::addRoleToUser(project.getRoleName(), newOwner.username, adminClient);
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
		ot::JsonDocument json;
		json.AddMember("id", ot::JsonString(project.getId().to_string(), json.GetAllocator()), json.GetAllocator());
		json.AddMember(OT_PARAM_AUTH_NAME, ot::JsonString(project.getName(), json.GetAllocator()), json.GetAllocator());
		json.AddMember(OT_PARAM_AUTH_PROJECT_TYPE, ot::JsonString(project.getType(), json.GetAllocator()), json.GetAllocator());
		json.AddMember(OT_PARAM_AUTH_PROJECT_COLLECTION, ot::JsonString(project.getCollectionName(), json.GetAllocator()), json.GetAllocator());
		json.AddMember(OT_PARAM_AUTH_OWNER, ot::JsonString(project.getUser().username, json.GetAllocator()), json.GetAllocator());
		json.AddMember(OT_PARAM_AUTH_PROJECT_LASTACCESS, project.getLastAccessedOn().value.count(), json.GetAllocator());
		
		std::list<std::string> groups;

		for (auto group : project.getGroups()) {
			groups.push_back(group.name);
		}
		json.AddMember("groups", ot::JsonArray(groups, json.GetAllocator()), json.GetAllocator());

		return json.toJson();
	}

	std::string projectsToJson(std::vector<Project>& projects)
	{
		std::list<std::string> jsonProjects{};

		for (auto project : projects)
		{
			jsonProjects.push_back(MongoProjectFunctions::projectToJson(project));
		}

		
		ot::JsonDocument json;
		json.AddMember("projects", ot::JsonArray(jsonProjects, json.GetAllocator()), json.GetAllocator());
		return json.toJson();
	}
}