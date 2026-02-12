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
	: m_mode(_browseMode), m_end()
{
	std::filesystem::directory_iterator it = std::filesystem::directory_iterator(_path);
	if (it != m_end) {
		m_stack.push(std::move(it));
	}
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
