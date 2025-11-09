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
#include "OTFMC/FMCTypes.h"
#include "OTFMC/FMDirectory.h"
#include "NewModelStateInfo.h"

namespace ot {

	class OT_FMC_API_EXPORT FMCache : public Serializable {
		OT_DECL_NOCOPY(FMCache)
		OT_DECL_DEFMOVE(FMCache)
	public:
		enum UpdateFlag : uint64_t {
			None           = 0 << 0, //! @brief No update flags.
			ShowProgress   = 1 << 0, //! @brief Show progress during the update.
			WriteOutput    = 1 << 1, //! @brief Write output messages during the update.
			UpdateDatabase = 1 << 2, //! @brief Write the changed files to the database.
			WriteToDisk    = 1 << 3  //! @brief Write the cache to disk after updating.
		};
		typedef Flags<UpdateFlag> UpdateFlags;

		FMCache() = default;
		~FMCache() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void clear();

		bool updateCache(const std::string& _rootPath, const UpdateFlags& _updateFlags);

		//! @brief Update the cache with new root directory data.
		//! @param _rootDirectory The new root directory data.
		bool updateCache(FMDirectory&& _rootDirectory, const UpdateFlags& _updateFlags);
		const FMDirectory& getRootDirectory() const { return m_rootDirectory; };

	private:
		static const int c_currentCacheVersion = 1;

		FMDirectory m_rootDirectory;

		struct UpdateInfo {
			std::list<FileInformation> newFiles;
			std::list<FileInformation> deletedFiles;
			std::list<FileInformation> modifiedFiles;
		};

		bool updateDirectory(FMDirectory& _cachedDir, const FMDirectory& _newDir, UpdateInfo& _resultInfo, const UpdateFlags& _updateFlags);
		bool writeToDatabase(const UpdateInfo& _updateInfo);
	};
}

OT_ADD_FLAG_FUNCTIONS(ot::FMCache::UpdateFlag, ot::FMCache::UpdateFlags)