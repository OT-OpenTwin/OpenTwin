// Project header
#include "File.h"
#include "Config.h"
#include "Logger.h"
#include "AppBase.h"
#include "Project.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtCore/qcoreapplication.h>

// std header
#include <chrono>
#include <sstream>

#define LOG_I(___message) LOG_INFO("Core", ___message)
#define LOG_W(___message) LOG_WARNING("Core", ___message)
#define LOG_E(___message) LOG_ERROR("Core", ___message)

AppBase::AppBase() {
	QMetaObject::invokeMethod(this, &AppBase::slotRun, Qt::QueuedConnection);
}

void AppBase::slotRun(void) {
	// Save start time
	std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

	// Initialize environment and config
	if (!this->iniConfigEnv()) {
		return;
	}

	// Detect projects
	this->scanProjects();

	// Process data


	// Log duration
	std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	LOG_I("Finished. Took " + QString::fromStdString(this->intervalString(static_cast<long long>(duration.count()))));

	// Done
	this->slotExit(0);
}

void AppBase::slotExit(int _exitCode) {
	QCoreApplication::exit(_exitCode);
}

bool AppBase::iniConfigEnv(void) {
	// Check User
	QString user = QString::fromStdString(qgetenv("OPENTWIN_DEV_USER").toStdString());
	if (user.isEmpty()) {
		LOG_E("No user specified in environment \"OPENTWIN_DEV_USER\". Skipping processing..");
		this->slotExit(0);
		return false;
	}

	// Check Development Environment
	QByteArray otdevenv = qgetenv("OPENTWIN_DEV_ROOT");
	if (otdevenv.isEmpty()) {
		LOG_E("OpenTwin development environment is not set: \"OPENTWIN_DEV_ROOT\".");
		this->slotExit(1);
		return false;
	}
	QString otdevpath = QString::fromStdString(otdevenv.toStdString());
	QDir otdevdir(otdevpath);
	if (!otdevdir.exists()) {
		LOG_E("Specified OpenTwin development environment is not set: \"" + otdevpath + "\".");
		this->slotExit(2);
		return false;
	}

	// Read config
	if (!Config::instance().readFromFile(otdevpath + "/Tools/FileCommentManager/FileCommentManager.config.json", otdevpath)) {
		this->slotExit(3);
		return false;
	}

	return true;
}

void AppBase::scanProjects(void) {
	for (const QString& path : Config::instance().getProjectSearchPaths()) {
		this->scanProjectDir(path);

		QDir dir(path);
		QFileInfoList childDirs = dir.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		for (const QFileInfo& info : childDirs) {
			this->scanProjectDir(info.canonicalPath() + "/" + info.baseName());
		}
	}
}

void AppBase::scanProjectDir(const QString& _path) {
	QDir dir(_path);

	QFileInfoList projectFiles = dir.entryInfoList({ "*.vcxproj" }, QDir::Files);
	for (const QFileInfo& fileInfo : projectFiles) {
		Project* newProject = new Project;
		try {
			if (!newProject->scan(fileInfo.canonicalPath(), fileInfo.baseName())) {
				delete newProject;
				newProject = nullptr;
			}
		}
		catch (const std::exception& _e) {
			LOG_E(QString(_e.what()));
			if (newProject) {
				delete newProject;
				newProject = nullptr;
			}
		}
		catch (...) {
			LOG_E("Unknown Error!");
			delete newProject;
			newProject = nullptr;
		}

		if (newProject) {
			if (m_projects.find(newProject->getName())!= m_projects.end()) {
				LOG_E("Duplicate project: " + newProject->getName());
				delete newProject;
				newProject = nullptr;
			}
			else {
				LOG_I("Adding project { \"Project\": \"" + newProject->getName() +
					"\", \"Path\": \"" + newProject->getPath() +
					"\", \"Headers\": " + QString::number(newProject->getHeaders().size()) +
					", \"Sources\": " + QString::number(newProject->getSources().size()) + " }");
				
				m_projects.insert_or_assign(newProject->getName(), newProject);
			}
		}
	}
}

std::string AppBase::intervalString(long long _ms) {
	long long minutes = _ms / 60000;
	long long seconds = (_ms % 60000) / 1000;
	long long milliseconds = _ms % 1000;

	std::ostringstream oss;
	if (minutes > 0) {
		oss << minutes << " minutes " << std::setw(2) << std::setfill('0') << seconds << "." << std::setw(3) << std::setfill('0') << milliseconds << " seconds";
	}
	else {
		oss << seconds << "." << std::setw(3) << std::setfill('0') << milliseconds << " seconds";
	}
	return oss.str();
}

