#include "RunIDContainer.h"
#include "Result1DData.h"
#include "InfoFileManager.h"

#include <stdexcept>

RunIDContainer::RunIDContainer()
{

}

RunIDContainer::~RunIDContainer()
{
	for (auto item : fileNameToData)
	{
		delete item.second;
	}

	fileNameToData.clear();
}

int RunIDContainer::readData(size_t &bufferIndex, char *dataBuffer, size_t dataBufferLength)
{
	size_t runId              = readIntegerFromBuffer(bufferIndex, dataBuffer, dataBufferLength);
	size_t numberOfFiles      = readIntegerFromBuffer(bufferIndex, dataBuffer, dataBufferLength);

	metaHashValue             = readStringFromBuffer(bufferIndex, dataBuffer, dataBufferLength);

	for (size_t fileIndex = 0; fileIndex < numberOfFiles; fileIndex++)
	{
		std::string fileName      = readStringFromBuffer(bufferIndex, dataBuffer, dataBufferLength);
		std::string dataHashValue = readStringFromBuffer(bufferIndex, dataBuffer, dataBufferLength);
		std::string data          = readStringFromBuffer(bufferIndex, dataBuffer, dataBufferLength);

		Result1DData* resultData = new Result1DData;
		resultData->setDataHashValue(dataHashValue);
		resultData->setData(data);

		fileNameToData[fileName] = resultData;
	}

	return (int) runId;
}

size_t RunIDContainer::readIntegerFromBuffer(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength)
{
	size_t value;
	char* valueBuffer = (char*)(&value);

	for (size_t index = 0; index < sizeof(size_t); index++)
	{
		if (bufferIndex == dataBufferLength) throw std::overflow_error("buffer overflow when reading data");

		valueBuffer[index] = dataBuffer[bufferIndex];
		bufferIndex++;
	}

	return value;
}

std::string RunIDContainer::readStringFromBuffer(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength)
{
	size_t stringLength = readIntegerFromBuffer(bufferIndex, dataBuffer, dataBufferLength);

	std::string value;
	value.reserve(stringLength);

	for (size_t index = 0; index < stringLength-1; index++)
	{
		if (bufferIndex == dataBufferLength) throw std::overflow_error("buffer overflow when reading data");
		
		value.push_back(dataBuffer[bufferIndex]);
		bufferIndex++;
	}

	// Now we read the string end character from the buffer
	if (bufferIndex == dataBufferLength) throw std::overflow_error("buffer overflow when reading data");
	if (dataBuffer[bufferIndex] != 0)
	{
		throw std::out_of_range("error when reading string end character");
	}
	bufferIndex++;

	return value;
}

void RunIDContainer::addResult1DInformation(int runID, InfoFileManager& infoFileManager)
{
	// First, add the meta hash information for this runId
	infoFileManager.setRunIDMetaHash(runID, metaHashValue);

	// Now add the information for each file
	for (auto file : fileNameToData)
	{
		infoFileManager.setRunIDFileHash(runID, file.first, file.second->getDataHashValue());
	}
}

