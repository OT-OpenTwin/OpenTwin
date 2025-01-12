//! @file ProjectInformation.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtCore/qdatetime.h>

// std header
#include <string>

class ProjectInformation {
public:
	ProjectInformation();
	ProjectInformation(const ProjectInformation&) = default;
	ProjectInformation(ProjectInformation&&) = default;
	~ProjectInformation() {};

	ProjectInformation& operator = (const ProjectInformation&) = default;
	ProjectInformation& operator = (ProjectInformation&&) = default;

	void setProjectName(const std::string& _name) { m_name = _name; };
	const std::string& getProjectName(void) const { return m_name; };

	void setUserName(const std::string& _name) { m_user = _name; };
	const std::string& getUserName(void) const { return m_user; };

	void setLastAccessTime(const QDateTime& _dateTime) { m_lastAccessTime = _dateTime; };
	const QDateTime& getLastAccessTime(void) const { return m_lastAccessTime; };

private:
	std::string m_name;
	std::string m_user;
	QDateTime m_lastAccessTime;

};