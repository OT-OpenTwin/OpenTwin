// @otlicense

// OpenTwin header
#include "OTSystem/FileSystem/File.h"

// std header
#include <fstream>

ot::File::File(File&& _other) noexcept
	: m_filePath(std::move(_other.m_filePath)), m_uniqueName(std::move(_other.m_uniqueName)),
	m_rawData(std::move(_other.m_rawData)), m_rawDataSize(_other.m_rawDataSize)
{
	_other.m_filePath.clear();
	_other.m_uniqueName.clear();

	_other.m_rawData.reset();
	_other.m_rawDataSize = 0;
}

ot::File& ot::File::operator=(File&& _other) noexcept
{
	if (this != &_other)
	{
		m_filePath = std::move(_other.m_filePath);
		m_uniqueName = std::move(_other.m_uniqueName);

		m_rawData = std::move(_other.m_rawData);
		m_rawDataSize = _other.m_rawDataSize;

		_other.m_filePath.clear();
		_other.m_uniqueName.clear();

		_other.m_rawData.reset();
		_other.m_rawDataSize = 0;
	}
	return *this;
}

bool ot::File::deleteFile()
{
	if (!isValid())
	{
		OTAssert(0, "Invalid file path");
		throw Exception::InvalidArgument("Invalid file path");
	}

	std::error_code ec;
	bool result = std::filesystem::remove(m_filePath, ec);
	if (ec)
	{
		OTAssert(0, "Failed to delete file");
		throw Exception::General("Failed to delete file: \"" + m_filePath.string() + "\"");
	}
	return result;
}

bool ot::File::readFile()
{
	if (!isValid())
	{
		OTAssert(0, "Invalid file path");
		throw Exception::InvalidArgument("Invalid file path");
	}

	std::ifstream file(m_filePath, std::ios::binary);
	if (!file.is_open())
	{
		throw Exception::FileOpen("Failed to open file for reading: \"" + m_filePath.string() + "\"");
	}
	file.seekg(0, std::ios::end);
	m_rawDataSize = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios::beg);
	
	m_rawData = std::make_unique<uint8_t[]>(m_rawDataSize);
	file.read(reinterpret_cast<char*>(m_rawData.get()), m_rawDataSize);
	
	if (!file)
	{
		throw Exception::FileRead("Failed to read data from file: \"" + m_filePath.string() + "\"");
	}

	return true;
}

ot::File ot::File::readFile(const std::filesystem::path& _filePath, const std::string& _uniqueName)
{
	File newFile(_filePath, _uniqueName);
	if (!newFile.readFile())
	{
		OTAssert(0, "Failed to read file");
	}
	return newFile;
}

bool ot::File::writeFile()
{
	if (!isValid())
	{
		OTAssert(0, "Invalid file path");
		throw Exception::InvalidArgument("Invalid file path");
	}

	std::ofstream file(m_filePath, std::ios::binary | std::ios::trunc);
	if (!file.is_open())
	{
		throw Exception::FileOpen("Failed to open file for writing: \"" + m_filePath.string() + "\"");
	}

	file.write(reinterpret_cast<const char*>(m_rawData.get()), m_rawDataSize);
	
	if (!file)
	{
		throw Exception::FileWrite("Failed to write data to file: \"" + m_filePath.string() + "\"");
	}

	return true;
}

bool ot::File::writeFile(const std::filesystem::path& _filePath, const uint8_t* _data, size_t _dataSize)
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

	std::ofstream file(_filePath, std::ios::binary | std::ios::trunc);
	if (!file.is_open())
	{
		throw Exception::FileOpen("Failed to open file for writing: \"" + _filePath.string() + "\"");
	}
	file.write(reinterpret_cast<const char*>(_data), _dataSize);
	
	if (!file)
	{
		throw Exception::FileWrite("Failed to write data to file: \"" + _filePath.string() + "\"");
	}
	return true;
}
