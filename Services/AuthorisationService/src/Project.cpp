// @otlicense

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
		m_userGroups.push_back(currentGroup);
	}
}

void Project::addUserGroup(const Group& _group) {
	m_userGroups.push_back(_group);
}

ot::ProjectInformation Project::toProjectInformation() const {
	ot::ProjectInformation info;
	
	info.setVersion(m_version);
	info.setProjectName(m_name);
	info.setProjectType(m_type);
	info.setCollectionName(m_collectionName);
	info.setUserName(m_creatingUser.username);
	info.setCreationTime(m_createdOn);
	info.setLastAccessTime(m_lastAccessedOn);
	info.setTags(m_tags);
	info.setProjectGroup(m_projectGroup);

	for (const Group& group : m_userGroups) {
		info.addUserGroup(group.name);
	}

	return info;
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

	auto tagsIt = _view.find("tags");
	if (tagsIt != _view.end()) {
		if (tagsIt->type() == bsoncxx::v_noabi::type::k_array) {
			auto tagsArr = tagsIt->get_array().value;
			for (const auto& tag : tagsArr) {
				m_tags.push_back(std::string(tag.get_utf8().value.data()));
			}
		}
		else {
			OT_LOG_E("Invalid tags format");
		}
	}

	auto categoryIt = _view.find("project_group");
	if (categoryIt != _view.end()) {
		if (categoryIt->type() == bsoncxx::v_noabi::type::k_utf8) {
			m_projectGroup = std::string(categoryIt->get_utf8().value.data());
		}
		else {
			OT_LOG_E("Invalid category format");
		}
	}
}
