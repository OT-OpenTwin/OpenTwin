//! @file ProjectInformation.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include <OTCore/JSON.h>

// Qt header
#include <QtCore/qdatetime.h>

// std header
#include <list>
#include <string>

class ProjectInformation {
public:
	ProjectInformation();
	ProjectInformation(const ot::ConstJsonObject& _jsonObject);
	ProjectInformation(const ProjectInformation&) = default;
	ProjectInformation(ProjectInformation&&) = default;
	~ProjectInformation() {};
	
	ProjectInformation& operator = (const ProjectInformation&) = default;
	ProjectInformation& operator = (ProjectInformation&&) = default;

	void setProjectName(const std::string& _name) { m_name = _name; };
	const std::string& getProjectName(void) const { return m_name; };

	void setProjectType(const std::string& _type) { m_type = _type; };
	const std::string& getProjectType(void) const { return m_type; };

	void setUserName(const std::string& _name) { m_user = _name; };
	const std::string& getUserName(void) const { return m_user; };

	void setLastAccessTime(const QDateTime& _dateTime) { m_lastAccessTime = _dateTime; };
	const QDateTime& getLastAccessTime(void) const { return m_lastAccessTime; };

	void addGroup(const std::string& _group);
	void setGroups(const std::list<std::string>& _groups) { m_groups = _groups; };
	const std::list<std::string>& getGroups(void) const { return m_groups; };

private:
	std::string m_name;
	std::string m_type;
	std::string m_user;
	QDateTime m_lastAccessTime;
	std::list<std::string> m_groups;

};