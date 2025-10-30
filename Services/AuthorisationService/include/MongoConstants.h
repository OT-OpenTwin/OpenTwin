// @otlicense
// File: MongoConstants.h
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

#include <cstdint>
#include <iostream>

namespace MongoConstants
{
	std::string const ADMIN_DB = "admin";
	std::string const SYSTEM_USERS_COLLECTION = "system.users";

	std::string const PROJECT_TEMPLATES_DB = "ProjectTemplates"; // Accessible by everyone
	std::string const PROJECT_TEMPLATES_ROLE = "ProjectTemplates_role";

	std::string const PROJECTS_LARGE_DATA_DB = "ProjectsLargeData"; // Accessible by everyone
	std::string const PROJECTS_LARGE_DATA_ROLE = "ProjectsLargeData_role";

	std::string const SYSTEM_DB = "System"; // Accessible by everyone
	std::string const SYSTEM_DB_ROLE = "System_db_role"; 

	std::string const SETTINGS_DB = "UserSettings"; // Accessible by everyone, keeps data one collection per user
	std::string const SETTINGS_DB_ROLE = "UserSettings_db_role";

	std::string const USER_DB = "User"; // Accessible by admin only
	std::string const USER_CATALOG_COLLECTION = "Catalog";
	std::string const USER_CATALOG_ROLE = "user_catalog_collection_role";
	std::string const USER_DB_LIST_CATALOG_ROLE = "user_db_list_collections_role";

	std::string const PROJECTS_DB = "Projects";
	std::string const PROJECT_CATALOG_COLLECTION = "Catalog";
	std::string const PROJECT_CATALOG_ROLE = "project_catalog_collection_role";
	std::string const PROJECT_DB_LIST_COLLECTIONS_ROLE = "project_db_list_collections_role";

	std::string const LIBRARIES_DB = "Libraries";
	std::string const LIBRARIES_DB_ROLE = "Libraries_db_role";

	std::string const GROUPS_COLLECTION = "Groups";
	std::string const GROUP_ROLE = "groups_collection_role";

	std::string const SESSIONS_COLLECTION = "Sessions";
	std::string const SESSIONS_ROLE = "sessions_collection_role";

}