// @otlicense
// File: LogNotifierFileWriter.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTCore/LogNotifierFileWriter.h"

// std header
#include <fstream>
#include <filesystem>

std::string ot::LogNotifierFileWriter::generateFileName(const std::string& _serviceName) {
	std::string baseName = _serviceName + "_log";
	std::string extension = ".txt";
	std::string fileName;
	int counter = 0;

	do {
		fileName = baseName;
		if (counter > 0) {
			fileName += "_" + std::to_string(counter);
		}
		fileName += extension;
		counter++;
	} while (std::filesystem::exists(fileName));

	return fileName;
}

ot::LogNotifierFileWriter::LogNotifierFileWriter(const std::string& _filePath) {
	OTAssert(!_filePath.empty(), "File path is empty");

	m_stream = new std::ofstream(_filePath);
	if (!m_stream->is_open()) {
		delete m_stream;
		m_stream = nullptr;

		OT_LOG_EAS("Unable to create file \"" + _filePath + "\"");
	}
}

ot::LogNotifierFileWriter::~LogNotifierFileWriter() {
	if (m_stream) {
		m_stream->close();
		delete m_stream;
		m_stream = nullptr;
	}
}

void ot::LogNotifierFileWriter::log(const LogMessage& _message) {
	if (m_stream) {
		std::lock_guard<std::mutex> lock(m_mutex);
		*m_stream << _message << std::endl;
	}
}

void ot::LogNotifierFileWriter::flushAndCloseStream(void) {
	if (m_stream) {
		m_stream->flush();
		this->closeStream();
	}
}

void ot::LogNotifierFileWriter::closeStream(void) {
	if (m_stream) {
		m_stream->close();
		delete m_stream;
		m_stream = nullptr;
	}
}
