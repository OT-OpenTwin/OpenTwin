// @otlicense

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