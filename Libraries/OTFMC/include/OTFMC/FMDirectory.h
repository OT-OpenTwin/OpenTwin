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
#include "OTCore/FileInformation.h"
#include "OTFMC/FMIgnoreFile.h"
#include "OTFMC/FMCTypes.h"

// std header
#include <list>
#include <optional>
#include <filesystem>

namespace ot {

	class OT_FMC_API_EXPORT FMDirectory : public Serializable {
		OT_DECL_NOCOPY(FMDirectory)
		OT_DECL_DEFMOVE(FMDirectory)
	public:
		enum ScanFlag : uint64_t {
			None                  = 0 << 0,
			ScanFiles			  = 1 << 0,
			ScanDirectories       = 1 << 1,
			ScanChildDirectories  = 1 << 2,
			UseRelativePaths      = 1 << 3,
			WriteOutput           = 1 << 4
		};
		typedef Flags<ScanFlag> ScanFlags;

		enum GetFileMode {
			TopLevelOnly,
			Recursive
		};

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

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void setPath(const std::filesystem::path& _path) { m_path = _path; };
		const std::filesystem::path& getPath() const { return m_path; };

		void addChildDirectory(FMDirectory&& _childDirectory) { m_childDirectories.push_back(std::move(_childDirectory)); };
		FMDirectory* getChildDirectory(const std::string& _directoryName, GetFileMode _mode);
		const std::list<FMDirectory>& getChildDirectories() const { return m_childDirectories; };

		void addFile(const FileInformation& _fileInfo) { m_files.push_back(_fileInfo); };
		void addFile(FileInformation&& _fileInfo) { m_files.push_back(std::move(_fileInfo)); };
		std::optional<FileInformation> getFile(const std::string& _fileName, GetFileMode _mode) const;
		std::list<FileInformation>& getFiles() { return m_files; };
		const std::list<FileInformation>& getFiles() const { return m_files; };
		std::list<FileInformation> getAllFiles(GetFileMode _mode) const;
		
	private:
		std::filesystem::path m_path;
		std::list<FMDirectory> m_childDirectories;
		std::list<FileInformation> m_files;

	};

}

OT_ADD_FLAG_FUNCTIONS(ot::FMDirectory::ScanFlag, ot::FMDirectory::ScanFlags)