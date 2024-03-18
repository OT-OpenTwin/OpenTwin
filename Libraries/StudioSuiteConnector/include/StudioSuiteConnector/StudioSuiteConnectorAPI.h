#pragma once

#include <string>

#include <qobject.h>

#include "OTCore/CoreTypes.h"

namespace StudioSuiteConnectorAPI
{
	__declspec(dllexport) void setStudioServiceData(std::string studioSuiteServiceURL, QObject* mainObject);
	__declspec(dllexport) void importProject(std::string fileName, std::string projectName);
	__declspec(dllexport) void commitProject(std::string fileName, std::string projectName, std::string changeComment);
	__declspec(dllexport) void getProject(std::string fileName, std::string projectName, const std::string& version);
	__declspec(dllexport) std::string getCurrentVersion(std::string fileName, std::string projectName);
	__declspec(dllexport) void uploadFiles(std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList);
	__declspec(dllexport) void copyFiles(std::string newVersion);



}
