#pragma once

// Qt header
#include <QtCore/qstring.h>

// std header
#include <map>

class Project;

class ScanData {
public:
	ScanData(const std::map<QString, Project*>& _projects, const std::map<QString, QString>& _hashData);

	const std::map<QString, Project*>& getProjects(void) const { return m_projects; };
	const std::map<QString, QString>& getFileHashData(void) const { return m_fileHashData; };

private:
	const std::map<QString, Project*>& m_projects;
	const std::map<QString, QString>& m_fileHashData;

	ScanData() = delete;
	ScanData(const ScanData&) = delete;
	ScanData(ScanData&&) = delete;
	ScanData& operator = (const ScanData&) = delete;
	ScanData& operator = (ScanData&&) = delete;
};