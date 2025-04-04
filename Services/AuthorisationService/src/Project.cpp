#include "Project.h"

Project::Project() : m_createdOn(std::chrono::system_clock::now()), m_lastAccessedOn(std::chrono::system_clock::now()),
	m_version(-1)
{

}

Project::Project(const bsoncxx::v_noabi::document::view& _view) : m_createdOn(std::chrono::system_clock::now()), m_lastAccessedOn(std::chrono::system_clock::now()) {
	this->importData(_view);
}

Project::Project(const bsoncxx::v_noabi::document::view& _view, mongocxx::client& _userClient) : m_createdOn(std::chrono::system_clock::now()), m_lastAccessedOn(std::chrono::system_clock::now()) {
	this->importData(_view);

	std::string userId = std::string(_view["created_by"].get_utf8().value.data());
	m_creatingUser = MongoUserFunctions::getUserDataThroughId(userId, _userClient);

	auto groupsArr = _view["groups"].get_array().value;
	for (const auto& groupId : groupsArr) {
		std::string currId = std::string(groupId.get_utf8().value.data());

		Group currentGroup = MongoGroupFunctions::getGroupDataById(currId, _userClient);
		m_groups.push_back(currentGroup);
	}
}

void Project::addGroup(const Group& _group) {
	m_groups.push_back(_group);
}

void Project::importData(const bsoncxx::v_noabi::document::view& _view) {
	m_id = _view["_id"].get_oid().value;
	m_name = std::string(_view["project_name"].get_utf8().value.data());

	try {
		m_type = std::string(_view["project_type"].get_utf8().value.data());
	}
	catch (...) {
		m_type = "Development";
	}
	
	m_roleName = std::string(_view["project_role_name"].get_utf8().value.data());
	m_collectionName = std::string(_view["collection_name"].get_utf8().value.data());
	m_createdOn = _view["created_on"].get_date();
	m_lastAccessedOn = _view["last_accessed_on"].get_date();

	try {
		m_version = _view["version"].get_int32().value;
	}
	catch (...) {
		m_version = -1;
	}
}
