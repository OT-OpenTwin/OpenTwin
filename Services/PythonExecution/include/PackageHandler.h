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
#include "PythonWrapper.h"
#include <map>

class PackageHandler
{
public:
	static PackageHandler& instance(void)
	{
		static PackageHandler g_instance;
		return g_instance;
	}

	~PackageHandler();
	//! @brief Handles package installation and manifest setting. 
	//! returns false if a restart is necessary.
	void initializeManifest(ot::UID _manifestUID);
	void initializeEnvironmentWithManifest(const std::string& _environmentPath);

	void extractMissingPackages(const std::string& _scriptContent);
	void importMissingPackages();
	std::string getEnvironmentName();

private:
	enum class EnvironmentState
	{
		empty,
		firstFilling,
		initialised,
	};
	
	PackageHandler() = default;

	std::map<std::string, std::string> m_installedPackageVersionsByName;

	void requestRestart();
	
	EntityPythonManifest* m_currentManifest = nullptr;
	std::string m_environmentPath;
	EnvironmentState m_environmentState = EnvironmentState::empty;
	std::list<std::string> m_uninstalledPackages;

	const std::list<std::string> parseImportedPackages(const std::string _scriptContent);
	bool isPackageInstalled(const std::string& _packageName);
	void installPackage(const std::string& _packageName);
	void dropImportCache();
	EntityPythonManifest* loadManifestEntity(ot::UID _manifestUID);
	ot::UID getUIDFromString(const std::string& _uid);
	std::string getListOfInstalledPackages();

	void buildPackageMap(const std::string& _packageList);

	std::string trim(const std::string& _line);
	std::pair<std::string, std::string> splitPackageIntoNameAndVersion(const std::string& _requirementLine);
};
