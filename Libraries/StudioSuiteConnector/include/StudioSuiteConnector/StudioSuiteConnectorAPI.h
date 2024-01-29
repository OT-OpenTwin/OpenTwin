#pragma once

#include <string>

#include <qobject.h>

namespace StudioSuiteConnectorAPI
{
	__declspec(dllexport) void setStudioServiceData(std::string studioSuiteServiceURL, QObject* mainObject);
	__declspec(dllexport) void importProject(std::string fileName, std::string projectName);



}
