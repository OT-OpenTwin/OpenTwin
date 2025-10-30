// @otlicense
// File: Result1DData.h
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
#include <sstream>
#include <vector>

class Result1DData
{
public:
	Result1DData();
	~Result1DData();

	void setDataHashValue(const std::string &dataHash);
	void setData(const std::string &data);

	std::string getDataHashValue() { return dataHashValue; }
	int getNumberOfQuantities();

	std::string getTitle() { return title; }
	std::string getXLabel() { return xlabel; }
	std::string getYLabel() { return ylabel; }
	std::vector<double>& getXValues() { return xValues; }
	std::vector<double>& getYreValues() { return yreValues; }
	std::vector<double>& getYimValues() { return yimValues; }

private:
	void readLine(std::stringstream& dataContent, std::string& line);
	void readDataLine(std::stringstream& dataContent, double& x, double& yre, double& yim, bool& xRead, bool& yreRead, bool& yimRead);

	std::string dataHashValue;
	std::string title;
	std::string xlabel;
	std::string ylabel;
	std::vector<double> xValues;
	std::vector<double> yreValues;
	std::vector<double> yimValues;
};
