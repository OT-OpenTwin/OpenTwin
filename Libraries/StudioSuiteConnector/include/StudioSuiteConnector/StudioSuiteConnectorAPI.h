#pragma once

#include <string>

#include <qobject.h>

#include "OTCore/CoreTypes.h"
#include "OTCore/JSON.h"

namespace StudioSuiteConnectorAPI
{
	__declspec(dllexport) std::string processAction(std::string action, ot::JsonDocument& doc, std::string projectName, QObject* mainObject, const QIcon& windowIcon);

	__declspec(dllexport) void openProject();
	__declspec(dllexport) void setStudioServiceData(std::string studioSuiteServiceURL, QObject* mainObject);
	__declspec(dllexport) void importProject(std::string fileName, std::string projectName);
	__declspec(dllexport) void commitProject(std::string fileName, std::string projectName, std::string changeComment);
	__declspec(dllexport) void getProject(std::string fileName, std::string projectName, std::string version);
	__declspec(dllexport) std::string getCurrentVersion(std::string fileName, std::string projectName);
	__declspec(dllexport) void uploadFiles(std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList);
	__declspec(dllexport) void downloadFiles(std::string fileName, std::string projectName, std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList, std::string version);
	__declspec(dllexport) void copyFiles(std::string newVersion);
	__declspec(dllexport) std::string getLocalFileName();
	__declspec(dllexport) void setLocalFileName(std::string fileName);
	__declspec(dllexport) bool checkValidLocalFile(std::string fileName, std::string projectName, bool ensureProjectExists);

	std::string getStudioSuiteFileNameForCommit(const std::string &projectName, const std::string& studioSuiteServiceURL, QObject* mainObject);
	std::string getStudioSuiteFileNameForGet(const std::string &projectName, const std::string& studioSuiteServiceURL, QObject* mainObject);
	std::string getLocalFileNameFromProject(const std::string& studioSuiteServiceURL, QObject* mainObject);
}
