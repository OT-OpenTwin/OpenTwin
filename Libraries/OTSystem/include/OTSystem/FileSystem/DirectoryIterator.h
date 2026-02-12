// @otlicense
// File: DirectoryIterator.h
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

// std header
#include <stack>

namespace ot {

	class OT_SYS_API_EXPORT DirectoryIterator {
	public:
		enum BrowseModeFlag {
			None           = 0 << 0,
			Files          = 1 << 0, //! @brief Include files.
			Directories    = 1 << 1, //! @brief Include directories.
			Symlinks       = 1 << 2, //! @brief Include symbolic links.
			Subdirectories = 1 << 3  //! @brief Traverse subdirectories.
		};
		typedef Flags<BrowseModeFlag> BrowseMode;

		DirectoryIterator() = delete;
		DirectoryIterator(const std::filesystem::path& _path, const BrowseMode& _browseMode);
		DirectoryIterator(const DirectoryIterator&) = delete;
		DirectoryIterator(DirectoryIterator&&) noexcept = delete;
		virtual ~DirectoryIterator() = default;

		DirectoryIterator& operator=(const DirectoryIterator&) = delete;
		DirectoryIterator& operator=(DirectoryIterator&&) noexcept = delete;

		bool hasNext();
		FileInformation next();

	protected:
		virtual bool checkEntrySkipped(const std::filesystem::directory_entry& _entry) const { return false; };

	private:
		bool advance();

		BrowseMode m_mode;
		std::stack<std::filesystem::directory_iterator> m_stack;
		std::filesystem::directory_iterator m_end;
		std::filesystem::directory_entry m_current;
	
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::DirectoryIterator::BrowseModeFlag, ot::DirectoryIterator::BrowseMode)