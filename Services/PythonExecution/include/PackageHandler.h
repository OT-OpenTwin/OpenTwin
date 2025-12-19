// @otlicense
// File: PackageHandler.h
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

#include <string>
#include <list>
#include "EntityPythonManifest.h"
#include <map>
#include "NewModelStateInfo.h"

class PackageHandler
{
public:
	PackageHandler() = default;
	~PackageHandler();
	PackageHandler(const PackageHandler& _other) = delete;
	PackageHandler(PackageHandler&& _other) = delete;
	PackageHandler& operator=(const PackageHandler& _other) = delete;
	PackageHandler& operator=(PackageHandler&& _other) = delete;

	//! @brief Checks if the manifest identified by _manifestUID is different from the current one. 
	//! Requests a restart if needed and initializes the manifest if this is the first call.
	void initializeManifest(ot::UID _manifestUID);
	void initializeEnvironmentWithManifest(const std::string& _environmentPath);

	void extractMissingPackages(const std::string& _scriptContent);
	void importMissingPackages();

	ot::UID getManifestUID();
	
	void clearBuffer() 
	{ 
		m_uninstalledPackages.clear(); 
	}

	void setRunningInCoreEnvironment()
	{
		m_environmentState = EnvironmentState::core;
	}

	void setRunningInFixedEnvironment()
	{
		m_environmentState = EnvironmentState::fixed;
	}

private:
	enum class EnvironmentState
	{
		empty,
		firstFilling,
		initialised,
		fixed,
		core
	};
		
	std::map<std::string, std::string> m_installedPackageVersionsByName;
	EntityPythonManifest* m_currentManifest = nullptr;
	std::string m_environmentPath;

	EnvironmentState m_environmentState = EnvironmentState::empty;
	std::list<std::string> m_uninstalledPackages;
	std::string m_installationLog;

	void requestRestart();
	const std::list<std::string> parseImportedPackages(const std::string _scriptContent);
	bool isPackageInstalled(const std::string& _packageName);
	void installPackage(const std::string& _packageName);
	void dropImportCache();
	EntityPythonManifest* loadManifestEntity(ot::UID _manifestUID);
	ot::UID getUIDFromString(const std::string& _uid);
	std::string getListOfInstalledPackages();
	void storeInstallationLog(ot::NewModelStateInfo& _newState);
	void buildPackageMap(const std::string& _packageList);
	std::string trim(const std::string& _line);
	std::pair<std::string, std::string> splitPackageIntoNameAndVersion(const std::string& _requirementLine);
};
