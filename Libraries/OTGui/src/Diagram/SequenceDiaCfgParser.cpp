// @otlicense
// File: SequenceDiaCfgParser.cpp
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
#include "OTSystem/DateTime.h"
#include "OTSystem/FileSystem/FileSystem.h"
#include "OTSystem/FileSystem/AdvancedDirectoryIterator.h"
#include "OTCore/String.h"
#include "OTCore/ContainerHelper.h"
#include "OTGui/Diagram/SequenceDiaCfgParser.h"
#include "OTGui/Diagram/SequenceDiaSelfCallCfg.h"
#include "OTGui/Diagram/SequenceDiaDirectCallCfg.h"

#define OT_PARSE_LOG_D(___filePath, ___lineNr, ___message) OT_LOG_D(___filePath.generic_string() + "::" + std::to_string(___lineNr) + ": " + ___message)
#define OT_PARSE_LOG_W(___filePath, ___lineNr, ___message) OT_LOG_W(___filePath.generic_string() + "::" + std::to_string(___lineNr) + ": " + ___message)
#define OT_PARSE_LOG_E(___filePath, ___lineNr, ___message) OT_LOG_E(___filePath.generic_string() + "::" + std::to_string(___lineNr) + ": " + ___message)

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ot::SequenceDiaCfgParser::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("FilesParsed", m_filesParsed, _allocator);
	_jsonObject.AddMember("InvalidFunctions", m_invalidFunctions, _allocator);
	_jsonObject.AddMember("InvalidDiagrams", m_invalidDiagrams, _allocator);

	_jsonObject.AddMember("TotalRuntime", m_totalRuntime, _allocator);
	_jsonObject.AddMember("ParseTime", m_parseTime, _allocator);
	_jsonObject.AddMember("ValidationTime", m_validationTime, _allocator);

	JsonArray funcArr;
	for (const auto& func : m_sequenceFunctions) {
		OTAssert(func.first == func.second.getName(), "Function name key does not match function name value");
		funcArr.PushBack(JsonObject(func.second, _allocator), _allocator);
	}
	_jsonObject.AddMember("Functions", std::move(funcArr), _allocator);

	JsonArray diagArr;
	for (const auto& diag : m_sequenceDiagrams) {
		OTAssert(diag.first == diag.second.getName(), "Diagram name key does not match diagram name value");
		diagArr.PushBack(JsonObject(diag.second, _allocator), _allocator);
	}
	_jsonObject.AddMember("Diagrams", std::move(diagArr), _allocator);
}

void ot::SequenceDiaCfgParser::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	clear();

	m_filesParsed = json::getUInt64(_jsonObject, "FilesParsed");
	m_invalidFunctions = json::getUInt64(_jsonObject, "InvalidFunctions");
	m_invalidDiagrams = json::getUInt64(_jsonObject, "InvalidDiagrams");

	m_totalRuntime = json::getInt64(_jsonObject, "TotalRuntime");
	m_parseTime = json::getInt64(_jsonObject, "ParseTime");
	m_validationTime = json::getInt64(_jsonObject, "ValidationTime");

	for (const ConstJsonObject& funcObj : json::getObjectList(_jsonObject, "Functions")) {
		SequenceDiaFunctionCfg funcCfg(funcObj);
		m_sequenceFunctions.emplace(funcCfg.getName(), std::move(funcCfg));
	}

	for (const ConstJsonObject& diagObj : json::getObjectList(_jsonObject, "Diagrams")) {
		SequenceDiaRefCfg diagCfg(diagObj);
		m_sequenceDiagrams.emplace(diagCfg.getName(), std::move(diagCfg));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::SequenceDiaCfgParser::clear() {
	m_parseFlags = NoFlags;

	m_filesParsed = 0;
	m_invalidFunctions = 0;
	m_invalidDiagrams = 0;

	m_totalRuntime = 0;
	m_parseTime = 0;
	m_validationTime = 0;

	m_sequenceFunctions.clear();
	m_sequenceDiagrams.clear();
}

void ot::SequenceDiaCfgParser::parseDirectory(const std::filesystem::path& _rootDirectoryPath, bool _topLevelOnly, const std::list<std::string>& _fileExtensionWhitelist, const IgnoreRules& _ignoreRules, const ParseFlags& _parseFlags) {
	clear();

	m_parseFlags = _parseFlags;

	auto startTime = DateTime::msSinceEpoch();

	DirectoryIterator::BrowseMode browseMode = DirectoryIterator::Files;
	if (!_topLevelOnly) {
		browseMode.set(DirectoryIterator::Subdirectories);
	}

	std::list<std::string> extensionsLower;
	for (const std::string& ext : _fileExtensionWhitelist) {
		extensionsLower.push_back(String::toLower(ext));
	}

	AdvancedDirectoryIterator dirIt(_rootDirectoryPath, browseMode, _ignoreRules);
	while (dirIt.hasNext()) {
		FileInformation fileInfo = dirIt.next();
		OTAssert(fileInfo.isFile(), "Expected file entry");
		
		if (!extensionsLower.empty()) {
			if (!ContainerHelper::contains(extensionsLower, String::toLower(fileInfo.getPath().extension().string()))) {
				continue;
			}
		}

		parseFile(fileInfo.getPath());
	}

	m_parseTime = DateTime::msSinceEpoch() - startTime;

	validate();

	m_totalRuntime = DateTime::msSinceEpoch() - startTime;
	m_validationTime = m_totalRuntime - m_parseTime;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Helper functions

std::string ot::SequenceDiaCfgParser::lineStr(const std::string& _filePath, size_t _lineNr) const {
	return _filePath + "::" + std::to_string(_lineNr);
}

std::list<std::pair<std::string, std::string>> ot::SequenceDiaCfgParser::getKeyValuePairs(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent) const {
	std::list<std::pair<std::string, std::string>> result;

	size_t currentPos = 0;
	size_t equalIx = _trimmedContent.find('=');
	while (equalIx != std::string::npos) {
		
		std::string key = String::trim(_trimmedContent.substr(currentPos, equalIx - currentPos));
		
		size_t openQuoteIx = _trimmedContent.find('"', equalIx + 1);
		if (openQuoteIx == std::string::npos) {
			if (!m_parseFlags.has(NoLogging)) {
				OT_PARSE_LOG_W(_filePath, _lineNr, "Missing opening quote for value of key \"" + key + "\"");
			}
			break;
		}

		size_t closeQuoteIx = _trimmedContent.find('"', openQuoteIx + 1);
		if (closeQuoteIx == std::string::npos) {
			if (!m_parseFlags.has(NoLogging)) {
				OT_PARSE_LOG_W(_filePath, _lineNr, "Missing closing quote for value of key \"" + key + "\"");
			}
			break;
		}

		std::string value = _trimmedContent.substr(openQuoteIx + 1, closeQuoteIx - openQuoteIx - 1);

		std::pair<std::string, std::string> pair;
		pair.first = std::move(key);
		pair.second = std::move(value);
		result.push_back(std::move(pair));

		currentPos = closeQuoteIx + 1;
		equalIx = _trimmedContent.find('=', currentPos);
	}

	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Parsing functions

void ot::SequenceDiaCfgParser::parseFile(const std::filesystem::path& _filePath) {
	m_filesParsed++;

	std::list<std::string> lines = FileSystem::readLines(_filePath.string());
	if (lines.empty()) {
		return;
	}

	SequenceDiaFunctionCfg currentFunction;
	size_t lineCt = 0;
	size_t prefLen = 0;

	for (const std::string& originalLine : lines) {
		lineCt++;
		const std::string line = String::trim(originalLine);
		
		if (line.empty()) {
			continue;
		}

		prefLen = 0;

		if (String::startsWith(line, "//@seq")) {
			prefLen = 6;
		}
		else if (String::startsWith(line, "// @seq")) {
			prefLen = 7;
		}

		if (prefLen > 0) {
			const std::string commandLine = String::trim(line.substr(prefLen));
			if (String::startsWith(commandLine, "func ")) {
				finalizeFunction(_filePath, lineCt, std::move(currentFunction));
				parseFunctionLine(_filePath, lineCt, String::trim(commandLine.substr(5)), currentFunction);
			}
			else if (String::startsWith(commandLine, "function ")) {
				finalizeFunction(_filePath, lineCt, std::move(currentFunction));
				parseFunctionLine(_filePath, lineCt, String::trim(commandLine.substr(9)), currentFunction);
			}
			else if (String::startsWith(commandLine, "endfunc")) {
				finalizeFunction(_filePath, lineCt, std::move(currentFunction));
				currentFunction.clear();
			}
			else if (String::startsWith(commandLine, "diagram ")) {
				parseDiagramLine(_filePath, lineCt, String::trim(commandLine.substr(8)));
			}
			else if (String::startsWith(commandLine, "dia ")) {
				parseDiagramLine(_filePath, lineCt, String::trim(commandLine.substr(4)));
			}
			else if (String::startsWith(commandLine, "start ")) {
				parseDiagramLine(_filePath, lineCt, String::trim(commandLine.substr(6)));
			}
			else if (String::startsWith(commandLine, "call ")) {
				if (!currentFunction.isValid()) {
					if (!m_parseFlags.has(NoLogging)) {
						OT_PARSE_LOG_W(_filePath, lineCt, "Ignoring 'seq call' command outside of function");
					}
					continue;
				}
				parseFunctionCallLine(_filePath, lineCt, String::trim(commandLine.substr(5)), currentFunction);
			}
			else if (String::startsWith(commandLine, "selfcall ")) {
				if (!currentFunction.isValid()) {
					if (!m_parseFlags.has(NoLogging)) {
						OT_PARSE_LOG_W(_filePath, lineCt, "Ignoring 'seq selfcall' command outside of function");
					}
					continue;
				}
				parseFunctionCallLine(_filePath, lineCt, String::trim(commandLine.substr(9)), currentFunction);
			}
			else if (String::startsWith(commandLine, "self ")) {
				if (!currentFunction.isValid()) {
					if (!m_parseFlags.has(NoLogging)) {
						OT_PARSE_LOG_W(_filePath, lineCt, "Ignoring 'seq self' command outside of function");
					}
					continue;
				}
				parseFunctionCallLine(_filePath, lineCt, String::trim(commandLine.substr(5)), currentFunction);
			}
			else if (String::startsWith(commandLine, "return ")) {
				if (!currentFunction.isValid()) {
					if (!m_parseFlags.has(NoLogging)) {
						OT_PARSE_LOG_W(_filePath, lineCt, "Ignoring 'return' command outside of function");
					}
					continue;
				}
				parseReturnLine(_filePath, lineCt, String::trim(commandLine.substr(7)), currentFunction);
				
				finalizeFunction(_filePath, lineCt, std::move(currentFunction));
				currentFunction.clear();
			}
		}
	}

	finalizeFunction(_filePath, lineCt, std::move(currentFunction));
}

void ot::SequenceDiaCfgParser::finalizeFunction(const std::filesystem::path& _filePath, size_t _lineNr, SequenceDiaFunctionCfg&& _function) {
	if (!_function.isValid()) {
		return;
	}

	auto it = m_sequenceFunctions.find(_function.getName());
	if (it != m_sequenceFunctions.end()) {
		if (!m_parseFlags.has(NoLogging)) {
			OT_PARSE_LOG_W(_filePath, _lineNr, "Function already defined { \"Function\": \"" + _function.getName() +
				"\", \"ExistingDefinition\": \"" + lineStr(it->second.getFilePath(), it->second.getLineNr()) + "\" }");
		}
		return;
	}

	m_sequenceFunctions.emplace(_function.getName(), std::move(_function));
}

void ot::SequenceDiaCfgParser::parseFunctionLine(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent, SequenceDiaFunctionCfg& _function) {
	_function.clear();
	
	_function.setFilePath(_filePath.generic_string());
	_function.setLineNr(_lineNr);
	
	for (const auto& pair : getKeyValuePairs(_filePath, _lineNr, _trimmedContent)) {
		const std::string& key = pair.first;
		const std::string& value = pair.second;
		if (key == "name") {
			_function.setName(value);
		}
		else if (key == "life" || key == "lifeline") {
			_function.setLifeLine(value);
		}
		else if (key == "txt" || key == "text" || key == "def" || key == "default") {
			_function.setDefaultText(value);
		}
		else {
			if (!m_parseFlags.has(NoLogging)) {
				OT_PARSE_LOG_W(_filePath, _lineNr, "Unknown key \"" + key + "\" in function definition. Ignoring...");
			}
		}
	}

	if (!_function.isValid()) {
		if (!m_parseFlags.has(NoLogging)) {
			OT_PARSE_LOG_W(_filePath, _lineNr, "Invalid function definition. Missing required keys. Ignoring...");
		}
		_function.clear();
	}
}

void ot::SequenceDiaCfgParser::parseDiagramLine(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent) {
	SequenceDiaRefCfg diagram;

	for (const auto& pair : getKeyValuePairs(_filePath, _lineNr, _trimmedContent)) {
		const std::string& key = pair.first;
		const std::string& value = pair.second;
		if (key == "name") {
			diagram.setName(value);
		}
		else if (key == "func" || key == "function") {
			diagram.setFunctionName(value);
		}
		else {
			if (!m_parseFlags.has(NoLogging)) {
				OT_PARSE_LOG_W(_filePath, _lineNr, "Unknown key \"" + key + "\" in diagram definition. Ignoring...");
			}
		}
	}

	if (!diagram.isValid()) {
		if (!m_parseFlags.has(NoLogging)) {
			OT_PARSE_LOG_W(_filePath, _lineNr, "Invalid diagram definition. Missing required keys. Ignoring...");
		}
		return;
	}

	auto it = m_sequenceDiagrams.find(diagram.getName());
	if (it != m_sequenceDiagrams.end()) {
		if (!m_parseFlags.has(NoLogging)) {
			OT_PARSE_LOG_W(_filePath, _lineNr, "Diagram already defined { \"Diagram\": \"" + diagram.getName() +
				"\", \"ExistingDefinition\": \"" + lineStr(it->second.getFilePath(), it->second.getLineNr()) + "\" }");
		}
		return;
	}

	m_sequenceDiagrams.emplace(diagram.getName(), std::move(diagram));
}

void ot::SequenceDiaCfgParser::parseFunctionCallLine(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent, SequenceDiaFunctionCfg& _function) {
	std::unique_ptr<SequenceDiaDirectCallCfg> call = std::make_unique<SequenceDiaDirectCallCfg>();
	
	auto keyValuePairs = getKeyValuePairs(_filePath, _lineNr, _trimmedContent);

	if (keyValuePairs.empty()) {
		// Simple call syntax: function name only
		call->setFunctionName(_trimmedContent);
	}
	else {
		for (const auto& kvp : keyValuePairs) {
			const std::string& key = kvp.first;
			const std::string& value = kvp.second;

			if (key == "func" || key == "function") {
				call->setFunctionName(value);
			}
			else if (key == "txt" || key == "text") {
				call->setText(value);
			}
			else {
				if (!m_parseFlags.has(NoLogging)) {
					OT_PARSE_LOG_W(_filePath, _lineNr, "Unknown key \"" + key + "\" in function call definition. Ignoring...");
				}
			}
		}
	}

	if (!call->isValid()) {
		if (!m_parseFlags.has(NoLogging)) {
			OT_PARSE_LOG_W(_filePath, _lineNr, "Invalid call definition. Missing required keys. Ignoring...");
		}
		return;
	}

	_function.addCall(call.release());
}

void ot::SequenceDiaCfgParser::parseSelfCallLine(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent, SequenceDiaFunctionCfg& _function) {
	std::unique_ptr<SequenceDiaSelfCallCfg> call = std::make_unique<SequenceDiaSelfCallCfg>();

	auto keyValuePairs = getKeyValuePairs(_filePath, _lineNr, _trimmedContent);

	if (keyValuePairs.empty()) {
		// Simple call syntax: function name only
		call->setText(_trimmedContent);
	}
	else {
		for (const auto& kvp : keyValuePairs) {
			const std::string& key = kvp.first;
			const std::string& value = kvp.second;

			if (key == "txt" || key == "text") {
				call->setText(value);
			}
			else {
				if (!m_parseFlags.has(NoLogging)) {
					OT_PARSE_LOG_W(_filePath, _lineNr, "Unknown key \"" + key + "\" in self call definition. Ignoring...");
				}
			}
		}
	}

	if (!call->isValid()) {
		if (!m_parseFlags.has(NoLogging)) {
			OT_PARSE_LOG_W(_filePath, _lineNr, "Invalid call definition. Missing required keys. Ignoring...");
		}
		return;
	}

	_function.addCall(call.release());
}

void ot::SequenceDiaCfgParser::parseReturnLine(const std::filesystem::path& _filePath, size_t _lineNr, const std::string& _trimmedContent, SequenceDiaFunctionCfg& _function) {
	if (!_function.getReturnValue().empty()) {
		if (!m_parseFlags.has(NoLogging)) {
			OT_PARSE_LOG_W(_filePath, _lineNr, "Multiple 'seq return' commands. Ignoring duplicate...");
		}
		return;
	}

	std::string cmd = String::trim(_trimmedContent.substr(std::string("return ").length()));
	if (cmd.empty()) {
		cmd = " ";
	}

	_function.setReturnValue(std::move(cmd));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Validation

void ot::SequenceDiaCfgParser::validate() {
	validateFunctions();
	validateDiagrams();
}

void ot::SequenceDiaCfgParser::validateFunctions() {
	const size_t functionCount = m_sequenceFunctions.size();
	if (functionCount == 0) {
		return;
	}

	// Assign stable indices
	ValidationInfo info(functionCount);
	size_t idx = 0;

	for (auto& kv : m_sequenceFunctions) {
		info.nameToIndex.emplace(kv.first, idx++);
		info.functions.push_back(&kv.second);
	}

	// Build adjacency list
	for (size_t i = 0; i < functionCount; ++i) {
		SequenceDiaFunctionCfg& func = *info.functions[i];

		for (SequenceDiaAbstractCallCfg* call : func.getCalls()) {
			if (call->getClassName() != SequenceDiaDirectCallCfg::className()) {
				continue;
			}

			auto* directCall = static_cast<SequenceDiaDirectCallCfg*>(call);
			const std::string& targetName = directCall->getFunctionName();

			auto it = info.nameToIndex.find(targetName);
			if (it == info.nameToIndex.end()) {
				// Undefined function
				info.state[i] = ValidationState::Invalid;

				if (!m_parseFlags.has(NoLogging)) {
					OT_LOG_W(
						"Function \"" + func.getName() +
						"\" calls undefined function \"" + targetName +
						"\". Removing function..."
					);
				}
				continue;
			}

			// Apply default text
			if (directCall->getText().empty()) {
				directCall->setText(info.functions[it->second]->getDefaultText());
			}

			info.graph[i].push_back(it->second);
		}
	}

	// Validate functions
	for (size_t ix = 0; ix < functionCount; ix++) {
		if (info.state[ix] == ValidationState::Unvisited) {
			validateNode(info, ix);
		}
	}

	// Remove invalid functions
	for (auto it = m_sequenceFunctions.begin(); it != m_sequenceFunctions.end();) {
		const size_t index = info.nameToIndex[it->first];

		if (info.state[index] == ValidationState::Invalid) {
			m_invalidFunctions++;
			it = m_sequenceFunctions.erase(it);
		}
		else {
			it++;
		}
	}
}

bool ot::SequenceDiaCfgParser::validateNode(ValidationInfo& _info, size_t _index) {
	if (_info.state[_index] == ValidationState::Valid) {
		return true;
	}
	if (_info.state[_index] == ValidationState::Invalid) {
		return false;
	}
	if (_info.state[_index] == ValidationState::Visiting) {
		// Recursion detected
		_info.state[_index] = ValidationState::Invalid;

		if (!m_parseFlags.has(NoLogging)) {
			OT_LOG_W("Detected recursive function call involving \"" +
				_info.functions[_index]->getName() +
				"\". Removing function..."
			);
		}
		return false;
	}

	_info.state[_index] = ValidationState::Visiting;

	for (size_t ix : _info.graph[_index]) {
		if (!validateNode(_info, ix)) {
			_info.state[_index] = ValidationState::Invalid;
			return false;
		}
	}

	_info.state[_index] = ValidationState::Valid;
	return true;
}

void ot::SequenceDiaCfgParser::validateDiagrams() {
	for (auto it = m_sequenceDiagrams.begin(); it != m_sequenceDiagrams.end(); ) {
		if (m_sequenceFunctions.find(it->second.getFunctionName()) == m_sequenceFunctions.end()) {
			if (!m_parseFlags.has(NoLogging)) {
				OT_LOG_W("Diagram \"" + it->second.getName() + "\" references undefined function \"" + it->second.getFunctionName() + "\". Removing diagram...");
			}
			m_invalidDiagrams++;
			it = m_sequenceDiagrams.erase(it);
		}
		else {
			it++;
		}
	}
}
