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
#include "OTSystem/FileSystem/AdvancedDirectoryIterator.h"
#include "OTCore/Logging/LogDispatcher.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"
#include "OTFMC/FMCache.h"

void ot::FMCache::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("CacheVersion", c_currentCacheVersion, _allocator);
	

}

void ot::FMCache::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	int cacheVersion = json::getInt(_jsonObject, "CacheVersion");
	if (cacheVersion != c_currentCacheVersion) {
		OT_LOG_W("Unsupported cache version");
		return;
	}

	
}

ot::FMCache::StateInfo ot::FMCache::determineNewState(const FMProjectPath& _projectPath, const IgnoreRules& _ignoreRules) {
	StateInfo newState;

	if (_projectPath.isEmpty()) {
		OT_LOG_E("Project path is empty");
		return newState;
	}

	AdvancedDirectoryIterator iter(_projectPath.getPath(), DirectoryIterator::Files, _ignoreRules);
	if (!iter.hasDirectory(_projectPath.getCacheFolderName())) {

	}

	return newState;
}
