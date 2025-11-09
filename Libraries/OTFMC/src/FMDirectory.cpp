// @otlicense
// File: FMDirectory.cpp
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
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"
#include "OTFMC/FMDirectory.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"

// std header
#include <filesystem>

ot::FMDirectory ot::FMDirectory::fromFileSystem(const std::filesystem::path& _path, const FMIgnoreFile& _ignoreData, const ScanFlags& _flags) {
    FMDirectory dir;
    
    std::error_code ec;
    if (!std::filesystem::exists(_path, ec) || !std::filesystem::is_directory(_path, ec)) {
        // Return empty FMDirectory if invalid path
        return dir;
    }

    dir.m_path = _path;

    if (_flags & ScanFlag::WriteOutput) {
        WindowAPI::appendOutputMessage("Scanning directory: " + _path.generic_string() + "\n");
    }

    // Iterate through directory entries
    for (const auto& entry : std::filesystem::directory_iterator(_path, ec)) {
        if (ec) {
			OT_LOG_E("Error accessing directory entry { \"Path\": \"" + _path.generic_string() + "\", \"Error\": \"" + ec.message() + "\" }");
			break;
        }

        const std::filesystem::path& p = entry.path();

		// Check ignore patterns
        if (_ignoreData.isIgnored(p)) {
            continue;
        }

		// Process directories and files
        if (entry.is_directory(ec)) {
            if (_flags.has(ScanFlag::ScanDirectories)) {
                if (_flags.has(ScanFlag::ScanChildDirectories)) {
                    dir.m_childDirectories.push_back(FMDirectory::fromFileSystem(p.string(), _ignoreData, _flags));
                }
                else {
                    // Only add top-level directory info without recursion
                    FMDirectory child;
                    child.m_path = p;
                    dir.m_childDirectories.push_back(std::move(child));
                }
            }
        }
        else if (entry.is_regular_file(ec)) {
            if (_flags.has(ScanFlag::ScanFiles)) {
                if (_flags & ScanFlag::WriteOutput) {
                    WindowAPI::appendOutputMessage("  Found file: " + p.generic_string() + "\n");
                }

                FileInformation info = FileInformation::fromFileSystem(entry.path());
                dir.m_files.push_back(std::move(info));
            }
        }
    }

    return dir;
}

void ot::FMDirectory::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("P", JsonString(m_path.generic_string(), _allocator), _allocator);
    JsonArray childDirsArr;
    for (const FMDirectory& childDir : m_childDirectories) {
		JsonObject childDirObj;
        childDir.addToJsonObject(childDirObj, _allocator);
		childDirsArr.PushBack(childDirObj, _allocator);
	}
    _jsonObject.AddMember("D", childDirsArr, _allocator);
    
    JsonArray filesArr;
    for (const FileInformation& fileInfo : m_files) {
        JsonObject fileObj;
        fileInfo.addToJsonObject(fileObj, _allocator);
        filesArr.PushBack(fileObj, _allocator);
	}
	_jsonObject.AddMember("F", filesArr, _allocator);
}

void ot::FMDirectory::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_childDirectories.clear();
	m_files.clear();

	m_path = std::filesystem::path(json::getString(_jsonObject, "P"));
    for (const ConstJsonObject& childDirObj : json::getObjectList(_jsonObject, "D")) {
        FMDirectory childDir;
        childDir.setFromJsonObject(childDirObj);
        m_childDirectories.push_back(std::move(childDir));
    }
    for (const ConstJsonObject& fileObj : json::getObjectList(_jsonObject, "F")) {
        FileInformation fileInfo;
        fileInfo.setFromJsonObject(fileObj);
        m_files.push_back(std::move(fileInfo));
	}
}

std::optional<ot::FileInformation> ot::FMDirectory::getFile(const std::string& _fileName) const {
	std::string normalizedFileName = "/" + _fileName;
    std::replace(normalizedFileName.begin(), normalizedFileName.end(), '\\', '/');

    for (const FileInformation& fileInfo : m_files) {
		std::string normalizedPath = fileInfo.getPath();
		std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');

        if (String::endsWith(normalizedPath, normalizedFileName)) {
            return fileInfo;
		}
	}

	return std::nullopt;
}
