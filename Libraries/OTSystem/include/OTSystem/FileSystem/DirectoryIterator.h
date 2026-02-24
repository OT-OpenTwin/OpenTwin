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
#include <list>
#include <stack>

namespace ot {

	class OT_SYS_API_EXPORT DirectoryIterator {
	public:
		enum BrowseModeFlag {
			None            = 0 << 0,
			Files           = 1 << 0, //! @brief Include files.
			Directories     = 1 << 1, //! @brief Include directories.
			Symlinks        = 1 << 2, //! @brief Include symbolic links.
			
			Subdirectories  = 1 << 3, //! @brief Traverse subdirectories.
			GenericFormat   = 1 << 4, //! @brief Use generic format for paths (forward slashes as separator).

			AllFiles        = Files | Subdirectories,
			AllDirectories  = Directories | Subdirectories,
			All             = Files | Directories | Subdirectories,
			AllWithSymlinks = All | Symlinks
		};
		typedef Flags<BrowseModeFlag> BrowseMode;
		
		DirectoryIterator() = delete;
		DirectoryIterator(const std::filesystem::path& _path, const BrowseMode& _browseMode);
		DirectoryIterator(const DirectoryIterator&) = delete;
		DirectoryIterator(DirectoryIterator&&) noexcept = delete;
		virtual ~DirectoryIterator() = default;

		DirectoryIterator& operator=(const DirectoryIterator&) = delete;
		DirectoryIterator& operator=(DirectoryIterator&&) noexcept = delete;

		virtual DirectoryIterator* createNew() const;

		const std::filesystem::path& getRootPath() const { return m_rootPath; };
		BrowseMode getBrowseMode() const { return m_mode; };

		bool hasNext();
		FileInformation next();

		inline void reset();
		inline void reset(const std::filesystem::path& _path);
		void reset(const std::filesystem::path& _path, const BrowseMode& _browseMode);

		FileInformation findFile(const std::string& _fileName);
		static FileInformation findFile(const std::filesystem::path& _scanDirectoryPath, const std::string& _fileName, bool _topLevelOnly = true);

		bool hasFile(const std::string& _fileName);
		static bool hasFile(const std::filesystem::path& _scanDirectoryPath, const std::string& _fileName, bool _topLevelOnly = true);

		bool hasDirectory(const std::string& _directoryName);
		static bool hasDirectory(const std::filesystem::path& _scanDirectoryPath, const std::string& _directoryName, bool _topLevelOnly = true);

		std::list<FileInformation> getInfos();
		static std::list<FileInformation> getInfos(const std::filesystem::path& _scanDirectoryPath, const BrowseMode& _browseMode);

		std::list<std::filesystem::path> getPaths();
		static std::list<std::filesystem::path> getPaths(const std::filesystem::path& _scanDirectoryPath, const BrowseMode& _browseMode);

		std::list<std::filesystem::path> getRelativePaths();
		static std::list<std::filesystem::path> getRelativePaths(const std::filesystem::path& _scanDirectoryPath, const BrowseMode& _browseMode);

		inline std::list<FileInformation> getFiles();
		inline static std::list<FileInformation> getFiles(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly);

		inline std::list<std::filesystem::path> getFilePaths();
		inline static std::list<std::filesystem::path> getFilePaths(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly);
		
		std::list<std::filesystem::path> getRelativeFilePaths();
		static std::list<std::filesystem::path> getRelativeFilePaths(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly);

		inline std::list<FileInformation> getDirectories();
		inline static std::list<FileInformation> getDirectories(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly);

		inline std::list<std::filesystem::path> getDirectoryPaths();
		inline static std::list<std::filesystem::path> getDirectoryPaths(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly);

		std::list<std::filesystem::path> getRelativeDirectoryPaths();
		static std::list<std::filesystem::path> getRelativeDirectoryPaths(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly);

	protected:
		virtual bool checkEntrySkipped(const std::filesystem::directory_entry& _entry) const { return false; };

	private:
		bool advance();

		std::filesystem::path m_rootPath;
		BrowseMode m_mode;

		std::stack<std::filesystem::directory_iterator> m_stack;
		std::filesystem::directory_iterator m_end;
		std::filesystem::directory_entry m_current;
	
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::DirectoryIterator::BrowseModeFlag, ot::DirectoryIterator::BrowseMode)

#include "OTSystem/FileSystem/DirectoryIterator.hpp"