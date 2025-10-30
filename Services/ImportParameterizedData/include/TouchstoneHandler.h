// @otlicense
// File: TouchstoneHandler.h
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
#include <string>
#include <list>
#include "OptionSettings.h"
#include "QuantityDescriptionSParameter.h"
#include "DatasetDescription.h"
#include "MetadataParameter.h"
#include "OTCore/GenericDataStructMatrix.h"

class TouchstoneHandler
{
public:
	TouchstoneHandler(const std::string& _fileName);
	TouchstoneHandler(const TouchstoneHandler& _other) = delete;
	TouchstoneHandler(TouchstoneHandler&& _other) noexcept = default;
	TouchstoneHandler& operator=(const TouchstoneHandler& _other) = delete;
	TouchstoneHandler& operator=(TouchstoneHandler&& _other) noexcept = default;

	static int32_t deriveNumberOfPorts(const std::string& _fileName);
	
	void analyseFile(const std::string& _fileContent, int32_t _numberOfPorts);
	const ts::OptionSettings& getOptionSettings() const { return m_optionSettings; }
	const std::string& getComments() const { return m_comments; }
	QuantityDescriptionSParameter* handOverQuantityDescription() 
	{
		QuantityDescriptionSParameter* temp = m_quantityDescription;
		m_quantityDescription = nullptr;
		return temp;
	}
	MetadataParameter& getMetadataFrequencyParameter() { return m_frequencyParameter; }

private:
	friend class FixtureTouchstoneHandler;

	std::string m_comments = "";
	ts::OptionSettings m_optionSettings;
	
	uint32_t m_touchstoneVersion = 1;
	uint32_t m_portNumber = 0;
	
	QuantityDescriptionSParameter* m_quantityDescription = nullptr;
	MetadataParameter m_frequencyParameter;

	ot::MatrixEntryPointer m_matrixEntry;
	
	ot::GenericDataStructMatrix* m_firstValues = nullptr;
	ot::GenericDataStructMatrix* m_secondValues = nullptr;

	bool m_firstValueOfTuple = true;

	void analyseLine(std::string& _content);
	void analyseDataLine(std::string& _content);
	void analyseOptionsLine(std::string& _line);
	void analyseVersionTwoLine(std::string& _content);

	void cleansOfComments(std::string& _content);
	void cleansOfSpecialCharacter(std::string& _content);

	//! @brief Currently all segments are turned into double values.
	ot::Variable turnLineSegmentToVariable(const std::string& _segment);
};

