// @otlicense
// File: AdvancedDirectoryIterator.cpp
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

ot::AdvancedDirectoryIterator::AdvancedDirectoryIterator(const std::filesystem::path& _path, const BrowseMode& _browseMode, const IgnoreRules& _ignoreRules)
	: DirectoryIterator(_path, _browseMode), m_ignoreRules(_ignoreRules)
{}

ot::AdvancedDirectoryIterator::AdvancedDirectoryIterator(const std::filesystem::path& _path, const BrowseMode& _browseMode, IgnoreRules&& _ignoreRules)
	: DirectoryIterator(_path, _browseMode), m_ignoreRules(std::move(_ignoreRules))
{}

ot::DirectoryIterator* ot::AdvancedDirectoryIterator::createNew() const {
	return new AdvancedDirectoryIterator(getRootPath(), getBrowseMode(), m_ignoreRules);
}

bool ot::AdvancedDirectoryIterator::checkEntrySkipped(const std::filesystem::directory_entry& _entry) const {
	const bool isDir = _entry.is_directory();

    if (m_ignoreRules.matches(_entry.path(), isDir)) {
        return true;
    }
	else {
		return false;
	}
}
