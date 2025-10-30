// @otlicense
// File: FixtureTouchstoneHandler.h
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
#include <gtest/gtest.h>

#include "TouchstoneHandler.h"
#include "QuantityDescriptionSParameter.h"

class FixtureTouchstoneHandler : public testing::Test
{
public:
	FixtureTouchstoneHandler()
		:m_handler("testFile.s2p")
	{

	}
	const ts::OptionSettings& analyseOptionSettings(std::string& text) 
	{ 
		m_handler.analyseLine(text);
		return m_handler.getOptionSettings(); 
	}
	
	void analyseLine(std::string& text)
	{
		m_handler.analyseLine(text);
	}
	
	void analyseFile(const std::string& fileContent)
	{
		m_handler.analyseFile(fileContent,m_handler.m_portNumber);
	}

	const std::string& getComments()
	{
		return m_handler.getComments();
	}

	void cleansLineOfComments(std::string& line)
	{
		m_handler.cleansOfComments(line);
	}

	void setNumberOfPorts(uint32_t portNumber)
	{
		m_handler.m_portNumber = portNumber;
	}

	const QuantityDescriptionSParameter& getQuantityDescriptionData()
	{
		return m_handler.getQuantityDescription();
	}

	const ts::OptionSettings& getOptionSettings()
	{
		return m_handler.getOptionSettings();
	}

	const MetadataParameter& getFrequencyParameter()
	{
		return m_handler.getMetadataFrequencyParameter();
	}

	void setup1PortTest()
	{
		std::string line = "2.000 0.894 -12.136\n";
		setNumberOfPorts(1);
		analyseLine(line);
	}

	const std::string& getFullExampleFourPorts() const { return m_exampleFourPorts; };
private:
	TouchstoneHandler m_handler;

	const std::string m_exampleFourPorts =
		"!4 - port S - parameter data, taken at three frequency points\n"
		"!note that data points need not be aligned\n"
		"# Hz Y RI R 90\n"
		"5.00000 0.60 161.24 0.40 -42.20 0.42 -66.58 0.53 -79.34 !row 1\n"
		"0.40 -42.20 0.60 161.20 0.53 -79.34 0.42 -66.58 !row 2\n"
		"0.42 -66.58 0.53 -79.34 0.60 161.24 0.40 -42.20 !row 3\n"
		"0.53 -79.34 0.42 -66.58 0.40 -42.20 0.60 161.24 !row 4\n"
		"6.00000 0.57 150.37 0.40 -44.34 0.41 -81.24 0.57 -95.77 !row 1\n"
		"0.40 -44.34 0.57 150.37 0.57 -95.77 0.41 -81.24 !row 2\n"
		"0.41 -81.24 0.57 -95.77 0.57 150.37 0.40 -44.34 !row 3\n"
		"0.57 -95.77 0.41 -81.24 0.40 -44.34 0.57 150.37 !row 4\n"
		"7.00000 0.50 136.69 0.45 -46.41 0.37 -99.09 0.62 -114.19 !row 1\n"
		"0.45 -46.41 0.50 136.69 0.62 -114.19 0.37 -99.09 !row 2\n"
		"0.37 -99.09 0.62 -114.19 0.50 136.69 0.45 -46.41 !row 3\n"
		"0.62 -114.19 0.37 -99.09 0.45 -46.41 0.50 136.69 !row 4\n";
};
