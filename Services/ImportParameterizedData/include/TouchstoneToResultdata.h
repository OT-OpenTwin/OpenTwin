// @otlicense
// File: TouchstoneToResultdata.h
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
#include "BusinessLogicHandler.h"
#include "ResultCollectionExtender.h"

#include "TouchstoneHandler.h"
#include "MetadataSeries.h"
#include "DatasetDescription.h"

class TouchstoneToResultdata : public BusinessLogicHandler
{
public:
	TouchstoneToResultdata() = default;
	TouchstoneToResultdata(const TouchstoneToResultdata& _other) = delete;
	TouchstoneToResultdata(TouchstoneToResultdata&& _other) = delete;
	TouchstoneToResultdata& operator=(const TouchstoneToResultdata& _other) = delete;
	TouchstoneToResultdata& operator=(TouchstoneToResultdata&& _other) = delete;
	~TouchstoneToResultdata() = default;

	int getAssumptionOfPortNumber(const std::string& fileName);
	void setResultdata(const std::string& fileName, const std::string& fileContent, uint64_t uncompressedLength);
	void createResultdata(int numberOfPorts);

private:
	std::string m_collectionName = "";
	std::string m_fileName = "";
	std::string m_fileContent = "";
	uint64_t m_uncompressedLength = 0;

	const std::string createSeriesName(const std::string& fileName);
	bool seriesAlreadyExists(const std::string& seriesName);

	void importTouchstoneFile(const std::string& fileName, const std::string& fileContent, uint64_t uncompressedLength, int numberOfPorts, TouchstoneHandler& _handler);
	DatasetDescription extractDatasetDescription(TouchstoneHandler& _handler);
};

