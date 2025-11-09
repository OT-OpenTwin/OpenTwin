// @otlicense
// File: FMDirectory.h
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
#include "OTCore/CoreTypes.h"
#include "OTCore/FileInformation.h"
#include "OTFMC/FMIgnoreFile.h"
#include "OTFMC/FMConnectorAPIExport.h"

// std header
#include <list>
#include <filesystem>

namespace ot {

	class FMDirectory {
		OT_DECL_NOCOPY(FMDirectory)
		OT_DECL_DEFMOVE(FMDirectory)
	public:
		enum ScanFlag : uint64_t {
			None        = 0 << 0,
			ScanChilds  = 1 << 0,
			WriteOutput = 1 << 1
		};
		typedef Flags<ScanFlag> ScanFlags;

		//! @brief Creates an FMDirectory object from the file system.
		//! @param _path The path to the directory to scan.
		//! @param _scanChilds If true, all child directories will be scanned as well, otherwise only the files and the top level child directories are scanned.
		static FMDirectory fromFileSystem(const std::string& _path, const FMIgnoreFile& _ignoreData, const ScanFlags& _flags) { return fromFileSystem(std::filesystem::path(_path), _ignoreData, _flags); };

		//! @brief Creates an FMDirectory object from the file system.
		//! @param _path The path to the directory to scan.
		//! @param _scanChilds If true, all child directories will be scanned as well, otherwise only the files and the top level child directories are scanned.
		static FMDirectory fromFileSystem(const std::filesystem::path& _path, const FMIgnoreFile& _ignoreData, const ScanFlags& _flags);

		FMDirectory() = default;
		~FMDirectory() = default;

		const std::filesystem::path& getPath() const { return m_path; };

		const std::list<FMDirectory>& getChildDirectories() const { return m_childDirectories; };
		const std::list<FileInformation>& getFiles() const { return m_files; };

	private:
		std::filesystem::path m_path;
		std::list<FMDirectory> m_childDirectories;
		std::list<FileInformation> m_files;

	};

}

OT_ADD_FLAG_FUNCTIONS(ot::FMDirectory::ScanFlag, ot::FMDirectory::ScanFlags)