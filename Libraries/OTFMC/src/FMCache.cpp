// @otlicense
// File: FMCache.cpp
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
#include "OTFMC/FMCache.h"
#include "OTCore/LogDispatcher.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"

// std header
#include <fstream>

void ot::FMCache::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("CacheVersion", c_currentCacheVersion, _allocator);
	JsonObject rootDirObj;
	m_rootDirectory.addToJsonObject(rootDirObj, _allocator);
	_jsonObject.AddMember("RootDirectory", rootDirObj, _allocator);
}

void ot::FMCache::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	int cacheVersion = json::getInt(_jsonObject, "CacheVersion");
	if (cacheVersion != c_currentCacheVersion) {
		OT_LOG_W("Unsupported cache version");
		return;
	}

	m_rootDirectory.setFromJsonObject(json::getObject(_jsonObject, "RootDirectory"));
}

void ot::FMCache::clear() {
	m_rootDirectory = FMDirectory();
}

bool ot::FMCache::updateCache(const std::string& _rootPath, const UpdateFlags& _updateFlags) {
	if (_updateFlags.has(UpdateFlag::ShowProgress)) {
		WindowAPI::setProgressBarVisibility("Scanning directory", true, false);
		WindowAPI::setProgressBarValue(0);
	}

	// Scan for ignore file
	FMIgnoreFile ignoreFile;
	FMDirectory::ScanFlags scanFlags = FMDirectory::ScanFlag::ScanFiles | FMDirectory::ScanFlag::ScanDirectories;
	FMDirectory rootDir = FMDirectory::fromFileSystem(_rootPath, ignoreFile, scanFlags);
	auto ignoreFileInfo = rootDir.getFile(ot::OpenTwinIgnoreFileName, FMDirectory::TopLevelOnly);
	if (ignoreFileInfo.has_value()) {
		if (!ignoreFile.parseFromFile(ignoreFileInfo->getPath())) {
			WindowAPI::appendOutputMessage(StyledTextBuilder() << "[" << StyledText::Error << "Error" << StyledText::ClearStyle << "] Failed to parse ignore file: " + ignoreFileInfo->getPath() + "\n");
			WindowAPI::setProgressBarVisibility("", false, false);
			return false;
		}
	}

	// Create cache directory if needed
	if (!rootDir.getChildDirectory(ot::OpenTwinCacheFolderName, FMDirectory::TopLevelOnly)) {
		std::filesystem::create_directories(rootDir.getPath() / ot::OpenTwinCacheFolderName);
	}


	// Scan top level directories and files
	if (_updateFlags.has(UpdateFlag::WriteOutput)) {
		scanFlags.set(FMDirectory::ScanFlag::WriteOutput);
	}

	rootDir = FMDirectory::fromFileSystem(_rootPath, ignoreFile, scanFlags);

	return true;
}
/*
bool ot::FMCache::updateCache(FMDirectory&& _rootDirectory, const UpdateFlags& _updateFlags) {
	if (_updateFlags.has(UpdateFlag::WriteOutput)) {
		WindowAPI::appendOutputMessage("Updating cache for root path: " + _rootDirectory.getPath().generic_string() + "\n");
	}
	if (_updateFlags.has(UpdateFlag::ShowProgress)) {
		WindowAPI::setProgressBarVisibility("Scanning directory", true, false);
		WindowAPI::setProgressBarValue(5);
	}

	// Perform internal update
	UpdateInfo updateInfo;
	if (!updateDirectory(m_rootDirectory, _rootDirectory, updateInfo, _updateFlags)) {
		WindowAPI::setProgressBarVisibility("", false, false);
		return false;
	}

	if (_updateFlags.has(UpdateFlag::ShowProgress)) {
		WindowAPI::setProgressBarVisibility("Writing to database", true, false);
		WindowAPI::setProgressBarValue(50);
	}

	// Write changes to database
	if (!writeToDatabase(updateInfo)) {
		WindowAPI::setProgressBarVisibility("", false, false);
		return false;
	}

	if (_updateFlags.has(UpdateFlag::WriteToDisk)) {
		if (_updateFlags.has(UpdateFlag::ShowProgress)) {
			WindowAPI::setProgressBarVisibility("Update cache file", true, false);
			WindowAPI::setProgressBarValue(95);
		}

		// Write updated cache to disk
		JsonDocument doc;
		addToJsonObject(doc, doc.GetAllocator());
		auto cachePath = _rootDirectory.getPath() / ot::OpenTwinCacheFileName;
		std::ofstream file(cachePath.native());
		if (!file.is_open()) {
			WindowAPI::appendOutputMessage(StyledTextBuilder() << "[" << StyledText::Error << "Error" << StyledText::ClearStyle << "] Failed to open cache file for writing: " + cachePath.generic_string() + "\n");
			WindowAPI::setProgressBarVisibility("", false, false);
			return false;
		}
		file << doc.toJson();
		file.close();
	}

	if (_updateFlags.has(UpdateFlag::ShowProgress)) {
		WindowAPI::setProgressBarValue(100);
		WindowAPI::setProgressBarVisibility("", false, false);
	}
	if (_updateFlags.has(UpdateFlag::WriteOutput)) {
		WindowAPI::appendOutputMessage("Cache update completed successfully.\n");
	}

	return true;
}
*/
bool ot::FMCache::updateDirectory(FMDirectory& _cachedDir, const FMDirectory& _newDir, UpdateInfo& _resultInfo, const UpdateFlags& _updateFlags) {
	if (_newDir.getPath() != _cachedDir.getPath()) {
		// Directory did not exist before, add it
		if (!_cachedDir.getPath().empty()) {
			OT_LOG_E("Directory paths do not match during cache update");
			return false;
		}

		_cachedDir.setPath(_newDir.getPath());

		// Add files
		for (const ot::FileInformation& newFileInfo : _newDir.getFiles()) {
			_resultInfo.newFiles.push_back(newFileInfo);
			_cachedDir.addFile(newFileInfo);
			if (_updateFlags.has(UpdateFlag::WriteOutput)) {
				WindowAPI::appendOutputMessage("New file detected: " + newFileInfo.getPath() + "\n");
			}
		}

		// Add child directories
		for (const FMDirectory& newChildDir : _newDir.getChildDirectories()) {
			FMDirectory cachedChildDir;
			updateDirectory(cachedChildDir, newChildDir, _resultInfo, _updateFlags);
			_cachedDir.addChildDirectory(std::move(cachedChildDir));
		}
	}
	else {
		// Directory exists, check for file changes
		if (_cachedDir.getPath().empty()) {
			OT_LOG_E("Cached directory path is empty during cache update");
			return false;
		}

		// Check for modified and removed files
		for (ot::FileInformation& cachedFileInfo : _cachedDir.getFiles()) {
			auto newFileInfo = _newDir.getFile(cachedFileInfo.getPath(), FMDirectory::TopLevelOnly);
			if (newFileInfo.has_value()) {
				// File exists, check for modification
				if (newFileInfo->getLastModified() != cachedFileInfo.getLastModified() || newFileInfo->getSize() != cachedFileInfo.getSize()) {
					_resultInfo.modifiedFiles.push_back(*newFileInfo);
					cachedFileInfo = *newFileInfo;
					if (_updateFlags.has(UpdateFlag::WriteOutput)) {
						WindowAPI::appendOutputMessage("Modified file detected: " + newFileInfo->getPath() + "\n");
					}
				}
			}
			else {
				// File was removed
				_resultInfo.deletedFiles.push_back(cachedFileInfo);
				if (_updateFlags.has(UpdateFlag::WriteOutput)) {
					WindowAPI::appendOutputMessage("Deleted file detected: " + cachedFileInfo.getPath() + "\n");
				}
			}
		}

		// Check for new files
		for (const ot::FileInformation& newFileInfo : _newDir.getFiles()) {
			auto cachedFileInfo = _cachedDir.getFile(newFileInfo.getPath(), FMDirectory::TopLevelOnly);
			if (!cachedFileInfo.has_value()) {
				// New file
				_resultInfo.newFiles.push_back(newFileInfo);
				_cachedDir.addFile(newFileInfo);
				if (_updateFlags.has(UpdateFlag::WriteOutput)) {
					WindowAPI::appendOutputMessage("New file detected: " + newFileInfo.getPath() + "\n");
				}
			}
		}
	}

	return true;
}

bool ot::FMCache::writeToDatabase(const UpdateInfo& _updateInfo) {
	return true;
}
