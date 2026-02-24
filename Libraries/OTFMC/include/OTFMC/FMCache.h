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
#include "OTSystem/FileSystem/FileInformation.h"
#include "OTFMC/FMCTypes.h"
#include "OTModelEntities/NewModelStateInfo.h"

namespace ot {

	class OT_FMC_API_EXPORT FMCache : public Serializable {
		OT_DECL_NOCOPY(FMCache)
		OT_DECL_DEFMOVE(FMCache)
	public:
		struct StateInfo {
			std::list<FileInformation> newFiles;
			std::list<FileInformation> deletedFiles;
			std::list<FileInformation> modifiedFiles;
		};

		FMCache() = default;
		~FMCache() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void clear();

	private:
		static const int c_currentCacheVersion = 1;
		
		struct FileEntry {
			FileInformation fileInfo;
		};

	};
}
