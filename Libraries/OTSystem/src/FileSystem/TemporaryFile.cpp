// @otlicense

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTSystem/FileSystem/TemporaryFile.h"

// std header
#include <fstream>

ot::TemporaryFile::TemporaryFile(const std::filesystem::path& _filePath, const char* _data, size_t _dataSize, const std::string& _uniqueName)
	: m_file()
{
	if (_filePath.empty())
	{
		OTAssert(0, "File path cannot be empty");
		throw Exception::InvalidArgument("File path cannot be empty");
	}
	if (_data == nullptr)
	{
		OTAssert(0, "Data pointer cannot be null");
		throw Exception::InvalidArgument("Data pointer cannot be null");
	}
	if (_dataSize == 0)
	{
		OTAssert(0, "Data size must be greater than zero");
		throw Exception::InvalidArgument("Data size must be greater than zero");
	}

	if (!File::writeFile(_filePath, reinterpret_cast<const uint8_t*>(_data), _dataSize))
	{
		throw Exception::FileWrite("Failed to write data to temporary file: \"" + _filePath.string() + "\"");
	}

	m_file = File(_filePath, _uniqueName);
}

ot::TemporaryFile::TemporaryFile(TemporaryFile&& _other) noexcept
	: m_file(std::move(_other.m_file))
{}

ot::TemporaryFile::~TemporaryFile()
{
	deleteFile();
}

ot::TemporaryFile& ot::TemporaryFile::operator=(TemporaryFile&& _other) noexcept
{
	if (this != &_other)
	{
		m_file = std::move(_other.m_file);
	}
	return *this;
}

bool ot::TemporaryFile::deleteFile()
{
	bool result = false;

	if (!m_file.isValid())
	{
		try
		{
			result = m_file.deleteFile();
			m_file = File();
		}
		catch (const std::exception& _e)
		{
			result = false;
			OTAssert(0, "Failed to delete temporary file");
			// Log error ...
		}
		catch (...)
		{
			result = false;
			OTAssert(0, "[FATAL] Failed to delete temporary file");
			// Log error ...
		}
	}

	return result;
}
