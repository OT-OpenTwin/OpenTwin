// @otlicense
// File: FileManager.h
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
#include "OTCore/LogDispatcher.h"

class FileManager {
public:
	FileManager();
	~FileManager();

	void initialize();

	//! @brief Append log to the current file.
	//! If the limit of messages for a single file is reached the next file will be opened.
	void appendLog(const ot::LogMessage& _message);
	
	void flush();

private:
	void openNewFile(bool _append);
	bool initializeFirstFile();

	bool m_active; //! @brief Initially true. If false the file manager won't write log messages to a file anymore.

	std::string m_fileRootDir; //! @brief Directory where the log files will be written to.
	std::ofstream* m_stream; //! @brief Current file stream.

	size_t m_currentSize; //! @brief Current file size.
	size_t m_maxFileSize; //! @brief Maximum file size.

	size_t m_currentFile; //! @brief Current file number.
	size_t m_fileCountLimit; //! @brief Maximum number of log files.
};