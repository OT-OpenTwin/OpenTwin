// @otlicense
// File: FMProjectPath.h
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

// std header
#include <filesystem>

namespace ot {

	class FMProjectPath {
		OT_DECL_DEFCOPY(FMProjectPath)
		OT_DECL_DEFMOVE(FMProjectPath)
	public:
		explicit FMProjectPath() = default;
		explicit FMProjectPath(const std::filesystem::path& _path) : m_path(_path) {};
		explicit FMProjectPath(std::filesystem::path&& _path) : m_path(std::move(_path)) {};

		void setPath(const std::filesystem::path& _path) { m_path = _path; };
		const std::filesystem::path& getPath() const { return m_path; };

		bool isEmpty() const { return m_path.empty(); };

		static std::string getIgnoreFileName() { return ".otignore"; };
		static std::string getCacheFolderName() { return ".otcache"; };
		static std::string getCacheInfoFileName() { return ".otcache.info.json"; };

		std::filesystem::path getIgnoreFilePath() const { return m_path / getIgnoreFileName(); };
		std::filesystem::path getCacheFolderPath() const { return m_path / getCacheFolderName(); };
		std::filesystem::path getCacheInfoFilePath() const { return getCacheFolderPath() / getCacheInfoFileName(); };

	private:
		std::filesystem::path m_path;
	};

}