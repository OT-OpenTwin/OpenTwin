#pragma once

// Qt header
#include <QtCore/qstring.h>

// std header
#include <list>

class File;
class AppBase;

class Project {
public:
	Project();

	bool scan(const QString& _path, const QString& _projectName);

	bool scanFiles(const AppBase* _app);

	const QString& getPath(void) const { return m_path; };
	const QString& getName(void) const { return m_projectName; };
	const std::list<File*>& getHeaders(void) const { return m_headers; };
	const std::list<File*> getSources(void) const { return m_sources; };

private:
	QString m_path;
	QString m_projectName;
	std::list<File*> m_headers;
	std::list<File*> m_sources;

	std::map<QString, QString> m_fileHashData;
};