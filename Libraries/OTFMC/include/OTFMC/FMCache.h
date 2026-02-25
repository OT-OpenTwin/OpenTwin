// @otlicense
// File: FMCache.h
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

// OpenTwin header
#include "OTSystem/IgnoreRules.h"
#include "OTSystem/FileSystem/FileInformation.h"
#include "OTFMC/FMProjectPath.h"
#include "OTModelEntities/NewModelStateInfo.h"

namespace ot {

	class OT_FMC_API_EXPORT FMCache : public Serializable {
		OT_DECL_NOCOPY(FMCache)
		OT_DECL_DEFMOVE(FMCache)
	public:
		struct CacheEntry {
			std::filesystem::path filePath;
			size_t fileSize = 0;
			BasicEntityInformation entity;
		};

		struct StateInfo {
			std::vector<std::filesystem::path> newFiles;
			std::vector<std::filesystem::path> deletedFiles;
			std::vector<std::filesystem::path> modifiedFiles;
		};

		FMCache() = default;
		~FMCache() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		FMCache::StateInfo determineNewState(const FMProjectPath& _projectPath, const IgnoreRules& _ignoreRules);

	private:
		static const int c_currentCacheVersion = 1;
		
		std::map<std::filesystem::path, UIDList> m_pathToEntityUIDs;
		std::map<UID, CacheEntry> m_entries;

	};
}
