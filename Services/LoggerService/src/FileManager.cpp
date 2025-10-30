// @otlicense
// File: FileManager.cpp
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

// Service header
#include "FileManager.h"

// OpenTwin header
#include "OTSystem/FileSystem.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/String.h"

// std header
#include <fstream>
#include <filesystem>

FileManager::FileManager() : m_active(false), m_stream(nullptr), m_currentSize(0), m_maxFileSize(200000000), m_currentFile(0), m_fileCountLimit(3)
{
	
}

FileManager::~FileManager() {
	if (m_stream) {
		m_stream->close();
		delete m_stream;
		m_stream = nullptr;
	}
}

void FileManager::initialize() {
	if (m_active) {
		return;
	}

	// Get root dir from environment
	m_fileRootDir = ot::OperatingSystem::getEnvironmentVariableString("OPEN_TWIN_LOG_BUFFER_ROOT");
	if (m_fileRootDir.empty() || m_fileRootDir.find("//") == 0) {
		m_active = false;
		return;
	}

	// Optional: Get log buffer size from env
	std::string logBufferSizeEnv = ot::OperatingSystem::getEnvironmentVariableString("OPEN_TWIN_LOG_BUFFER_SIZE");
	if (!logBufferSizeEnv.empty()) {
		bool convFail = false;
		size_t size = ot::String::toNumber<size_t>(logBufferSizeEnv, convFail);
		if (!convFail) {
			m_maxFileSize = size;
		}
	}

	// Optional: Get file count from env
	std::string fileCountEnv = ot::OperatingSystem::getEnvironmentVariableString("OPEN_TWIN_LOG_BUFFER_FILECOUNT");
	if (!fileCountEnv.empty()) {
		bool convFail = false;
		size_t size = ot::String::toNumber<size_t>(fileCountEnv, convFail);
		if (!convFail && m_fileCountLimit > 1) {
			m_fileCountLimit = size;
		}
	}

	// Ensure directory exists
	try {
		if (!std::filesystem::is_directory(m_fileRootDir) &&
			!std::filesystem::create_directories(m_fileRootDir)) {
			m_active = false;
			return;
		}

		// Find initial file to start writing to
		if (!this->initializeFirstFile()) {
			return;
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
		return;
	}
}

void FileManager::appendLog(const ot::LogMessage& _message) {
	if (!m_active) {
		return;
	}
	if (!m_stream) {
		return;
	}

	// Write message to file
	const std::string msg = _message.toJson();
	(*m_stream) << msg << '\n';
	
	m_currentSize += (msg.size() + 1);

	// Check if limit reached
	if (m_currentSize >= m_maxFileSize) {
		m_currentFile = (m_currentFile + 1) % m_fileCountLimit;
		openNewFile(false);
	}
}

void FileManager::flush() {
	if (m_stream) {
		m_stream->flush();
	}
}

void FileManager::openNewFile(bool _append) {
	// Clean up old stream
	if (m_stream) {
		m_stream->close();
		delete m_stream;
		m_stream = nullptr;
	}

	// Build filename like: OpenTwinLog_001.otlogbuf
	std::ostringstream filename;
	filename << m_fileRootDir << "/OpenTwinLog_" << std::setw(std::to_string(m_fileCountLimit).length()) 
		<< std::setfill('0') << m_currentFile << ".otlogbuf";

	// Open new file (overwrite mode)
	const std::string fileNameStr = filename.str();
	m_stream = new std::ofstream(fileNameStr, (_append ? std::ios::app : std::ios::trunc));
	if (!m_stream->is_open()) {
		delete m_stream;
		m_stream = nullptr;
		// Disable logging if cannot open
		m_active = false;
		return;
	}

	if (_append) {
		m_currentSize = std::filesystem::file_size(fileNameStr);
	}
	else {
		m_currentSize = 0;
	}
}

bool FileManager::initializeFirstFile() {
	// Find last modified .otlogbuf file
	size_t lastFileIndex = 0;
	std::string lastFilePath;
	std::filesystem::file_time_type lastWriteTime{};
	bool found = false;

	for (auto& entry : std::filesystem::directory_iterator(m_fileRootDir)) {
		if (!entry.is_regular_file()) continue;
		if (entry.path().extension() != ".otlogbuf") continue;

		// Filename is like OpenTwinLog_001.otlogbuf
		std::string stem = entry.path().stem().string();
		if (stem.rfind("OpenTwinLog_", 0) == 0) {
			bool convFail = false;
			size_t index = ot::String::toNumber<size_t>(stem.substr(12), convFail);
			if (convFail) {
				return false;
			}

			auto ftime = std::filesystem::last_write_time(entry);

			if (!found || ftime > lastWriteTime) {
				lastWriteTime = ftime;
				lastFileIndex = index;
				lastFilePath = entry.path().string();
				found = true;
			}
		}
	}

	m_active = true;

	if (found && lastFileIndex < m_fileCountLimit) {
		// Check if file matches buffer size

		size_t fileSize = std::filesystem::file_size(lastFilePath);
		if (fileSize >= m_maxFileSize) {
			m_currentFile = (lastFileIndex + 1) % m_fileCountLimit;
			this->openNewFile(false);
		}
		else {
			m_currentFile = lastFileIndex;
			this->openNewFile(true);
		}
	}
	else {
		m_currentFile = 0;
		this->openNewFile(false);
	}

	return true;
}
