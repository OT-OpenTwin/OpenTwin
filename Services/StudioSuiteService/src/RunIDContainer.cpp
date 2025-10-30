// @otlicense
// File: RunIDContainer.cpp
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

#include "RunIDContainer.h"
#include "Result1DData.h"
#include "InfoFileManager.h"

#include <stdexcept>
#include <cassert>
#include <algorithm>
#include <set>

RunIDContainer::RunIDContainer()
{

}

RunIDContainer::~RunIDContainer()
{
	clear();
}

void RunIDContainer::clear()
{
	metaHashValue.clear();

	for (auto item : fileNameToData)
	{
		delete item.second;
	}

	fileNameToData.clear();
	parameterValues.clear();
}

int RunIDContainer::readData(size_t &bufferIndex, char *dataBuffer, size_t dataBufferLength)
{
	clear();

	size_t runId              = readIntegerFromBuffer(bufferIndex, dataBuffer, dataBufferLength);
	size_t numberOfFiles      = readIntegerFromBuffer(bufferIndex, dataBuffer, dataBufferLength);

	metaHashValue             = readStringFromBuffer(bufferIndex, dataBuffer, dataBufferLength);

	for (size_t fileIndex = 0; fileIndex < numberOfFiles; fileIndex++)
	{
		std::string fileName      = readStringFromBuffer(bufferIndex, dataBuffer, dataBufferLength);
		std::string dataHashValue = readStringFromBuffer(bufferIndex, dataBuffer, dataBufferLength);
		std::string data          = readStringFromBuffer(bufferIndex, dataBuffer, dataBufferLength);

		std::replace(fileName.begin(), fileName.end(), '\\', '/');

		Result1DData* resultData = new Result1DData;
		resultData->setDataHashValue(dataHashValue);

		if (fileName == "parameters.info")
		{
			processParameterInformation(data);
		}
		else
		{
			resultData->setData(data);
		}

		if (resultData->getXValues().empty()) 
		{
			// We skip all data which is not x/y type of data, e.g. data contains only one point value
			delete resultData;
			resultData = nullptr;
		}
		else
		{
			fileNameToData[fileName] = resultData;
		}
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

void RunIDContainer::processParameterInformation(const std::string& data)
{
	std::stringstream dataContent(data);

	while (!dataContent.eof())
	{
		std::string name, value;

		readLine(dataContent, name);
		readLine(dataContent, value);

		if (!name.empty() && !value.empty())
		{
			double numericValue = atof(value.c_str());

			assert(parameterValues.count(name) == 0);
			parameterValues[name] = numericValue;
		}
	}
}

void RunIDContainer::readLine(std::stringstream& dataContent, std::string& line)
{
	std::getline(dataContent, line);

	if (!line.empty())
	{
		if (line[line.size() - 1] == '\r')
		{
			line.pop_back();
		}
	}
}

std::list<std::string> RunIDContainer::getListOfCategories()
{
	std::set<std::string> categorySet;

	for (auto file : fileNameToData)
	{
		// extract category from file name
		size_t pos = file.first.find_last_of('/');
		std::string category = (pos != std::string::npos) ? file.first.substr(0, pos) : file.first;

		if (category.substr(0, 11) == "1D Results/")
		{
			categorySet.emplace(category);
		}
	}

	// convert the map to a list
	std::list<std::string> categories(categorySet.begin(), categorySet.end());

	return categories;
}


std::map<std::string, Result1DData*> RunIDContainer::getResultsForCategory(const std::string& category)
{
	std::map<std::string, Result1DData*> results;

	std::string filter = category + "/";

	for (auto file : fileNameToData)
	{
		if (file.first.substr(0, filter.size()) == filter)
		{
			results[file.first] = file.second;
		}
	}

	return results;
}
