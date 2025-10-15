#pragma once

#include <iostream>
#include <cstdint>
#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "User.h"
#include "Project.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::document::value;
using bsoncxx::document::view;
using bsoncxx::document::element;


namespace MongoProjectFunctions
{

	Project createProject(std::string projectName, std::string projectType, User& creatingUser, mongocxx::client& adminClient);

	std::string generateProjectCollectionName(mongocxx::client& adminClient);

	Project getProject(bsoncxx::oid& projectId, mongocxx::client& userClient);

	Project getProject(std::string projectName, mongocxx::client& userClient);

	std::string getProjectsInfo(const std::list<std::string>& _projectNames, mongocxx::client& _adminClient);

	std::vector<Project> getAllGroupProjects(Group& group, mongocxx::client& userClient);

	std::vector<Project> getAllUserProjects(User& loggedInUser, std::string filter, int limit, mongocxx::client& userClient);

	std::vector<Project> getAllProjects(User& loggedInUser, std::string filter, int limit, mongocxx::client& userClient);
	size_t getAllProjectCount(User& loggedInUser, mongocxx::client& userClient);

	Project changeProjectName(Project& project, std::string newName, mongocxx::client& adminClient);

	Project changeProjectOwner(Project& project, User& newOwner, mongocxx::client& adminClient);

	bool addGroupToProject(Group& group, Project& project, mongocxx::client& adminClient);

	bool removeGroupFromProject(Group& group, Project& project, mongocxx::client& adminClient);

	bool removeProject(Project& project, mongocxx::client& adminClient);

	bool changeProjectCreator(bsoncxx::oid& projectId, User& oldOwner, User& newOwner, mongocxx::client& adminClient);

	bool checkForCollectionExistence(const std::string& collectionName, mongocxx::client& loggedInUser);

	std::string projectsToJson(std::vector<Project>& projects);

}