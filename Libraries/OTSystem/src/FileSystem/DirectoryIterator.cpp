// @otlicense
// File: DirectoryIterator.cpp
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
#include "OTSystem/Exception.h"
#include "OTSystem/FileSystem/DirectoryIterator.h"

ot::DirectoryIterator::DirectoryIterator(const std::filesystem::path& _path, const BrowseMode& _browseMode)
	: m_rootPath(_path), m_mode(_browseMode), m_end(), m_current()
{
	std::filesystem::directory_iterator it = std::filesystem::directory_iterator(m_rootPath);
	if (it != m_end) {
		m_stack.push(std::move(it));
	}
}

ot::DirectoryIterator* ot::DirectoryIterator::createNew() const {
	return new DirectoryIterator(m_rootPath, m_mode);
}

bool ot::DirectoryIterator::hasNext() {
	return advance();
}

ot::FileInformation ot::DirectoryIterator::next() {
	if (!advance()) {
		throw Exception::OutOfBounds("DirectoryIterator::next: no more entries");
	}

	FileInformation result(m_current);
	m_current = std::filesystem::directory_entry{}; // invalidate cache
	return result;
}

void ot::DirectoryIterator::reset(const std::filesystem::path& _path, const BrowseMode& _browseMode) {
	m_rootPath = _path;
	m_mode = _browseMode;

	m_stack = std::stack<std::filesystem::directory_iterator>();
	m_current = std::filesystem::directory_entry{};
	
	std::filesystem::directory_iterator it = std::filesystem::directory_iterator(m_rootPath);
	if (it != m_end) {
		m_stack.push(std::move(it));
	}
}

ot::FileInformation ot::DirectoryIterator::findFile(const std::string& _fileName) {
	intern::ValueRAIIDepr<BrowseMode> mode(m_mode, m_mode.has(Subdirectories) ? AllFiles : Files);

	std::unique_ptr<DirectoryIterator> it(createNew());
	if (!it) {
		throw Exception::NullPointer("Failed to create DirectoryIterator instance");
	}

	try {
		while (it->hasNext()) {
			const auto entry = it->next();
			if (entry.isFile() && entry.getPath().filename() == _fileName) {
				return FileInformation(entry.getPath());
			}
		}
	}
	catch (const std::exception& _e) {
		//! @todo Log exception
		OTAssert(0, "Log and not throw...");
	}

	return FileInformation();
}

ot::FileInformation ot::DirectoryIterator::findFile(const std::filesystem::path& _scanDirectoryPath, const std::string& _fileName, bool _topLevelOnly) {
	DirectoryIterator it(_scanDirectoryPath, _topLevelOnly ? DirectoryIterator::Files : DirectoryIterator::AllFiles);
	return it.findFile(_fileName);
}

bool ot::DirectoryIterator::hasFile(const std::string& _fileName) {
	intern::ValueRAIIDepr<BrowseMode> mode(m_mode, m_mode.has(Subdirectories) ? AllFiles : Files);
	
	std::unique_ptr<DirectoryIterator> it(createNew());
	if (!it) {
		throw Exception::NullPointer("Failed to create DirectoryIterator instance");
	}

	try {
		while (it->hasNext()) {
			const auto entry = it->next();
			if (entry.isFile() && entry.getPath().filename() == _fileName) {
				return true;
			}
		}
	}
	catch (const std::exception& _e) {
		//! @todo Log exception
		OTAssert(0, "Log and not throw...");
	}

	return false;
}

bool ot::DirectoryIterator::hasFile(const std::filesystem::path& _scanDirectoryPath, const std::string& _fileName, bool _topLevelOnly) {
	DirectoryIterator it(_scanDirectoryPath, _topLevelOnly ? DirectoryIterator::Files : DirectoryIterator::AllFiles);
	return it.hasFile(_fileName);
}

bool ot::DirectoryIterator::hasDirectory(const std::string& _directoryName) {
	intern::ValueRAIIDepr<BrowseMode> mode(m_mode, m_mode.has(Subdirectories) ? AllDirectories : Directories);
	
	std::unique_ptr<DirectoryIterator> it(createNew());
	if (!it) {
		throw Exception::NullPointer("Failed to create DirectoryIterator instance");
	}
	try {
		while (it->hasNext()) {
			const auto entry = it->next();
			if (entry.isDirectory() && entry.getPath().filename() == _directoryName) {
				return true;
			}
		}
	}
	catch (const std::exception& _e) {
		//! @todo Log exception
		OTAssert(0, "Log and not throw...");
	}
	return false;
}

bool ot::DirectoryIterator::hasDirectory(const std::filesystem::path& _scanDirectoryPath, const std::string& _directoryName, bool _topLevelOnly) {
	DirectoryIterator it(_scanDirectoryPath, _topLevelOnly ? DirectoryIterator::Directories : DirectoryIterator::AllDirectories);
	return it.hasDirectory(_directoryName);
}

std::list<ot::FileInformation> ot::DirectoryIterator::getInfos() {
	OTAssert(!m_rootPath.empty(), "Scan directory path must not be empty");

	std::list<FileInformation> result;

	std::unique_ptr<DirectoryIterator> it(createNew());
	if (!it) {
		throw Exception::NullPointer("Failed to create DirectoryIterator instance");
	}

	try {
		while (it->hasNext()) {
			result.push_back(it->next());
		}
	}
	catch (const std::exception& _e) {
		//! @todo Log exception
		OTAssert(0, "Log and not throw...");
	}

	return result;
}

std::list<ot::FileInformation> ot::DirectoryIterator::getInfos(const std::filesystem::path& _scanDirectoryPath, const BrowseMode& _browseMode) {
	DirectoryIterator it(_scanDirectoryPath, _browseMode);
	return it.getInfos();
}

std::list<std::filesystem::path> ot::DirectoryIterator::getPaths() {
	OTAssert(!m_rootPath.empty(), "Scan directory path must not be empty");

	std::list<std::filesystem::path> result;

	std::unique_ptr<DirectoryIterator> it(createNew());
	if (!it) {
		throw Exception::NullPointer("Failed to create DirectoryIterator instance");
	}

	try {
		while (it->hasNext()) {
			result.push_back(it->next().getPath());
		}
	}
	catch (const std::exception& _e) {
		//! @todo Log exception
		OTAssert(0, "Log and not throw...");
	}

	return result;
}

std::list<std::filesystem::path> ot::DirectoryIterator::getPaths(const std::filesystem::path& _scanDirectoryPath, const BrowseMode& _browseMode) {
	DirectoryIterator it(_scanDirectoryPath, _browseMode);
	return it.getPaths();
}

std::list<std::filesystem::path> ot::DirectoryIterator::getRelativePaths() {
	OTAssert(!m_rootPath.empty(), "Scan directory path must not be empty");

	std::list<std::filesystem::path> result;

	std::unique_ptr<DirectoryIterator> it(createNew());
	if (!it) {
		throw Exception::NullPointer("Failed to create DirectoryIterator instance");
	}

	try {
		while (it->hasNext()) {
			const auto entry = it->next();
			const auto& path = entry.getPath();

			result.push_back(path.lexically_relative(m_rootPath));
		}
	}
	catch (const std::exception& _e) {
		//! @todo Log exception
		OTAssert(0, "Log and not throw...");
	}

	return result;
}

std::list<std::filesystem::path> ot::DirectoryIterator::getRelativePaths(const std::filesystem::path& _scanDirectoryPath, const BrowseMode& _browseMode) {
	DirectoryIterator it(_scanDirectoryPath, _browseMode);
	return it.getRelativePaths();
}

bool ot::DirectoryIterator::advance() {
	while (true) {
		// Check if we have a current valid entry
		if (!m_current.path().empty()) {
			return true;
		}

		// If the stack is empty, we are done
		if (m_stack.empty()) {
			return false;
		}

		auto& currentIt = m_stack.top();
		if (currentIt == m_end) {
			m_stack.pop();
			continue;
		}

		std::filesystem::directory_entry entry = *currentIt;
		currentIt++;

		if (checkEntrySkipped(entry)) {
			continue;
		}

		const bool isDir = entry.is_directory();
		const bool isFile = entry.is_regular_file();
		const bool isSym = entry.is_symlink();

		if (isDir && m_mode.has(Subdirectories)) {
			std::filesystem::directory_iterator sub(entry.path(), std::filesystem::directory_options::skip_permission_denied);
			if (sub != m_end) {
				m_stack.push(std::move(sub));
			}
		}

		if ((isFile && m_mode.has(Files)) ||
			(isDir && m_mode.has(Directories)) ||
			(isSym && m_mode.has(Symlinks))) 
		{
			m_current = std::move(entry);
			return true;
		}
	}
}
