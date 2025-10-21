// @otlicense
// File: Application.h
// 
// License:
// The MIT License (MIT)
// 
// Copyright (c) 2021-2024 OpenTwin
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 
// @otlicense-end

#pragma once

// std header
#include <list>
#include <string>
#include <vector>

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
		std::string configFileName;
	};

	struct Config {
		bool dryRun = false; // Set from command line
		bool showExpected = false; // Set from command line

		bool isValid = false;
		bool warnMissingHeader = false;
		bool addFileName = false;
		bool addLicenseTitle = false;
		std::string keywordStart;
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
		size_t filesEmpty = 0;
		size_t filesUpToDate = 0;
		size_t errors = 0;
		size_t warnings = 0;
	};

	ResultData m_resultData;

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
