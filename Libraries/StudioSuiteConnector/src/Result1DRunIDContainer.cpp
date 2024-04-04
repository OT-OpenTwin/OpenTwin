#include "StudioSuiteConnector/Result1DRunIDContainer.h"

#include <filesystem>
#include <sstream>
#include <cassert>

#include <qcryptographichash.h>

Result1DRunIDContainer::~Result1DRunIDContainer()
{
	for (auto item : fileNameToDataMap)
	{
		if (item.second != nullptr)
		{
			delete item.second;
		}
	}

	fileNameToDataMap.clear();
}

bool Result1DRunIDContainer::readData(const std::string& dataDir)
{
	bool success = true;

	// We recursively iterate through all files and read the data.
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(dataDir))
	{
		if (!dirEntry.is_directory())
		{
			// We have a data file
			Result1DFileData* data = new Result1DFileData;
			
			if (data->readData(dirEntry.path().string()))
			{
				assert(fileNameToDataMap.count(dirEntry.path().string()) == 0);
				fileNameToDataMap[dirEntry.path().string()] = data;
			}
			else
			{
				assert(0);
				delete data;
				data = nullptr;
			}
		}
	}

	calculateMetaDataHash();
	calculateSize();

	return success;
}

size_t Result1DRunIDContainer::getNumberFiles()
{
	return fileNameToDataMap.size();
}

size_t Result1DRunIDContainer::getSize()
{
	return totalSize;
}

std::string Result1DRunIDContainer::getMetaDataHash()
{
	return metaHashValue;
}

void Result1DRunIDContainer::calculateSize()
{
	size_t containerSize = 0;

	containerSize += sizeof(size_t);           // Add the storage for the runId
	containerSize += sizeof(size_t);           // Add the storage for total number of files
	containerSize += sizeof(size_t);           // Add the storage for the meta data hash length
	containerSize += metaHashValue.size() + 1; // Add the storage for the meta data hash

	for (auto item : fileNameToDataMap)
	{
		assert(item.second != nullptr);

		containerSize += sizeof(size_t);						 // Add the storage for the file name length
		containerSize += item.first.size() + 1;					 // Add the storage for the file name
		containerSize += sizeof(size_t);					     // Add the storage for the data hash length
		containerSize += item.second->getDataHash().size() + 1;  // Add the storage for the data hash
		containerSize += sizeof(size_t);					     // Add the storage for the data length
		containerSize += item.second->getBufferSize();	         // Add the storage for the data itself (the buffer already includes the ending 0)
	}

	totalSize = containerSize;
}

void Result1DRunIDContainer::writeToBuffer(size_t runId, std::vector<char>& buffer)
{
	writeIntegerToBuffer(runId, buffer);
	writeIntegerToBuffer(getNumberFiles(), buffer);
	writeStringToBuffer(metaHashValue, buffer);

	for (auto item : fileNameToDataMap)
	{
		assert(item.second != nullptr);

		writeStringToBuffer(item.first, buffer);
		writeStringToBuffer(item.second->getDataHash(), buffer);

		writeIntegerToBuffer(item.second->getBufferSize() + 1, buffer);
		for (size_t index = 0; index < item.second->getBufferSize(); index++)
		{
			buffer.push_back(item.second->getBuffer()[index]);
		}
	}
}

void Result1DRunIDContainer::writeIntegerToBuffer(size_t value, std::vector<char>& buffer)
{
	char* valueBuffer = (char *)(&value);

	for (size_t index = 0; index < sizeof(size_t); index++)
	{
		buffer.push_back(valueBuffer[index]);
	}
}

void Result1DRunIDContainer::writeStringToBuffer(std::string value, std::vector<char>& buffer)
{
	writeIntegerToBuffer(value.size() + 1, buffer);

	for (size_t index = 0; index < value.size(); index++)
	{
		buffer.push_back(value[index]);
	}

	buffer.push_back(0);
}

void Result1DRunIDContainer::calculateMetaDataHash()
{
	// Get a total hash value for the meta data (including file names and their sizes
	// First we generate a string which contains all file names and their sizes
	// Afterward, we generate a hash for this string

	std::stringstream data;

	for (auto item : fileNameToDataMap)
	{
		data << item.first << ", " << item.second->getBufferSize() << std::endl;
	}

	// And finally calculate the hash
	QCryptographicHash hashCalculator(QCryptographicHash::Md5);
	hashCalculator.addData(data.str().c_str(), data.str().size());

	metaHashValue = hashCalculator.result().toHex().toStdString();
}
