// @otlicense
// File: Application.cpp
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

// OpenTwin header
#include "Application.h"

// rapidjson headers
#include "rapidjson/document.h"

// std header
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

#define LHU_VERSION "1.0"

Application::Application() {
}

Application::~Application() {

}

int Application::run(int _argc, char** _argv) {
	auto start = std::chrono::system_clock::now();

	log("OpenTwin - Starting File Header Updater v" LHU_VERSION);

	// Parse command line
	CommandLine cmdLine = parseCommandLine(_argc, _argv);
	if (cmdLine.configFileName.empty()) {
		log("No config file specified. Use --help or -h for help.");
		return 1;
	}

	// Load config file
	m_config = loadConfigFile(cmdLine.configFileName);
	if (!m_config.isValid) {
		logE("Failed to import config file. Aborting... File: \"" + cmdLine.configFileName + "\"");
		return 2;
	}
	m_config.dryRun |= cmdLine.dryRun;
	m_config.showExpected |= cmdLine.showExpected;

	if (cmdLine.showConfig) {
		showConfig();
	}

	// Scan root directories
	for (const std::string& rootDir : m_config.rootDirectories) {
		log("Scanning root directory: \"" + rootDir + "\"");
		scanDirectory(rootDir);
	}

	// Show results
	showResults();

	// Show elapsed time
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	double seconds = elapsed.count();

	std::ostringstream oss;
	oss << std::fixed << std::setprecision(3) << seconds;
	std::string durationStr = oss.str();

	log("Finished (Took: " + durationStr + "s)");
	return 0;
}

void Application::scanDirectory(const std::string& _directoryPath) {
	// Ensure directory exists
	if (!std::filesystem::exists(_directoryPath)) {
		logW("Directory does not exist: \"" + _directoryPath + "\"");
		return;
	}

	m_resultData.directoriesScanned++;
	
	// Iterate through files and directories
	for (const auto& entry : std::filesystem::directory_iterator{ _directoryPath }) {
		if (entry.is_directory()) {
			// Check if directory should be scanned
			std::string dirPath = entry.path().string();
			if (useDirectory(dirPath)) {
				// Recurse into directory
				scanDirectory(dirPath);
			}

		}
		else if (entry.is_regular_file()) {
			// Check file extension
			std::string filePath = entry.path().string();
			std::string fileExtension = entry.path().extension().string();
			if (useFile(filePath)) {
				scanFile(filePath);
			}
		}
	}
}

void Application::scanFile(const std::string& _filePath) {
	// Read file lines
	std::ifstream fileStream(_filePath);
	if (!fileStream.is_open()) {
		logE("Failed to open file: \"" + _filePath + "\"");
		return;
	}

	m_resultData.filesScanned++;

	std::list<std::string> fileLines;
	std::string line;

	// Read first line
	if (!std::getline(fileStream, line)) {
		log("File is empty: \"" + _filePath + "\"");
		fileStream.close();

		m_resultData.filesEmpty++;
		return;
	}

	std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) { return std::tolower(c); });

	// Check for file header
	if (line != m_config.keywordStart) {
		if (m_config.warnMissingHeader) {
			log("File missing license header: \"" + _filePath + "\"");
		}

		fileStream.close();

		m_resultData.filesWithMissingLicense++;
		return;
	}
	fileLines.push_back(line);

	// Read remaining lines
	while (std::getline(fileStream, line)) {
		fileLines.push_back(line);
	}

	// If the file ends with a newline, add an empty line
	if (!fileLines.empty()) {
		fileStream.clear(); // reset EOF flag
		fileStream.seekg(-1, std::ios_base::end);
		char lastChar;
		if (fileStream.get(lastChar) && lastChar == '\n') {
			fileLines.push_back(std::string());
		}
	}

	fileStream.close();

	// Create expected license header
	std::list<std::string> expectedLicenseLines = m_config.licenseLines;

	// Add optional file name and license title
	if (m_config.addLicenseTitle) {
		expectedLicenseLines.push_front(m_config.headerLinePrefix + "License:");
	}

	if (m_config.addFileName) {
		if (m_config.addLicenseTitle) {
			expectedLicenseLines.push_front(m_config.headerLinePrefix);
		}
		
		expectedLicenseLines.push_front(m_config.headerLinePrefix + "File: " + fileNameOnly(_filePath));
	}

	expectedLicenseLines.push_front(m_config.keywordStart);
	expectedLicenseLines.push_back(m_config.keywordEnd);

	// Compare license header
	auto fileLineIt = fileLines.begin();
	auto expectedLineIt = expectedLicenseLines.begin();
	bool licenseMismatch = false;

	for (size_t i = 0; i < expectedLicenseLines.size(); i++) {
		if (fileLineIt == fileLines.end()) {
			licenseMismatch = true;
			break;
		}
		if (*fileLineIt != *expectedLineIt) {
			licenseMismatch = true;
			break;
		}

		fileLineIt++;
		expectedLineIt++;
	}

	if (!licenseMismatch) {
		m_resultData.filesUpToDate++;
		return;
	}

	if (m_config.dryRun && !m_config.showExpected) {
		log("License header needs to be updated: \"" + _filePath + "\"");
		m_resultData.outdateFiles++;
		return;
	}

	// Update license header
	std::list<std::string> newFileLines = expectedLicenseLines;

	// Skip existing license lines in the file
	fileLineIt = fileLines.begin();
	if (fileLineIt != fileLines.end() && *fileLineIt == m_config.keywordStart) {
		fileLineIt++;
	}
	while (fileLineIt != fileLines.end()) {
		if (*fileLineIt == m_config.keywordEnd) {
			fileLineIt++;
			break;
		}
		else if (fileLineIt->find(m_config.headerLinePrefix) > 0) {
			break;
		}
		fileLineIt++;
	}

	// Add remaining file lines
	for (; fileLineIt != fileLines.end(); fileLineIt++) {
		newFileLines.push_back(*fileLineIt);
	}

	if (m_config.showExpected) {
		log("License header needs to be updated: \"" + _filePath + "\"");

		log("Preview:");
		size_t lineNum = 1;
		for (const std::string& newLine : newFileLines) {
			log(std::to_string(lineNum++) + "\t" + newLine);
		}
		log("");
		m_resultData.outdateFiles++;
		return;
	}

	// Write updated file
	std::ofstream outFileStream(_filePath, std::ios::trunc);
	if (!outFileStream.is_open()) {
		logE("Failed to open file for writing: \"" + _filePath + "\"");
		return;
	}

	std::string* currentLine = nullptr;
	for (auto it = newFileLines.begin(); it != newFileLines.end(); ) {
		currentLine = &(*it);
		it++;
		if (it == newFileLines.end()) {
			outFileStream << *currentLine;
			break;
		}
		else {
			outFileStream << *currentLine << std::endl;
		}
	}
	outFileStream.close();
	log("File updated: \"" + _filePath + "\"");
	m_resultData.filesModified++;
}

bool Application::useDirectory(const std::string& _directoryPath) {
	for (const std::string& blacklistedDir : m_config.blacklistDirectories) {
		if (stringToLower(_directoryPath).find(blacklistedDir) != std::string::npos) {
			return false;
		}
	}
	return true;
}

bool Application::useFile(const std::string& _filePath) {
	for (const std::string& ext : m_config.fileExtensions) {
		if (stringToLower(_filePath).rfind(ext) == (_filePath.length() - ext.length())) {
			return true;
		}
	}
	return false;
}

void Application::log(const std::string& _message) {
	std::cout << "[OT FHU] " + _message + "\n";
	std::cout.flush();
}

void Application::logW(const std::string& _message) {
	log("[WARNING] " + _message);
	m_resultData.warnings++;
}

void Application::logE(const std::string& _message) {
	log("[ERROR] " + _message);
}

Application::CommandLine Application::parseCommandLine(int _argc, char** _argv) {
	CommandLine cmdLine;
	
	for (int i = 1; i < _argc; i++) {
		std::string arg = _argv[i];
		if (arg == "--config") {
			if (!cmdLine.configFileName.empty()) {
				logW("Multiple config files specified. Using first specified...");
				continue;
			}

			if ((i + 1) < _argc) {
				cmdLine.configFileName = expandPath(_argv[i + 1]);
				i++;
			}
		}
		else if (arg == "--dry") {
			log("Performing dry run (no files will be modified)...");
			cmdLine.dryRun = true;
		}
		else if (arg == "--preview") {
			log("Show modified files data (no files will be modified)...");
			cmdLine.showExpected = true;
		}
		else if (arg == "--help" || arg == "-h") {
			showHelp();
		}
		else if (arg == "--showconfig") {
			cmdLine.showConfig = true;
		}
		else {
			logW("Unknown command line argument: \"" + arg + "\"");
		}
	}

	return cmdLine;
}

std::string toString(rapidjson::ParseErrorCode _code) {
	switch (_code) {
	case rapidjson::kParseErrorNone:
		return "No error";
	case rapidjson::kParseErrorDocumentEmpty:
		return "Document is empty";
	case rapidjson::kParseErrorDocumentRootNotSingular:
		return "Document root not singular";
	case rapidjson::kParseErrorValueInvalid:
		return "Invalid value";
	case rapidjson::kParseErrorObjectMissName:
		return "Missing object name";
	case rapidjson::kParseErrorObjectMissColon:
		return "Object missing colon";
	case rapidjson::kParseErrorObjectMissCommaOrCurlyBracket:
		return "Object missing comma or curly bracket";
	case rapidjson::kParseErrorArrayMissCommaOrSquareBracket:
		return "Array missing comma or square bracket";
	case rapidjson::kParseErrorStringUnicodeEscapeInvalidHex:
		return "String has invalid unicode escape hex";
	case rapidjson::kParseErrorStringUnicodeSurrogateInvalid:
		return "String has invalid unicode surrogate";
	case rapidjson::kParseErrorStringEscapeInvalid:
		return "String has invalid escape";
	case rapidjson::kParseErrorStringMissQuotationMark:
		return "String missing quotation mark";
	case rapidjson::kParseErrorStringInvalidEncoding:
		return "String has invalid encoding";
	case rapidjson::kParseErrorNumberTooBig:
		return "Number too big";
	case rapidjson::kParseErrorNumberMissFraction:
		return "Number missing fraction";
	case rapidjson::kParseErrorNumberMissExponent:
		return "Number missing exponent";
	case rapidjson::kParseErrorTermination:
		return "Termination error";
	case rapidjson::kParseErrorUnspecificSyntaxError:
		return "Unspecific syntax error";
	default:
		return "Unknown error";
	}
}

Application::Config Application::loadConfigFile(const std::string& _fileName) {
	Config cfg;
	cfg.isValid = false;

	// Read file content
	std::ifstream configFileStream(_fileName);
	if (!configFileStream.is_open()) {
		logE("Failed to open config file \"" + _fileName + "\"");
		return cfg;
	}
	std::string fileContent((std::istreambuf_iterator<char>(configFileStream)), std::istreambuf_iterator<char>());
	configFileStream.close();

	// Parse config file
	rapidjson::Document doc;
	if (doc.Parse(fileContent.c_str()).HasParseError()) {
		logE("Failed to parse config file \"" + _fileName + "\": JSON error: " + toString(doc.GetParseError()));
		return cfg;
	}
	if (!doc.IsObject()) {
		logE("Invalid config file format: Root element is not an object");
		return cfg;
	}

	size_t licenseLineCount = 0;
	std::list<std::string> tmpLicenseLines;

	// Read license file path
	if (doc.HasMember("LicenseFile") && doc["LicenseFile"].IsString()) {
		std::string licenseFilePath = expandPath(doc["LicenseFile"].GetString());
		std::ifstream licenseFileStream(licenseFilePath);
		if (!licenseFileStream.is_open()) {
			logE("Failed to open license file: \"" + licenseFilePath + "\"");
			return cfg;
		}
		std::string line;
		while (std::getline(licenseFileStream, line)) {
			tmpLicenseLines.push_back(line);
			licenseLineCount++;
		}

		// If the file ends with a newline, add an empty line
		if (!tmpLicenseLines.empty()) {
			licenseFileStream.clear(); // reset EOF flag
			licenseFileStream.seekg(-1, std::ios_base::end);
			char lastChar;
			if (licenseFileStream.get(lastChar) && lastChar == '\n') {
				tmpLicenseLines.push_back(std::string());
			}
		}

		licenseFileStream.close();
		if (licenseLineCount == 0) {
			logE("Invalid license file content: No lines found in license file");
			return cfg;
		}
	}
	else {
		logE("Invalid config file format: Missing or invalid \"LicenseFilePath\" string");
		return cfg;
	}

	// Read keyword start
	if (doc.HasMember("Keyword.Start") && doc["Keyword.Start"].IsString()) {
		cfg.keywordStart = doc["Keyword.Start"].GetString();
		cfg.keywordStartLower = cfg.keywordStart;
		std::transform(cfg.keywordStartLower.begin(), cfg.keywordStartLower.end(), cfg.keywordStartLower.begin(), 
			[](unsigned char c) { return std::tolower(c); });

		if (cfg.keywordStart.empty()) {
			logE("Invalid config content: Empty \"Keyword.Start\" string");
			return cfg;
		}
	}
	else {
		logE("Invalid config file format: Missing or invalid \"Keyword.Start\" string");
		return cfg;
	}

	// Read keyword end
	if (doc.HasMember("Keyword.End") && doc["Keyword.End"].IsString()) {
		cfg.keywordEnd = doc["Keyword.End"].GetString();
		
		if (cfg.keywordEnd.empty()) {
			logE("Invalid config content: Empty \"Keyword.End\" string");
			return cfg;
		}
	}
	else {
		logE("Invalid config file format: Missing or invalid \"Keyword.End\" string");
		return cfg;
	}

	// Read license line prefix
	if (doc.HasMember("HeaderLinePrefix")){
		if (!doc["HeaderLinePrefix"].IsString()) {
			logE("Invalid config file format: Invalid \"HeaderLinePrefix\" string");
			return cfg;
		}

		cfg.headerLinePrefix = doc["HeaderLinePrefix"].GetString();
	}

	// Read warn missing license flag
	if (doc.HasMember("WarnMissingHeader")) {
		if (!doc["WarnMissingHeader"].IsBool()) {
			logE("Invalid config file format: Invalid \"WarnMissingHeader\" boolean");
			return cfg;
		}
		cfg.warnMissingHeader = doc["WarnMissingHeader"].GetBool();
	}

	// Read root directories
	if (doc.HasMember("RootDirectories") && doc["RootDirectories"].IsArray()) {
		const rapidjson::Value& roots = doc["RootDirectories"];
		for (rapidjson::SizeType i = 0; i < roots.Size(); i++) {
			if (roots[i].IsString()) {
				cfg.rootDirectories.push_back(expandPath(roots[i].GetString()));
			}
		}
		cfg.rootDirectories.unique();

		if (cfg.rootDirectories.empty()) {
			logE("Invalid config content: No root directories specified");
			return cfg;
		}
	}
	else {
		logE("Invalid config file format: Missing or invalid \"RootDirectories\" array");
		return cfg;
	}

	// Read file extensions
	if (doc.HasMember("FileExtensions") && doc["FileExtensions"].IsArray()) {
		const rapidjson::Value& extensions = doc["FileExtensions"];
		for (rapidjson::SizeType i = 0; i < extensions.Size(); i++) {
			if (extensions[i].IsString()) {
				cfg.fileExtensions.push_back(stringToLower(extensions[i].GetString()));
			}
		}

		cfg.fileExtensions.unique();
		if (cfg.fileExtensions.empty()) {
			logE("Invalid config content: No file extensions specified");
			return cfg;
		}
	}
	else {
		logE("Invalid config file format: Missing or invalid \"FileExtensions\" array");
		return cfg;
	}

	// Read blacklist directories
	if (doc.HasMember("BlacklistDirectories")) {
		if (!doc["BlacklistDirectories"].IsArray()) {
			logE("Invalid config file format: Invalid \"BlacklistDirectories\" array");
			return cfg;
		}
		
		const rapidjson::Value& blacklist = doc["BlacklistDirectories"];
		for (rapidjson::SizeType i = 0; i < blacklist.Size(); i++) {
			if (blacklist[i].IsString()) {
				cfg.blacklistDirectories.push_back(stringToLower(expandPath(blacklist[i].GetString())));
			}
		}

		cfg.blacklistDirectories.unique();
	}

	// Read add file name flag
	if (doc.HasMember("AddFileName")) {
		if (!doc["AddFileName"].IsBool()) {
			logE("Invalid config file format: Invalid \"AddFileName\" boolean");
			return cfg;
		}
		cfg.addFileName = doc["AddFileName"].GetBool();
	}

	// Read add license title flag
	if (doc.HasMember("AddLicenseTitle")) {
		if (!doc["AddLicenseTitle"].IsBool()) {
			logE("Invalid config file format: Invalid \"AddLicenseTitle\" boolean");
			return cfg;
		}
		cfg.addLicenseTitle = doc["AddLicenseTitle"].GetBool();
	}

	// Set license lines
	for (const std::string& line : tmpLicenseLines) {
		cfg.licenseLines.push_back(cfg.headerLinePrefix + line);
	}

	cfg.isValid = true;
	return cfg;
}

void Application::showHelp() {
	log("------------------------------------------------- Help -----------------------------------------------------------------------------");
	log("Command line arguments:");
	log("  --config <file path>   Specifies the config file to use");
	log("  --dry                  Performs a dry run without modifying any files      [Optional]");
	log("  --preview              Shows the modified files data, will enable --dry    [Optional]");
	log("  --help, -h             Shows this help message                             [Optional]");
	log("  --showconfig           Shows the currently imported config                 [Optional]");
	log("");
	log("Config file format (JSON):");
	log("{");
	log("  ---REQUIRED---");
	log("");
	log("  \"LicenseFile\":          \"<path>\",                     // Path to the license file");
	log("  \"Keyword.Start\":        \"<string>\",                   // Start keyword indicating the beginning of the license header");
	log("  \"Keyword.End\":          \"<string>\",                   // End keyword indicating the end of the license header");
	log("  \"RootDirectories\":      [\"<path1>\", \"<path2>\", ...],  // List of root directories to scan");
	log("  \"FileExtensions\":       [\".ext1\", \".ext2\", ...],      // List of file extensions to consider");
	log("");
	log("  ---OPTIONAL---");
	log("");
	log("  \"AddFileName\":          <true|false>,                 // Add file name line to the license header");
	log("  \"AddLicenseTitle\":      <true|false>,                 // Add license title line to the license header");
	log("  \"HeaderLinePrefix\":     \"<string>\",                   // Prefix to add to each line of the license (e.g., comment characters)");
	log("  \"WarnMissingHeader\":    <true|false>,                 // Warn if a file is missing a header");
	log("  \"BlacklistDirectories\": [\"<path1>\", \"<path2>\", ...]   // List of directories to exclude from scanning");
	log("}");
	log("------------------------------------------------------------------------------------------------------------------------------------");

}

void Application::showConfig() {
	log("------------------------------------------------- Config ---------------------------------------------------------------------------");
	log("Dry Run:              " + std::string((m_config.dryRun || m_config.showExpected) ? "True" : "False"));
	log("Show Expected:        " + std::string(m_config.showExpected ? "True" : "False"));
	log("Add File Name:        " + std::string(m_config.addFileName ? "True" : "False"));
	log("Add License Title:    " + std::string(m_config.addLicenseTitle ? "True" : "False"));
	log("Warn Missing Header:  " + std::string(m_config.warnMissingHeader ? "True" : "False"));
	log("Keyword Start:        \"" + m_config.keywordStart + "\"");
	log("Keyword End:          \"" + m_config.keywordEnd + "\"");
	log("Header Line Prefix:   \"" + m_config.headerLinePrefix + "\"");
	log("Root Directories:");
	for (const std::string& dir : m_config.rootDirectories) {
		log("\t" + dir);
	}
	log("Blacklist Directories:");
	for (const std::string& dir : m_config.blacklistDirectories) {
		log("\t" + dir);
	}
	log("File Extensions:");
	for (const std::string& ext : m_config.fileExtensions) {
		log("\t" + ext);
	}
	log("License Lines:");
	for (const std::string& line : m_config.licenseLines) {
		log("\t" + line);
	}
	log("------------------------------------------------------------------------------------------------------------------------------------");
}

void Application::showResults()  {
	log("Results:");
	log("  Errors:                  " + std::to_string(m_resultData.errors));
	log("  Warnings:                " + std::to_string(m_resultData.warnings));
	log("  Directories Scanned:     " + std::to_string(m_resultData.directoriesScanned));
	log("  Files Scanned:           " + std::to_string(m_resultData.filesScanned));
	log("  Empty Files:             " + std::to_string(m_resultData.filesEmpty));
	log("  Files Up-To-Date:        " + std::to_string(m_resultData.filesUpToDate));
	log("  Files Missing License:   " + std::to_string(m_resultData.filesWithMissingLicense));
	log("  Files Require Update:    " + std::to_string(m_resultData.outdateFiles));
	log("  Files Modified:          " + std::to_string(m_resultData.filesModified));
}

std::string Application::fileNameOnly(const std::string& _filePath) const {
	std::string result = _filePath;
	std::replace(result.begin(), result.end(), '\\', '/');
	size_t ix = result.rfind('/');
	if (ix != std::string::npos) {
		return result.substr(ix + 1);
	}
	else {
		return result;
	}
}

std::string Application::stringToLower(const std::string& _string) const {
	std::string lowerString = _string;
	std::transform(lowerString.begin(), lowerString.end(), lowerString.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return lowerString;
}

std::string Application::expandPath(const std::string& _path) {
	std::string expandedPath;
	size_t currentIx = 0;
	size_t ix = _path.find('%');
	while (ix != std::string::npos) {
		size_t ix2 = _path.find('%', ix + 1);

		if (ix2 != std::string::npos) {
			std::string varName = _path.substr(ix + 1, ix2 - ix - 1);
			expandedPath.append(_path.substr(currentIx, ix - currentIx));
			expandedPath.append(getEnvVar(varName));
			currentIx = ix2 + 1;
			ix = _path.find('%', currentIx);
		}
		else {
			break;
		}
	}

	if (currentIx < _path.size()) {
		expandedPath.append(_path.substr(currentIx));
	}

	return expandedPath;
}

std::string Application::getEnvVar(const std::string& _varName) {
	char* ret = nullptr;
	std::string result;
#if defined(_WIN32) || defined(_WIN64)
	size_t ct = 0;
	if (_dupenv_s(&ret, &ct, _varName.c_str()) != 0) {
		if (ret) {
			logE("Failed to get environment variable \"" + _varName + "\"");
			delete[] ret;
			ret = nullptr;
		}
	}

	if (ret != nullptr) {
		result = std::string(ret);
	}
#else
	ret = std::getenv(_varName.c_str());
	if (ret == nullptr) {
		return std::string();
	}
	result = std::string(ret);
#endif

	if (result.empty()) {
		logW("Environment variable \"" + _varName + "\" is not set");
	}

	return result;
}
