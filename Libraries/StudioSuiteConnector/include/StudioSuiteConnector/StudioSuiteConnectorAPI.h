// @otlicense
// File: StudioSuiteConnectorAPI.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"

// std header
#include <string>

namespace StudioSuiteConnectorAPI
{
	__declspec(dllexport) std::string processAction(std::string action, ot::JsonDocument& doc, std::string projectName);

	__declspec(dllexport) void openProject();
	__declspec(dllexport) void setStudioServiceData(std::string studioSuiteServiceURL);
	__declspec(dllexport) void importProject(std::string fileName, std::string projectName, std::string message, bool includeResults, bool includeParametricResults);
	__declspec(dllexport) void commitProject(std::string fileName, std::string projectName, std::string changeComment, bool includeResults, bool includeParametricResults);
	__declspec(dllexport) void getProject(std::string fileName, std::string projectName, std::string version);
	__declspec(dllexport) std::string getCurrentVersion(std::string fileName, std::string projectName);
	__declspec(dllexport) void uploadFiles(std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList, ot::UID infoEntityID, ot::UID infoEntityVersion);
	__declspec(dllexport) void downloadFiles(std::string fileName, std::string projectName, std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList, std::string version);
	__declspec(dllexport) void copyFiles(std::string newVersion);
	__declspec(dllexport) std::string getLocalFileName();
	__declspec(dllexport) void setLocalFileName(std::string fileName);
	__declspec(dllexport) bool checkValidLocalFile(std::string fileName, std::string projectName, bool ensureProjectExists, std::string& errorMessage);
	__declspec(dllexport) void setAndStoreLocalFileName(std::string fileName, const std::string& studioSuiteServiceURL);

	std::string getStudioSuiteFileNameForCommit(const std::string &projectName, const std::string& studioSuiteServiceURL);
	std::string getStudioSuiteFileNameForGet(const std::string &projectName, const std::string& studioSuiteServiceURL);
	std::string getLocalFileNameFromProject(const std::string& studioSuiteServiceURL);
	std::string getSimpleFileNameFromProject(const std::string& studioSuiteServiceURL);

}
