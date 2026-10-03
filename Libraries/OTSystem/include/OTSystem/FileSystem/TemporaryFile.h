// @otlicense

#pragma once

// OpenTwin header
#include "OTSystem/SystemTypes.h"
#include "OTSystem/FileSystem/File.h"

// std header
#include <string>
#include <filesystem>

namespace ot
{

	//! @brief The TemporaryFile class is responsible for creating a temporary file.
	//! The file is created with the specified data and size, and it is automatically deleted when the TemporaryFile object is destroyed.
	class OT_SYS_API_EXPORT TemporaryFile
	{
	public:
		//! @brief Creates a temporary file with the given data and size at the specified file path.
		//! @param _filePath The path where the temporary file will be created. Must not be empty.
		//! @param _data Pointer to the data to be written to the temporary file. Must not be null.
		//! @param _dataSize The size of the data to be written to the temporary file. Must be greater than zero.
		//! @param _openMode The open mode for the file (default is binary and truncation).
		//! @param _uniqueName An optional unique name for the temporary file.
		//! @throws ot::InvalidArgumentException If any of the input parameters are invalid.
		//! @throws ot::FileOpenException If the file cannot be opened for writing.
		//! @throws ot::FileWriteException If the data cannot be written to the file.
		explicit TemporaryFile(const std::filesystem::path& _filePath, const char* _data, size_t _dataSize, const std::string& _uniqueName = std::string());

		TemporaryFile(TemporaryFile&& _other) noexcept;

		//! @brief Destructor that deletes the temporary file.
		~TemporaryFile();

		TemporaryFile& operator=(TemporaryFile&& _other) noexcept;

		inline bool isValid() const { return m_file.isValid(); };
		inline const std::filesystem::path& getFilePath() const { return m_file.getFilePath(); };
		const std::string& getUniqueName() const { return m_file.getUniqueName(); };

		bool deleteFile();

	private:
		File m_file;

		TemporaryFile() = delete;
		TemporaryFile(const TemporaryFile&) = delete;
		TemporaryFile& operator=(const TemporaryFile&) = delete;
	};

}