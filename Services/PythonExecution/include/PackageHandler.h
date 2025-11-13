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

class PackageHandler
{
public:
	void importMissingPackages(const std::string _scriptContent);
	void setTargetPath(const std::string& _targetPath) 
	{ 
		m_targetPath = _targetPath; 
	}
private:
	std::string m_targetPath = "";

	const std::list<std::string> parseImportedPackages(const std::string _scriptContent);
	bool isPackageInstalled(const std::string& _packageName);
	void installPackage(const std::string& _packageName);

	std::string trim(const std::string& _line);
};
