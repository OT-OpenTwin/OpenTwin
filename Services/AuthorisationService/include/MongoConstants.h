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