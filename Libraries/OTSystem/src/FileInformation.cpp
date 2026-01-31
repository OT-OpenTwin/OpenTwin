// @otlicense
// File: FileInformation.cpp
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
#include "OTSystem/FileInformation.h"

ot::FileInformation::FileInformation(const std::filesystem::path& _path) 
	: m_type(FileType::Unknown)
{
	std::filesystem::path pathCopy = _path;
	this->operator=(std::move(pathCopy));
}

ot::FileInformation::FileInformation(std::filesystem::path&& _path) 
	: m_type(FileType::Unknown)
{
	this->operator=(std::move(_path));
}

ot::FileInformation::FileInformation(const std::filesystem::directory_entry& _entry) 
	: m_type(FileType::Unknown)
{
	this->operator=(_entry);
}

void ot::FileInformation::operator=(const std::filesystem::path& _path) {
	std::filesystem::path pathCopy = _path;
	this->operator=(std::move(pathCopy));
}

void ot::FileInformation::operator=(std::filesystem::path&& _path) {
	m_path = std::move(_path);

	if (std::filesystem::exists(m_path)) {
		if (std::filesystem::is_regular_file(m_path)) {
			m_type = FileType::File;
		}
		else if (std::filesystem::is_directory(m_path)) {
			m_type = FileType::Directory;
		}
		else if (std::filesystem::is_symlink(m_path)) {
			m_type = FileType::Symlink;
		}
		else {
			m_type = FileType::Unknown;
		}
	}
	else {
		m_type = FileType::Unknown;
	}
}

void ot::FileInformation::operator=(const std::filesystem::directory_entry& _entry) {
	m_path = _entry.path();

	if (_entry.is_regular_file()) {
		m_type = FileType::File;
	}
	else if (_entry.is_directory()) {
		m_type = FileType::Directory;
	}
	else if (_entry.is_symlink()) {
		m_type = FileType::Symlink;
	}
	else {
		m_type = FileType::Unknown;
	}
}

bool ot::FileInformation::exists() const {
	return std::filesystem::exists(m_path);
}
