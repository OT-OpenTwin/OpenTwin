// @otlicense
// File: AdvancedDirectoryIterator.h
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
#include "OTSystem/FileSystem/DirectoryIterator.h"

namespace ot {

	class OT_SYS_API_EXPORT AdvancedDirectoryIterator : public DirectoryIterator {
	public:
		AdvancedDirectoryIterator(const std::filesystem::path& _path, const BrowseMode& _browseMode, const IgnoreRules& _ignoreRules);
		AdvancedDirectoryIterator(const std::filesystem::path& _path, const BrowseMode& _browseMode, IgnoreRules&& _ignoreRules);
		AdvancedDirectoryIterator(const AdvancedDirectoryIterator&) = delete;
		AdvancedDirectoryIterator(AdvancedDirectoryIterator&&) noexcept = delete;
		virtual ~AdvancedDirectoryIterator() = default;

		AdvancedDirectoryIterator& operator=(const AdvancedDirectoryIterator&) = delete;
		AdvancedDirectoryIterator& operator=(AdvancedDirectoryIterator&&) noexcept = delete;

		virtual DirectoryIterator* createNew() const override;

	protected:
		virtual bool checkEntrySkipped(const std::filesystem::directory_entry& _entry) const override;

	private:
		IgnoreRules m_ignoreRules;

	};

}
