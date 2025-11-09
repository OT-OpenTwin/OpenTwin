// @otlicense
// File: FMIgnoreFile.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTFMC/FMIgnoreFile.h"

// std header
#include <fstream>
#include <sstream>

bool ot::FMIgnoreFile::parseFromFile(const std::filesystem::path& _filePath) {
    std::ifstream file(_filePath);
    if (!file.is_open()) {
        OT_LOG_E("Failed to open file for reading: \"" + _filePath.generic_string() + "\"");
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseFromText(buffer.str());
}

bool ot::FMIgnoreFile::parseFromText(const std::string& _text) {
	bool success = true;
    m_patterns.clear();
	m_rawText = _text;

    std::istringstream stream(_text);
    std::string line;
    while (std::getline(stream, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Normalize slashes
        std::replace(line.begin(), line.end(), '\\', '/');

        try {
			const std::string regexStr = globToRegex(line);
            if (!regexStr.empty()) {
                m_patterns.emplace_back(regexStr, std::regex::icase);
            }
        }
        catch (const std::regex_error& _e) {
            OT_LOG_W("Invalid pattern in ignore file: " + std::string(_e.what()));
			success = false;
        }
    }

	return success;
}

bool ot::FMIgnoreFile::isIgnored(const std::filesystem::path& _path) const {
    if (m_patterns.empty()) return false;

    std::string normalized = normalizePath(_path);

    for (const std::regex& regex : m_patterns) {
        if (std::regex_search(normalized, regex)) {
            return true;
        }
    }

    return false;
}

void ot::FMIgnoreFile::clear() {
    m_patterns.clear();
	m_rawText.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

std::string ot::FMIgnoreFile::normalizePath(const std::filesystem::path& _path) {
    std::string s = _path.generic_string();
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::string ot::FMIgnoreFile::globToRegex(const std::string& _pattern) {
    if (_pattern.empty()) return "";

    std::string normalized = _pattern;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    bool dirOnly = false;
    if (normalized.back() == '/') {
        dirOnly = true;
        normalized.pop_back(); // remove trailing slash for regex construction
    }

    std::string regexStr;
    regexStr.reserve(normalized.size() * 2);

    // Anchor to start if pattern starts with '/'
    if (!normalized.empty() && normalized.front() == '/') {
        regexStr += "^";
        normalized.erase(normalized.begin());
    }
    else {
        regexStr += "(^|.*/)";
    }

    for (char c : normalized) {
        switch (c) {
        case '*': regexStr += ".*"; break;
        case '?': regexStr += '.'; break;
        case '.': case '(': case ')': case '+': case '|': case '^':
        case '$': case '{': case '}': case '[': case ']': case '\\':
            regexStr += '\\'; regexStr += c; break;
        default: regexStr += c; break;
        }
    }

    if (dirOnly) {
        regexStr += "(/.*)?$"; // match directory and everything inside
    }
    else {
        regexStr += "($|/.*)"; // match file or directory contents
    }

    return regexStr;
}
