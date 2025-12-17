// @otlicense
// File: FileSystem.h
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
#include "OTSystem/Flags.h"
#include "OTSystem/DateTime.h"
#include "OTSystem/SystemAPIExport.h"

// std header
#include <list>
#include <string>
#include <fstream>

namespace ot {

	class OT_SYS_API_EXPORT FileSystem {
		FileSystem() = delete;
	public:
		//! @brief Option flags used to change the behaviour of the FileSystem methods.
		enum FileSystemOption {
			NoOptions     = 0 << 0, //! @brief All options disabled.
			Recursive     = 1 << 0, //! @brief Search recursive.
			GenericFormat = 1 << 1  //! @brief Return paths in generic format (e.g. Root/Path even on windows).
		};
		typedef Flags<FileSystemOption> FileSystemOptions;

		//! @brief Reads the entire content of a file.
		//! @param _filePath File path.
		//! @param _outputContent Output content.
		//! @param _fileSize Output file size in bytes.
		//! @param _exceptionMask Exception mask for file stream.
		static bool readFile(const std::string& _filePath, std::string& _outputContent, uint64_t& _fileSize, std::ifstream::iostate _exceptionMask = std::ifstream::badbit);

		//! @brief Returns all subdirectories at the given path.
		//! @param _path Top level path to search at.
		//! @param _options Search and format options.
		//! @throw std::filesystem::filesystem_error in case of filesystem related error.
		static std::list<std::string> getDirectories(const std::string& _path, const FileSystemOptions& _options = FileSystemOption::NoOptions);

		//! @brief Returns the last modification time of the given file or directory.
		//! @param _path Path to the file or directory.
		//! @param _useLocalTime If true local time is used, otherwise UTC time is used.
		static DateTime getLastModifiedTime(const std::string& _path, bool _useLocalTime = false);

		//! @brief Returns the last access time of the given file or directory.
		//! @param _path Path to the file or directory.
		//! @param _useLocalTime If true local time is used, otherwise UTC time is used.
		static DateTime getLastAccessTime(const std::string& _path, bool _useLocalTime = false);

		//! @brief Returns all files at the given path.
		//! @param _path Top level path to search at.
		//! @param _extensions Whitelisted file extensions. All files will be returned if the list is empty.
		//! @param _options Search and format options.
		//! @throw std::filesystem::filesystem_error in case of filesystem related error.
		static std::list<std::string> getFiles(const std::string& _path, const std::list<std::string>& _extensions, const FileSystemOptions& _options = FileSystemOption::NoOptions);

		//! @brief Reads a file line by line.
		//! @param _filePath File path.
		//! @throw ot::FileOpenException in case the file could not be opened.
		//! @throw std::ios_base::failure in case of serious IO error.
		static std::list<std::string> readLines(const std::string& _filePath, std::ifstream::iostate _exceptionMask = std::ifstream::badbit);
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::FileSystem::FileSystemOption, ot::FileSystem::FileSystemOptions)