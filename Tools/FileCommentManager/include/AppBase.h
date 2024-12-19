#pragma once

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qobject.h>

// std header
#include <list>

class Project;

class AppBase : public QObject {
public:
	AppBase();
	virtual ~AppBase() {};

	const std::map<QString, Project*>& getProjects(void) const { return m_projects; };

private Q_SLOTS:
	void slotRun(void);
	void slotExit(int _exitCode = 0);

private:
	bool iniConfigEnv(void);
	void scanProjects(void);
	void scanProjectDir(const QString& _path);
	std::string intervalString(long long _ms);

	std::map<QString, Project*> m_projects;

};