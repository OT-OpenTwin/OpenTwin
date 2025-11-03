// @otlicense
// File: Application.cpp
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
	if (m_outputStream) {
		m_outputStream->close();
		delete m_outputStream;
		m_outputStream = nullptr;
	}
}

int Application::run(int _argc, char** _argv) {
	auto start = std::chrono::system_clock::now();

	// Parse command line
	CommandLine cmdLine = parseCommandLine(_argc, _argv);
	
	log("OpenTwin - Starting File Header Updater v" LHU_VERSION);

	if (cmdLine.showHelp) {
		showHelp();
		return 0;
	}

	for (const std::string& error : cmdLine.parseErrors) {
		logE(error);
	}
	for (const std::string& warning : cmdLine.parseWarnings) {
		logW(warning);
	}
	for (const std::string& info : cmdLine.parseInfos) {
		log(info);
	}

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
	m_config.notifyChangeOnly |= cmdLine.notifyChangeOnly;

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
		if (!m_config.notifyChangeOnly) {
			log("File is empty: \"" + _filePath + "\"");
		}
		fileStream.close();

		m_resultData.filesEmpty++;
		return;
	}

	std::string lineLower = line;
	std::transform(lineLower.begin(), lineLower.end(), lineLower.begin(), [](unsigned char c) { return std::tolower(c); });

	// Check for file header
	StartLineInfo startLineInfo = analyzeStartLine(line);

	if (startLineInfo.signature != FileSignature::NoSignature && m_config.warnSignature) {
		logW("File with file signature (" + signatureToString(startLineInfo.signature) + ") detected: \"" + _filePath + "\".");
	}

	if (startLineInfo.isIgnore) {
		if (!m_config.notifyChangeOnly) {
			log("File ignored: \"" + _filePath + "\"");
		}
		fileStream.close();

		m_resultData.filesIgnored++;
		return;
	}
	else if (!startLineInfo.isStart) {
		if (m_config.warnMissingHeader) {
			logW("File missing license header: \"" + _filePath + "\"");
		}

		fileStream.close();

		m_resultData.filesWithMissingLicense++;
		return;
	}
	fileLines.push_back(startLineInfo.line);

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

	if (m_config.warnInvalidChars) {
		int lineNum = 1;
		bool hasInvalidChars = false;
		for (const std::string& l : fileLines) {
			int colNum = 1;
			for (const unsigned char c : l) {
				if ((c < 32 && c != '\t') || c > 126) {
					logW("Non ASCII character in file \"" + _filePath + "\" at " + 
						std::to_string(lineNum) + ":" + std::to_string(colNum) + 
						". Char(" + std::to_string(static_cast<uint32_t>(c)) + ")");
					if (!hasInvalidChars) {
						m_resultData.filesWithInvalidChars++;
					}
					hasInvalidChars = true;
				}
				colNum++;
			}
			lineNum++;
		}
	}

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

	expectedLicenseLines.push_front(startLineInfo.line);
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
	if (fileLineIt != fileLines.end() && *fileLineIt == startLineInfo.line) {
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
	if (m_outputStream) {
		(*m_outputStream) << "[OT FHU] " + _message + "\n";
	}
	std::cout << "[OT FHU] " + _message + "\n";
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
				cmdLine.parseWarnings.push_back("Multiple config files specified. Using first specified...");
				continue;
			}

			if ((i + 1) < _argc) {	
				cmdLine.configFileName = expandPath(_argv[i + 1]);
				i++;
			}
		}
		else if (arg == "--out") {
			if (m_outputStream) {
				cmdLine.parseWarnings.push_back("Multiple output files specified. Using first specified...");
				continue;
			}
			if ((i + 1) < _argc) {
				m_outputStream = new std::ofstream(expandPath(_argv[i + 1]));
				if (!m_outputStream->is_open()) {
					delete m_outputStream;
					m_outputStream = nullptr;
					cmdLine.parseErrors.push_back("Failed to open output file: \"" + std::string(_argv[i + 1]) + "\". Ignoring...");
				}
				i++;
			}
			else {
				cmdLine.parseWarnings.push_back("No output file specified after --out argument. Ignoring...");
			}
		}
		else if (arg == "--notifychangeonly") {
			cmdLine.parseInfos.push_back("Only notifying about changes (ignored/empty files will be skipped)...");
			cmdLine.notifyChangeOnly = true;
		}
		else if (arg == "--dry") {
			cmdLine.parseInfos.push_back("Performing dry run (no files will be modified)...");
			cmdLine.dryRun = true;
		}
		else if (arg == "--preview") {
			cmdLine.parseInfos.push_back("Show modified files data (no files will be modified)...");
			cmdLine.showExpected = true;
		}
		else if (arg == "--help" || arg == "-h") {
			cmdLine.showHelp = true;
		}
		else if (arg == "--showconfig") {
			cmdLine.showConfig = true;
		}
		else {
			cmdLine.parseWarnings.push_back("Unknown command line argument: \"" + arg + "\"");
		}
	}

	return cmdLine;
}

static std::string toString(rapidjson::ParseErrorCode _code) {
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
	fillFileSignatureMap(std::string(), cfg.signatureMap);

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

		std::string keywordLower = cfg.keywordStart;
		std::transform(keywordLower.begin(), keywordLower.end(), keywordLower.begin(),
			[](unsigned char c) { return std::tolower(c); });

		fillFileSignatureMap(cfg.keywordStart, cfg.keywordStartSignatures);

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

	// Read keyword ignore
	if (doc.HasMember("Keyword.Ignore") && doc["Keyword.Ignore"].IsString()) {
		cfg.keywordIgnore = doc["Keyword.Ignore"].GetString();
		std::transform(cfg.keywordIgnore.begin(), cfg.keywordIgnore.end(), cfg.keywordIgnore.begin(),
			[](unsigned char c) { return std::tolower(c); });

		fillFileSignatureMap(cfg.keywordIgnore, cfg.keywordIgnoreSignatures);
	}
	else {
		logE("Invalid config file format: Missing or invalid \"Keyword.Ignore\" string");
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

	// Read warn invalid chars flag
	if (doc.HasMember("WarnInvalidChars")) {
		if (!doc["WarnInvalidChars"].IsBool()) {
			logE("Invalid config file format: Invalid \"WarnInvalidChars\" boolean");
			return cfg;
		}
		cfg.warnInvalidChars = doc["WarnInvalidChars"].GetBool();
	}

	// Read warn signature flag
	if (doc.HasMember("WarnFileSignature")) {
		if (!doc["WarnFileSignature"].IsBool()) {
			logE("Invalid config file format: Invalid \"WarnFileSignature\" boolean");
			return cfg;
		}
		cfg.warnSignature = doc["WarnFileSignature"].GetBool();
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
	log("  --out <file path>      Specifies the output report file path (if not specified, output is shown on console only)");
	log("  --dry                  Performs a dry run without modifying any files                 [Optional]");
	log("  --preview              Shows the modified files data, will enable --dry               [Optional]");
	log("  --help, -h             Shows this help message                                        [Optional]");
	log("  --showconfig           Shows the currently imported config                            [Optional]");
	log("  --notifychangeonly     Only notifies about changes (ignored/empty files are skipped)  [Optional]");
	log("");
	log("Config file format (JSON):");
	log("{");
	log("  ---REQUIRED---");
	log("");
	log("  \"LicenseFile\":          \"<path>\",                     // Path to the license file");
	log("  \"Keyword.Start\":        \"<string>\",                   // Start keyword indicating the beginning of the license header");
	log("  \"Keyword.Ignore\":       \"<string>\",                   // Keyword indicating that the file should be ignored");
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
	log("  \"WarnInvalidChars\":     <true|false>,                 // Warn if a file contains non-ASCII characters");
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
	log("Warn Invalid Chars:   " + std::string(m_config.warnInvalidChars ? "True" : "False"));
	log("Notify Change Only:   " + std::string(m_config.notifyChangeOnly ? "True" : "False"));
	log("Keyword Start:        \"" + m_config.keywordStart + "\"");
	log("Keyword Ignore:       \"" + m_config.keywordIgnore + "\"");
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
	log("  Errors:                   " + std::to_string(m_resultData.errors));
	log("  Warnings:                 " + std::to_string(m_resultData.warnings));
	log("  Directories Scanned:      " + std::to_string(m_resultData.directoriesScanned));
	log("  Files Scanned:            " + std::to_string(m_resultData.filesScanned));
	log("  Empty Files:              " + std::to_string(m_resultData.filesEmpty));
	log("  Ignored Files:            " + std::to_string(m_resultData.filesIgnored));

	if (m_config.warnInvalidChars) {
	log("  Files with Invalid Chars: " + std::to_string(m_resultData.filesWithInvalidChars));
	}
	
	log("  Files Missing License:    " + std::to_string(m_resultData.filesWithMissingLicense));
	log("  Files Up-To-Date:         " + std::to_string(m_resultData.filesUpToDate));
	log("  Files Require Update:     " + std::to_string(m_resultData.outdateFiles));
	log("  Files Modified:           " + std::to_string(m_resultData.filesModified));
}

Application::StartLineInfo Application::analyzeStartLine(const std::string& _line) const {
	StartLineInfo info;
	auto ignoreIt = m_config.keywordIgnoreSignatures.find(_line);
	if (ignoreIt != m_config.keywordIgnoreSignatures.end()) {
		info.isIgnore = true;
		info.line = ignoreIt->first;
		info.signature = ignoreIt->second;
	}
	else {
		auto startIt = m_config.keywordStartSignatures.find(_line);
		if (startIt != m_config.keywordStartSignatures.end()) {
			info.isStart = true;
			info.line = startIt->first;
			info.signature = startIt->second;
		}
		else {
			for (const auto& it : m_config.signatureMap) {
				if (_line.rfind(it.first, 0) == 0) {
					info.signature = it.second;
					break;
				}
			}
		}
	}
	
	return info;
}

void Application::fillFileSignatureMap(const std::string& _text, std::map<std::string, FileSignature>& _map) {
	// No BOM (plain text)
	_map.emplace(_text, FileSignature::NoSignature);

	// UTF-8 BOM: EF BB BF
	_map.emplace(std::string{ static_cast<char>(0xEF), static_cast<char>(0xBB), static_cast<char>(0xBF) } + _text,
		FileSignature::BOM_UTF8);

	// UTF-16 LE BOM: FF FE
	_map.emplace(std::string{ static_cast<char>(0xFF), static_cast<char>(0xFE) } + _text,
		FileSignature::BOM_UTF16_LE);

	// UTF-16 BE BOM: FE FF
	_map.emplace(std::string{ static_cast<char>(0xFE), static_cast<char>(0xFF) } + _text,
		FileSignature::BOM_UTF16_BE);

	// UTF-32 LE BOM: FF FE 00 00
	_map.emplace(std::string{ static_cast<char>(0xFF), static_cast<char>(0xFE), 
							  static_cast<char>(0x00), static_cast<char>(0x00) } + _text,
		FileSignature::BOM_UTF32_LE);

	// UTF-32 BE BOM: 00 00 FE FF
	_map.emplace(std::string{ static_cast<char>(0x00), static_cast<char>(0x00),
							  static_cast<char>(0xFE), static_cast<char>(0xFF) } + _text,
		FileSignature::BOM_UTF32_BE);

	// UTF-7 BOM: 2B 2F 76 (and one of 38 | 39 | 2B | 2F)
	_map.emplace(std::string{ static_cast<char>(0x2B), static_cast<char>(0x2F),
							  static_cast<char>(0x76), static_cast<char>(0x38) } + _text,
		FileSignature::BOM_UTF7);
	_map.emplace(std::string{ static_cast<char>(0x2B), static_cast<char>(0x2F),
							  static_cast<char>(0x76), static_cast<char>(0x39) } + _text,
		FileSignature::BOM_UTF7);
	_map.emplace(std::string{ static_cast<char>(0x2B), static_cast<char>(0x2F),
							  static_cast<char>(0x76), static_cast<char>(0x2B) } + _text,
		FileSignature::BOM_UTF7);
	_map.emplace(std::string{ static_cast<char>(0x2B), static_cast<char>(0x2F),
							  static_cast<char>(0x76), static_cast<char>(0x2F) } + _text,
		FileSignature::BOM_UTF7);

	// UTF-1 BOM: F7 64 4C
	_map.emplace(std::string{ static_cast<char>(0xF7), static_cast<char>(0x64),
							  static_cast<char>(0x4C) } + _text,
		FileSignature::BOM_UTF1);

	// UTF-EBCDIC BOM: DD 73 66 73
	_map.emplace(std::string{ static_cast<char>(0xDD), static_cast<char>(0x73),
							  static_cast<char>(0x66), static_cast<char>(0x73) } + _text,
		FileSignature::BOM_UTF_EBCDIC);

	// SCSU BOM: 0E FE FF
	_map.emplace(std::string{ static_cast<char>(0x0E), static_cast<char>(0xFE),
							  static_cast<char>(0xFF) } + _text,
		FileSignature::BOM_SCSU);

	// BOCU-1 BOM: FB EE 28
	_map.emplace(std::string{ static_cast<char>(0xFB), static_cast<char>(0xEE),
							  static_cast<char>(0x28) } + _text,
		FileSignature::BOM_BOCU1);

	// GB-18030 BOM: 84 31 95 33
	_map.emplace(std::string{ static_cast<char>(0x84), static_cast<char>(0x31),
							  static_cast<char>(0x95), static_cast<char>(0x33) } + _text,
		FileSignature::BOM_GB18030);
}

std::string Application::signatureToString(FileSignature _signature) {
	switch (_signature) {
	case Application::FileSignature::NoSignature: return "No Signature";
	case Application::FileSignature::BOM_UTF8: return "UTF-8 BOM";
	case Application::FileSignature::BOM_UTF16_LE: return "UTF-16 LE BOM";
	case Application::FileSignature::BOM_UTF16_BE: return "UTF-16 BE BOM";
	case Application::FileSignature::BOM_UTF32_LE: return "UTF-32 LE BOM";
	case Application::FileSignature::BOM_UTF32_BE: return "UTF-32 BE BOM";
	case Application::FileSignature::BOM_UTF7: return "UTF-7 BOM";
	case Application::FileSignature::BOM_UTF1: return "UTF-1 BOM";
	case Application::FileSignature::BOM_UTF_EBCDIC: return "UTF-EBCDIC BOM";
	case Application::FileSignature::BOM_SCSU: return "SCSU BOM";
	case Application::FileSignature::BOM_BOCU1: return "BOCU-1 BOM";
	case Application::FileSignature::BOM_GB18030: return "GB-18030 BOM";
	default:
		logE("Unknown file signature value (" + std::to_string(static_cast<int>(_signature)) + ")");
		return "Unknown Signature";
	}
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
