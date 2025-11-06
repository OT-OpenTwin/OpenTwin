// @otlicense
// File: MongoProjectFunctions.cpp
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

ot::ProjectFilterData MongoProjectFunctions::getProjectFilterOptions(const User& _loggedInUser, mongocxx::client& _adminClient) {
	ot::ProjectFilterData filters;

	mongocxx::database db = _adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);
	
	std::vector<Group> userGroups = MongoGroupFunctions::getAllUserGroups(_loggedInUser, _adminClient);

	// Build group access array
	auto accessBuilder = bsoncxx::builder::basic::array{};
	for (const auto& group : userGroups) {
		accessBuilder.append(group.id);
	}
	auto acessArr = document{} << "$in" << accessBuilder << finalize;

	// Build user access filter (access via ownership or group membership)
	auto userAccessDoc = document{}
		<< "$or"
		<< open_array
		<< open_document
		<< "created_by" << _loggedInUser.userId
		<< close_document
		<< open_document
		<< "groups" << acessArr
		<< close_document
		<< close_array
		<< finalize;

	std::list<std::string> result;
	getDistinctStrings(projectCollection, userAccessDoc, "project_name", result);
	filters.setProjectNames(std::move(result));
	result.clear();

	getDistinctStrings(projectCollection, userAccessDoc, "project_type", result);
	filters.setProjectTypes(std::move(result));
	result.clear();

	getDistinctStrings(projectCollection, userAccessDoc, "project_group", result);
	filters.setProjectGroups(std::move(result));
	result.clear();

	getDistinctStrings(projectCollection, userAccessDoc, "tags", result);
	filters.setTags(std::move(result));
	result.clear();

	getDistinctStrings(projectCollection, userAccessDoc, "groups", result);
	std::list<std::string> groupNames;
	for (const auto& groupId : result) {
		try {
			Group group = MongoGroupFunctions::getGroupDataById(groupId, _adminClient);
			groupNames.push_back(group.name);
		}
		catch (std::runtime_error err) {
			OT_LOG_E("Error retrieving group data for group ID \"" + groupId + "\". Error: " + err.what());
		}
	}
	filters.setUserGroups(std::move(groupNames));
	result.clear();

	getDistinctStrings(projectCollection, userAccessDoc, "created_by", result);
	std::list<std::string> ownerNames;
	for (const auto& ownerId : result) {
		try {
			User ownerUser = MongoUserFunctions::getUserDataThroughId(ownerId, _adminClient);
			ownerNames.push_back(ownerUser.username);
		}
		catch (std::runtime_error err) {
			OT_LOG_E("Error retrieving owner user data for user ID \"" + ownerId + "\". Error: " + err.what());
		}
	}
	filters.setOwners(std::move(ownerNames));

	return filters;
}

Project MongoProjectFunctions::createProject(std::string projectName, std::string projectType, User& creatingUser, mongocxx::client& adminClient) {
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
		<< "tags" << open_array << close_array
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

void MongoProjectFunctions::projectWasOpened(const std::string& projectName, mongocxx::client& adminClient) {
	mongocxx::database db = adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	// Filter
	auto filterQuery = bsoncxx::builder::stream::document{}
		<< "project_name" << projectName
		<< bsoncxx::builder::stream::finalize;

	// Modify
	auto modifyQuery = bsoncxx::builder::stream::document{}
		<< "$set" << bsoncxx::builder::stream::open_document
		<< "last_accessed_on" << bsoncxx::types::b_date(std::chrono::system_clock::now())
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	// Perform update
	projectCollection.update_one(filterQuery.view(), modifyQuery.view());
}

ot::ReturnMessage MongoProjectFunctions::updateAdditionalProjectInformation(const ot::ProjectInformation& _projectInfo, mongocxx::client& _adminClient) {
	mongocxx::database db = _adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	// Create tag array
	bsoncxx::builder::stream::array tagArray;
	for (const std::string& tag : _projectInfo.getTags()) {
		tagArray << tag;
	}

	// Filter
	auto filterQuery = bsoncxx::builder::stream::document{}
		<< "project_name" << _projectInfo.getProjectName()
		<< bsoncxx::builder::stream::finalize;

	// Modify
	auto modifyQuery = bsoncxx::builder::stream::document{}
		<< "$set" << bsoncxx::builder::stream::open_document
		<< "tags" << tagArray.view()
		<< "project_group" << _projectInfo.getProjectGroup()
		<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

	// Perform update
	projectCollection.update_one(filterQuery.view(), modifyQuery.view());

	return ot::ReturnMessage(ot::ReturnMessage::Ok);
}

std::string MongoProjectFunctions::generateProjectCollectionName(mongocxx::client& adminClient) {
	std::time_t currentTime = std::time(nullptr);
	struct tm currentDate;
	localtime_s(&currentDate, &currentTime);
	std::ostringstream collectionName;
	int random = rand() % 100;

	collectionName << "Data-" << 1900 + currentDate.tm_year << currentDate.tm_mon + 1 << currentDate.tm_mday << "-"
		<< currentDate.tm_hour << currentDate.tm_min << currentDate.tm_sec << "-" << random;

	bool done = false;

	while (!done) {
		try {
			getProject(collectionName.str(), adminClient);

			// This collection name is already in use
			random = rand() % 100;
			collectionName << random;

		}
		catch (std::runtime_error err) {
			done = true;
		}
	}

	return collectionName.str();
}

Project MongoProjectFunctions::getProject(bsoncxx::oid& projectId, mongocxx::client& userClient) {
	mongocxx::database secondaryDb = userClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectDataCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	value filter = document{}
		<< "_id" << projectId
		<< finalize;

	auto optional = projectDataCollection.find_one(filter.view());


	if (!optional.has_value()) {
		return Project();
	}

	value projectValue = optional.value();

	return Project(projectValue.view(), userClient);
}

Project MongoProjectFunctions::getProject(std::string projectName, mongocxx::client& userClient) {
	mongocxx::database secondaryDb = userClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectDataCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	value filter = document{}
		<< "project_name" << projectName
		<< finalize;

	auto optional = projectDataCollection.find_one(filter.view());

	if (!optional) {
		throw std::runtime_error("Project was not found");
	}

	value projectValue = optional.value();
	return Project(projectValue.view(), userClient);
}

std::string MongoProjectFunctions::getProjectsInfo(const std::list<std::string>& _projectNames, mongocxx::client& _adminClient) {
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

std::vector<Project> MongoProjectFunctions::getAllUserProjects(User& loggedInUser, std::string filter, int limit, mongocxx::client& _adminClient) {
	OT_LOG_D("Searching projects of user: " + loggedInUser.username);

	std::vector<Group> userGroups = MongoGroupFunctions::getAllUserGroups(loggedInUser, _adminClient);

	auto array_builder = bsoncxx::builder::basic::array{};

	for (const auto& group : userGroups) {
		array_builder.append(group.id);
	}

	auto inArr = document{} << "$in" << array_builder << finalize;


	std::string regexFilter = ".*" + filter + ".*";
	auto regex = bsoncxx::types::b_regex(regexFilter, "i");


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


	mongocxx::database db = _adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	mongocxx::pipeline p{};
	p.match(filterDoc.view());
	if (limit > 0) {
		p.limit(limit);
	}


	mongocxx::cursor cursor = projectsCollection.aggregate(p);

	std::vector<Project> projects{};
	for (auto doc : cursor) {
		// Research std::move() 

		Project tmpProject(doc);
		const std::string creatingUserId = std::string(doc["created_by"].get_utf8().value.data());
		tmpProject.setUser(MongoUserFunctions::getUserDataThroughId(creatingUserId, _adminClient));

		auto groupsArr = doc["groups"].get_array().value;

		for (const auto& groupId : groupsArr) {
			std::string id = std::string(groupId.get_utf8().value.data());

			Group currentGroup = MongoGroupFunctions::getGroupDataById(id, _adminClient);

			tmpProject.addUserGroup(currentGroup);
		}

		projects.push_back(tmpProject);
	}

	return projects;
}

std::vector<Project> MongoProjectFunctions::getAllUserProjects(User& loggedInUser, const ot::ProjectFilterData& _filter, const std::string& _textFilter, int limit, mongocxx::client& _adminClient) {
	OT_LOG_D("Searching projects of user: " + loggedInUser.username);

	std::vector<Group> userGroups = MongoGroupFunctions::getAllUserGroups(loggedInUser, _adminClient);

	std::list<bsoncxx::document::value> filters;

	// Directly accessible data
	appendFilter(filters, "project_group", _filter.getProjectGroups(), false);
	appendFilter(filters, "project_type", _filter.getProjectTypes(), false);
	appendFilter(filters, "project_name", _filter.getProjectNames(), false);
	appendFilter(filters, "tags", _filter.getTags(), true);

	// Owner filter, names provided, need to convert to IDs
	if (!_filter.getOwners().empty()) {
		std::list<std::string> ownerIds;
		bool hasEmpty = false;
		for (const std::string& ownerName : _filter.getOwners()) {
			if (ownerName.empty()) {
				hasEmpty = true;
			}
			else {
				try {
					User ownerUser = MongoUserFunctions::getUserDataThroughUsername(ownerName, _adminClient);
					ownerIds.push_back(ownerUser.userId);
				}
				catch (std::runtime_error err) {
					OT_LOG_E("Owner user " + ownerName + " specified in the project filter was not found.");
				}
			}
		}
		if (hasEmpty) {
			ownerIds.push_back("");
		}
		appendFilter(filters, "created_by", ownerIds, false);
	}

	// Group access filter, names provided, need to convert to IDs
	if (!_filter.getUserGroups().empty()) {
		std::list<std::string> groupIds;
		bool hasEmpty = false;
		for (const auto& groupName : _filter.getUserGroups()) {
			if (groupName.empty()) {
				hasEmpty = true;
			}
			else {
				bool found = false;
				for (const Group& group : userGroups) {
					if (group.name == groupName) {
						groupIds.push_back(group.id);
						found = true;
						break;
					}
				}
				if (!found) {
					OT_LOG_E("User " + loggedInUser.username + " is not member of group " + groupName + " specified in the project filter.");
				}
			}
		}
		if (hasEmpty) {
			groupIds.push_back("");
		}
		appendFilter(filters, "groups", groupIds, true);
	}

	// Build group access array
	auto accessBuilder = bsoncxx::builder::basic::array{};
	for (const auto& group : userGroups) {
		accessBuilder.append(group.id);
	}
	auto acessArr = document{} << "$in" << accessBuilder << finalize;

	// Build user access filter (access via ownership or group membership)
	auto userAccessDoc = document{}
		<< "$or"
		<< open_array
		<< open_document
		<< "created_by" << loggedInUser.userId
		<< close_document
		<< open_document
		<< "groups" << acessArr
		<< close_document
		<< close_array
		<< finalize;

	// Combine all filters
	auto filterBuilder = bsoncxx::builder::basic::array{};
	for (auto& f : filters) {
		filterBuilder.append(std::move(f));
	}
	filterBuilder.append(std::move(userAccessDoc));

	// Add text filter if specified
	if (!_textFilter.empty()) {
		auto projectNameFilter = bsoncxx::types::b_regex(".*" + _textFilter + ".*", "i");
		filterBuilder.append(document{} << "project_name" << projectNameFilter << finalize);
	}

	value filterDoc = document{} << "$and" << filterBuilder << finalize;

	//OT_LOG_T(bsoncxx::to_json(filterDoc.view()));

	mongocxx::database db = _adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	mongocxx::pipeline p{};
	p.match(filterDoc.view());
	if (limit > 0) {
		p.limit(limit);
	}

	mongocxx::cursor cursor = projectsCollection.aggregate(p);

	std::vector<Project> projects{};
	for (auto doc : cursor) {
		// Research std::move() 

		Project tmpProject(doc);
		const std::string creatingUserId = std::string(doc["created_by"].get_utf8().value.data());
		tmpProject.setUser(MongoUserFunctions::getUserDataThroughId(creatingUserId, _adminClient));

		auto groupsArr = doc["groups"].get_array().value;

		for (const auto& groupId : groupsArr) {
			std::string id = std::string(groupId.get_utf8().value.data());

			Group currentGroup = MongoGroupFunctions::getGroupDataById(id, _adminClient);

			tmpProject.addUserGroup(currentGroup);
		}

		projects.push_back(tmpProject);
	}

	return projects;
}

std::vector<Project> MongoProjectFunctions::getAllProjects(User& loggedInUser, std::string filter, int limit, mongocxx::client& userClient) {
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

size_t MongoProjectFunctions::getAllProjectCount(User& loggedInUser, mongocxx::client& userClient) {
	mongocxx::database db = userClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	size_t count = projectsCollection.count_documents({});

	return count;
}

std::vector<Project> MongoProjectFunctions::getAllGroupProjects(Group& group, mongocxx::client& userClient) {
	value filter = document{}
		<< "groups" << group.id
		<< finalize;

	mongocxx::database db = userClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectsCollection = db.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	mongocxx::cursor cursor = projectsCollection.find(filter.view());

	std::vector<Project> projects{};
	for (auto doc : cursor) {
		Project newProject(doc, userClient);
		projects.push_back(std::move(newProject));
	}

	return projects;
}

Project MongoProjectFunctions::changeProjectName(Project& project, std::string newName, mongocxx::client& adminClient) {
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

	if (!result.has_value()) {
		throw std::exception("Change project name failed.");
	}
	int32_t matchedCount = result.value().matched_count();
	int32_t modifiedCount = result.value().modified_count();

	if (matchedCount == 1 && modifiedCount == 1) {
		project.setName(newName);
		return project;
	}

	throw std::runtime_error("Couldn't change the project name!");
}

Project MongoProjectFunctions::changeProjectOwner(Project& project, User& newOwner, mongocxx::client& adminClient) {
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
	if (!result.has_value()) {
		throw std::exception("Change project owner failed.");
	}

	int32_t matchedCount = result.value().matched_count();
	int32_t modifiedCount = result.value().modified_count();

	if (matchedCount == 1 && modifiedCount == 1) {
		MongoRoleFunctions::addRoleToUser(project.getRoleName(), newOwner.username, adminClient);
		MongoRoleFunctions::removeRoleFromUser(project.getRoleName(), project.getUser().username, adminClient);

		project.setUser(newOwner);
		return project;
	}

	throw std::runtime_error("Couldn't change the project owner!");
}

bool MongoProjectFunctions::addGroupToProject(Group& group, Project& project, mongocxx::client& adminClient) {
	mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	// checking whether it is already contained
	for (const auto& alreadyContainedGroup : project.getUserGroups()) {
		if (alreadyContainedGroup.id == group.id) {
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

	if (!result.has_value()) {
		throw std::exception("Add group to project failed.");
	}

	int32_t matchedCount = result.value().matched_count();
	int32_t modifiedCount = result.value().modified_count();

	if (matchedCount == 1 && modifiedCount == 1) {
		return true;
	}

	return false;
}

bool MongoProjectFunctions::removeGroupFromProject(Group& group, Project& project, mongocxx::client& adminClient) {
	mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	bool isContained = false;
	// checking whether it is already contained
	for (const auto& alreadyContainedGroup : project.getUserGroups()) {
		if (alreadyContainedGroup.id == group.id) {
			isContained = true;
			break;
		}
	}

	if (!isContained) {
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

	if (!result.has_value()) {
		throw std::exception("Remove role from group role failed.");
	}

	int32_t matchedCount = result.value().matched_count();
	int32_t modifiedCount = result.value().modified_count();

	if (matchedCount == 1 && modifiedCount == 1) {
		return true;
	}

	return false;
}

bool MongoProjectFunctions::removeProject(Project& project, mongocxx::client& adminClient) {
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

	if (!result.has_value()) {
		throw std::exception("Remove project failed.");
	}
	if (result.value().deleted_count() == 1) {
		return true;
	}

	return false;
}

bool MongoProjectFunctions::changeProjectCreator(bsoncxx::oid& projectId, User& oldOwner, User& newOwner, mongocxx::client& adminClient) {
	mongocxx::database secondaryDb = adminClient.database(MongoConstants::PROJECTS_DB);
	mongocxx::collection projectsCollection = secondaryDb.collection(MongoConstants::PROJECT_CATALOG_COLLECTION);

	Project project = getProject(projectId, adminClient);

	if (project.getUser().userId != oldOwner.userId) {
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

	if (!result.has_value()) {
		throw std::exception("Change project creator failed.");
	}

	int32_t matchedCount = result.value().matched_count();
	int32_t modifiedCount = result.value().modified_count();


	if (matchedCount == 1 && modifiedCount == 1) {
		MongoRoleFunctions::addRoleToUser(project.getRoleName(), newOwner.username, adminClient);
		return true;
	}

	return false;
}

bool MongoProjectFunctions::checkForCollectionExistence(const std::string& collectionName, mongocxx::client& userClient) {
	mongocxx::database secondaryDb = userClient.database(MongoConstants::PROJECTS_DB);
	return secondaryDb.has_collection(collectionName);
}

std::string MongoProjectFunctions::projectsToJson(std::vector<Project>& projects) {
	ot::JsonDocument doc;
	ot::JsonArray projectsArr;
	for (const Project& project : projects) {
		projectsArr.PushBack(ot::JsonObject(project.toProjectInformation(), doc.GetAllocator()), doc.GetAllocator());
	}
	doc.AddMember(OT_ACTION_PARAM_List, projectsArr, doc.GetAllocator());
	return doc.toJson();

	/*
	std::list<std::string> jsonProjects{};

	for (const Project& project : projects)
	{
		jsonProjects.push_back(MongoProjectFunctions::projectToJson(project));
	}


	ot::JsonDocument json;
	json.AddMember("projects", ot::JsonArray(jsonProjects, json.GetAllocator()), json.GetAllocator());
	return json.toJson();

	*/
}

void MongoProjectFunctions::appendFilter(std::list<bsoncxx::document::value>& _createdFilters, const std::string& _field, const std::list<std::string>& _values, bool _isArray) {
	if (_values.empty()) {
		return;
	}

	// First separate empty and non-empty values
	std::vector<std::string> nonEmpty;
	bool hasEmpty = false;

	for (const std::string& val : _values) {
		if (val.empty()) {
			hasEmpty = true;
		}
		else {
			nonEmpty.push_back(val);
		}
	}

	bsoncxx::builder::basic::array orArray;

	// Match non-empty entries
	if (!nonEmpty.empty()) {
		auto optArray = bsoncxx::builder::basic::array{};
		for (const std::string& v : nonEmpty) {
			optArray.append(v);
		}
		orArray.append(document{} << _field << open_document << "$in" << optArray << close_document << finalize);
	}

	// Match documents where field is missing or empty
	if (hasEmpty) {
		if (_isArray) {
			// field == [] (empty array)
			orArray.append(document{} << _field << open_document << "$eq" << open_array << close_array << close_document << finalize);
		}
		else {
			// field == "" (empty string)
			orArray.append(document{} << _field << "" << finalize);
		}
		
		// field does not exist (legacy support)
		orArray.append(document{} << _field << open_document << "$exists" << false << close_document << finalize);
	}
	_createdFilters.push_back(document{} << "$or" << orArray << finalize);

	//OT_LOG_W(bsoncxx::to_json(_createdFilters.back().view()));
}

void MongoProjectFunctions::getDistinctStrings(mongocxx::collection& _collection, const bsoncxx::v_noabi::document::value& _filter, const std::string& _fieldName, std::list<std::string>& _result) {
	try {
		auto distinctCursor = _collection.distinct(_fieldName, bsoncxx::v_noabi::document::value(_filter));
		for (auto&& it : distinctCursor) {
			if (it["values"] && it["values"].type() == bsoncxx::type::k_array) {
				auto arrayView = it["values"].get_array().value;
				for (auto& val : arrayView) {
					if (val.type() == bsoncxx::type::k_utf8) {
						std::string value = val.get_utf8().value.data();
						if (!value.empty()) {
							_result.push_back(std::move(value));
						}
					}
				}
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_E("Error getting distinct strings for field \"" + _fieldName + "\": " + _e.what());
	}
}
