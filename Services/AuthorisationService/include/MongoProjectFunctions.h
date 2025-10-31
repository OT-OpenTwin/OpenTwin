// @otlicense
// File: MongoProjectFunctions.h
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

#include "OTCore/ReturnMessage.h"
#include "OTCore/ProjectFilterData.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::document::value;
using bsoncxx::document::view;
using bsoncxx::document::element;


class MongoProjectFunctions {
	OT_DECL_STATICONLY(MongoProjectFunctions)
public:
	static ot::ProjectFilterData getProjectFilterOptions(const User& _loggedInUser, mongocxx::client& _adminClient);

	static Project createProject(std::string projectName, std::string projectType, User& creatingUser, mongocxx::client& adminClient);

	static void projectWasOpened(const std::string& projectName, mongocxx::client& adminClient);

	static ot::ReturnMessage updateAdditionalProjectInformation(const ot::ProjectInformation& _projectInfo, mongocxx::client& _adminClient);

	static std::string generateProjectCollectionName(mongocxx::client& adminClient);

	static Project getProject(bsoncxx::oid& projectId, mongocxx::client& userClient);

	static Project getProject(std::string projectName, mongocxx::client& userClient);

	static std::string getProjectsInfo(const std::list<std::string>& _projectNames, mongocxx::client& _adminClient);

	static std::vector<Project> getAllGroupProjects(Group& group, mongocxx::client& userClient);

	static std::vector<Project> getAllUserProjects(User& loggedInUser, std::string filter, int limit, mongocxx::client& userClient);
	static std::vector<Project> getAllUserProjects(User& loggedInUser, const ot::ProjectFilterData& filter, int limit, mongocxx::client& userClient);

	static std::vector<Project> getAllProjects(User& loggedInUser, std::string filter, int limit, mongocxx::client& userClient);
	static size_t getAllProjectCount(User& loggedInUser, mongocxx::client& userClient);

	static Project changeProjectName(Project& project, std::string newName, mongocxx::client& adminClient);

	static Project changeProjectOwner(Project& project, User& newOwner, mongocxx::client& adminClient);

	static bool addGroupToProject(Group& group, Project& project, mongocxx::client& adminClient);

	static bool removeGroupFromProject(Group& group, Project& project, mongocxx::client& adminClient);

	static bool removeProject(Project& project, mongocxx::client& adminClient);

	static bool changeProjectCreator(bsoncxx::oid& projectId, User& oldOwner, User& newOwner, mongocxx::client& adminClient);

	static bool checkForCollectionExistence(const std::string& collectionName, mongocxx::client& loggedInUser);

	static std::string projectsToJson(std::vector<Project>& projects);

private:
	static void appendFilter(std::list<bsoncxx::document::value>& _createdFilters, const std::string& _field, const std::list<std::string>& _values, bool _isArray);

	static void getDistinctStrings(mongocxx::collection& _collection, const bsoncxx::v_noabi::document::value& _filter, const std::string& _fieldName, std::list<std::string>& _result);
};