// @otlicense
// File: Application.h
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

// std header
#include <list>
#include <string>
#include <vector>
#include <fstream>

class Application {
public:
	Application();
	~Application();

	int run(int _argc, char** _argv);

private:
	struct CommandLine {
		bool dryRun = false;
		bool showExpected = false;
		bool showConfig = false;
		bool notifyChangeOnly = false;
		bool showHelp = false;

		std::string configFileName;

		std::list<std::string> parseWarnings;
		std::list<std::string> parseErrors;
		std::list<std::string> parseInfos;
	};

	struct Config {
		bool dryRun = false; // Set from command line
		bool showExpected = false; // Set from command line
		bool notifyChangeOnly = false; // Set from command line

		bool isValid = false;
		bool warnMissingHeader = false;
		bool warnInvalidChars = false;
		bool addFileName = false;
		bool addLicenseTitle = false;
		std::string keywordStart;
		std::string keywordStartLower;
		std::string keywordIgnoreLower;
		std::string keywordEnd;
		std::string headerLinePrefix;
		std::list<std::string> licenseLines;
		std::list<std::string> rootDirectories;
		std::list<std::string> blacklistDirectories;
		std::list<std::string> fileExtensions;
	};

	Config m_config;

	struct ResultData{
		size_t directoriesScanned = 0;
		size_t filesScanned = 0;
		size_t outdateFiles = 0;
		size_t filesModified = 0;
		size_t filesWithMissingLicense = 0;
		size_t filesWithInvalidChars = 0;
		size_t filesEmpty = 0;
		size_t filesIgnored = 0;
		size_t filesUpToDate = 0;
		size_t errors = 0;
		size_t warnings = 0;
	};

	ResultData m_resultData;
	std::ofstream* m_outputStream = nullptr;

	void scanDirectory(const std::string& _directoryPath);
	void scanFile(const std::string& _filePath);

	bool useDirectory(const std::string& _directoryPath);
	bool useFile(const std::string& _filePath);

	void log(const std::string& _message);
	void logW(const std::string& _message);
	void logE(const std::string& _message);

	CommandLine parseCommandLine(int _argc, char** _argv);
	Config loadConfigFile(const std::string& _fileName);
	void showHelp();
	void showConfig();
	void showResults();

	std::string fileNameOnly(const std::string& _filePath) const;
	std::string stringToLower(const std::string& _string) const;
	std::string expandPath(const std::string& _path);
	std::string getEnvVar(const std::string& _varName);
};
