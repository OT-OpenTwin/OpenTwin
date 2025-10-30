// @otlicense
// File: PHREECLauncher.h
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

namespace ot {
	namespace components {
		class ModelComponent;
	}
}

class Application;

class PHREECLauncher
{
public:
	PHREECLauncher(Application *app);
	virtual ~PHREECLauncher();

	std::string startSolver(const std::string &dataBaseURL, const std::string &frequencyHz, const std::string &uiUrl, const std::string &projectName, const std::string &modelStateVersion, unsigned long long meshEntityID, bool debugFlag, int serviceID, int sessionCount, ot::components::ModelComponent *modelComponent);

private:
	std::string readOutputFile(const std::string &fileName);
	std::string getUniqueTempDir(void);
	std::string getSystemTempDir(void);
	std::string readEnvironmentVariable(const std::string &variableName);
	bool checkFileOrDirExists(const std::string &path);
	bool deleteDirectory(const std::string &pathName);
	bool runExecutableAndWaitForCompletion(std::string commandLine);
	std::string runPHREECSolver(const std::string &pmdFileName, const std::string &outputDirectory, const std::string &frequencyHz, const std::string &uiUrl);

	Application *application;
};
