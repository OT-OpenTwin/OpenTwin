// @otlicense
// File: DirectoryIterator.hpp
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
#include "OTSystem/ArchitectureInfo.h"
#include "OTSystem/FileSystem/DirectoryIterator.h"

// std header
#include <type_traits>

namespace ot {

	namespace intern {

		template <typename T> class ValueRAIIDepr {
		public:
			ValueRAIIDepr(T& _value, T _set) : ValueRAIIDepr(_value, _set, _value) {};
			ValueRAIIDepr(T& _value, T _set, T _reset) : m_value(_value = _set), m_reset(_reset) {}
			~ValueRAIIDepr() {
				m_value = m_reset;
			}

		private:
			T& m_value;
			T m_reset;
		};

	}
}

inline void ot::DirectoryIterator::reset() {
	reset(m_rootPath, m_mode);
}

inline void ot::DirectoryIterator::reset(const std::filesystem::path& _path) {
	reset(_path, m_mode);
}

inline std::list<ot::FileInformation> ot::DirectoryIterator::getFiles() {
	intern::ValueRAIIDepr<BrowseMode> mode(m_mode, m_mode.has(Subdirectories) ? AllFiles : Files);
	return getInfos();
}

inline std::list<ot::FileInformation> ot::DirectoryIterator::getFiles(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly) {
	return getInfos(_scanDirectoryPath, _topLevelOnly ? Files : AllFiles);
}

inline std::list<std::filesystem::path> ot::DirectoryIterator::getFilePaths() {
	intern::ValueRAIIDepr<BrowseMode> mode(m_mode, m_mode.has(Subdirectories) ? AllFiles : Files);
	return getPaths();
}

inline std::list<std::filesystem::path> ot::DirectoryIterator::getFilePaths(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly) {
	return getPaths(_scanDirectoryPath, _topLevelOnly ? Files : AllFiles);
}

inline std::list<std::filesystem::path> ot::DirectoryIterator::getRelativeFilePaths() {
	intern::ValueRAIIDepr<BrowseMode> mode(m_mode, m_mode.has(Subdirectories) ? AllFiles : Files);
	return getRelativePaths();
}

inline std::list<std::filesystem::path> ot::DirectoryIterator::getRelativeFilePaths(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly) {
	return getRelativePaths(_scanDirectoryPath, _topLevelOnly ? Files : AllFiles);
}

inline std::list<ot::FileInformation> ot::DirectoryIterator::getDirectories() {
	intern::ValueRAIIDepr<BrowseMode> mode(m_mode, m_mode.has(Subdirectories) ? AllFiles : Files);
	return getInfos();
}

inline std::list<ot::FileInformation> ot::DirectoryIterator::getDirectories(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly) {
	return getInfos(_scanDirectoryPath, _topLevelOnly ? Directories : AllDirectories);
}

inline std::list<std::filesystem::path> ot::DirectoryIterator::getDirectoryPaths() {
	intern::ValueRAIIDepr<BrowseMode> mode(m_mode, m_mode.has(Subdirectories) ? AllDirectories : Directories);
	return getPaths();
}

inline std::list<std::filesystem::path> ot::DirectoryIterator::getDirectoryPaths(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly) {
	return getPaths(_scanDirectoryPath, _topLevelOnly ? Directories : AllDirectories);
}

inline std::list<std::filesystem::path> ot::DirectoryIterator::getRelativeDirectoryPaths() {
	intern::ValueRAIIDepr<BrowseMode> mode(m_mode, m_mode.has(Subdirectories) ? AllDirectories : Directories);
	return getRelativePaths();
}

inline std::list<std::filesystem::path> ot::DirectoryIterator::getRelativeDirectoryPaths(const std::filesystem::path& _scanDirectoryPath, bool _topLevelOnly) {
	return getRelativePaths(_scanDirectoryPath, _topLevelOnly ? Directories : AllDirectories);
}
