//! @file FileManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "FileManager.h"

// OpenTwin header
#include "OTSystem/FileSystem.h"
#include "OTSystem/OperatingSystem.h"

// std header
#include <fstream>
#include <filesystem>

FileManager::FileManager() : m_active(false), m_stream(nullptr), m_messageCount(0), m_messageLimit(10000), m_currentFile(0), m_fileCountLimit(10)
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

	// Ensure directory exists
	try {
		if (!std::filesystem::is_directory(m_fileRootDir) &&
			!std::filesystem::create_directories(m_fileRootDir)) {
			m_active = false;
			return;
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
		return;
	}

	m_active = true;

	// Open first file
	this->openNewFile();
}

void FileManager::appendLog(const ot::LogMessage& _message) {
	if (!m_active) {
		return;
	}
	if (!m_stream) {
		return;
	}

	// Write message to file
	(*m_stream) << _message.toJson() << std::endl;
	
	// Check if limit reached
	if (++m_messageCount >= m_messageLimit) {
		m_currentFile = (m_currentFile + 1) % m_fileCountLimit;
		openNewFile();
	}
}

void FileManager::openNewFile() {
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
	m_stream = new std::ofstream(filename.str(), std::ios::trunc);
	if (!m_stream->is_open()) {
		delete m_stream;
		m_stream = nullptr;
		// Disable logging if cannot open
		m_active = false;
		return;
	}

	m_messageCount = 0;
}
