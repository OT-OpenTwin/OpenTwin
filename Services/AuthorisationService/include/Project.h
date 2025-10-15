#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include "User.h"
#include "Group.h"
#include "MongoUserFunctions.h"
#include "MongoGroupFunctions.h"

#include "OTCore/ProjectInformation.h"

class Project
{

public:
	Project();
	Project(const bsoncxx::v_noabi::document::view& _view);
	Project(const bsoncxx::v_noabi::document::view& _view, mongocxx::client& _userClient);
	Project(const Project&) = default;
	Project(Project&&) = default;
	~Project() {};

	Project& operator = (const Project&) = default;
	Project& operator = (Project&&) = default;

	void setId(const bsoncxx::oid& _id) { m_id = _id; };
	const bsoncxx::oid& getId(void) const { return m_id; };

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName(void) const { return m_name; };

	void setType(const std::string& _type) { m_type = _type; };
	const std::string& getType(void) const { return m_type; };

	void setRoleName(const std::string& _roleName) { m_roleName = _roleName; };
	const std::string& getRoleName(void) const { return m_roleName; };

	void setCollectionName(const std::string& _collectionName) { m_collectionName = _collectionName; };
	const std::string& getCollectionName(void) const { return m_collectionName; };

	void setCreatedOn(bsoncxx::types::b_date _msSinceEpoch) { m_createdOn = _msSinceEpoch; };
	bsoncxx::types::b_date getCreatedOn(void) const { return m_createdOn; };

	void setUser(const User& _user) { m_creatingUser = _user; };
	const User& getUser(void) const { return m_creatingUser; };

	void addGroup(const Group& _group);
	void setGroups(const std::vector<Group>& _groups) { m_groups = _groups; };
	const std::vector<Group>& getGroups(void) const { return m_groups; };

	void setVersion(int _version) { m_version = _version; };
	int getVersion(void) const { return m_version; };

	void setLastAccessedOn(bsoncxx::types::b_date _msSinceEpoch) { m_lastAccessedOn = _msSinceEpoch; };
	bsoncxx::types::b_date getLastAccessedOn(void) const { return m_lastAccessedOn; };

	ot::ProjectInformation toProjectInformation() const;

private:
	void importData(const bsoncxx::v_noabi::document::view& _view);

	bsoncxx::oid m_id;
	std::string m_name;
	std::string m_type;
	std::string m_roleName;
	std::string m_collectionName;
	bsoncxx::types::b_date m_createdOn;
	User m_creatingUser;
	std::vector<Group> m_groups;
	int m_version;
	bsoncxx::types::b_date m_lastAccessedOn;
};

