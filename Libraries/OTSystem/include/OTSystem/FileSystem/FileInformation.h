// @otlicense
// File: FileInformation.h
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
#include "OTSystem/SystemTypes.h"
#include "OTSystem/SystemAPIExport.h"

// std header
#include <filesystem>

namespace ot {

	class OT_SYS_API_EXPORT FileInformation {
	public:
		enum FileType {
			Unknown,
			File,
			Directory,
			Symlink
		};

		FileInformation();
		FileInformation(const FileInformation&) = default;
		FileInformation(FileInformation&&) noexcept = default;
		FileInformation(const std::filesystem::path& _path);
		FileInformation(std::filesystem::path&& _path);
		FileInformation(const std::filesystem::directory_entry& _entry);

		FileInformation& operator=(const FileInformation&) = default;
		FileInformation& operator=(FileInformation&&) noexcept = default;

		void operator=(const std::filesystem::path& _path);
		void operator=(std::filesystem::path&& _path);
		void operator=(const std::filesystem::directory_entry& _entry);

		bool isValid() const { return m_type != FileType::Unknown && !m_path.empty(); };
		bool exists() const;
		
		FileType getType() const { return m_type; };
		bool isFile() const { return m_type == FileType::File; };
		bool isDirectory() const { return m_type == FileType::Directory; };
		bool isSymlink() const { return m_type == FileType::Symlink; };

		std::filesystem::path getPath() const { return m_path; };

	private:
		FileType m_type;
		std::filesystem::path m_path;
	};
}