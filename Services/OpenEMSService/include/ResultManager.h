// @otlicense
// File: ResultManager.h
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

#include <list>
#include <string>
#include <map>

#include "OTServiceFoundation/BusinessLogicHandler.h"

#include "ParametricCombination.h"

#include "OTResultDataAccess/SerialisationInterfaces/DatasetDescription.h"
#include "OTResultDataAccess/SerialisationInterfaces/ParameterDescription.h"

class MetadataSeries;

class ResultManager : public BusinessLogicHandler
{
public:
	ResultManager(ot::components::ModelComponent* _modelComponent, const std::string &_tmpDirPath, const std::string &_resultBasePath);
	virtual ~ResultManager();

	void clear();
	void loadParameters();
	void convert1D(const std::string &resultName, const std::string &fileName, const std::string& quantityName, const std::string& quantityUnit, const std::string& xAxis, const std::string& xUnit, double xScale);
	void storeResults();

private:
	void addParameterDescriptions(ParametricCombination& currentRun, const std::string& xAxis, const std::string& xUnit, std::list<ot::Variable> parameterValuesXAxis, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions);
	void addCurveData(const std::string& resultName, const std::string& quantityName, const std::string& quantityUnit, std::vector<std::pair<double, std::complex<double>>>& curveData, bool& isComplex, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions, std::list<DatasetDescription>& allCurveDescriptions);
	void storeCurves(std::list<DatasetDescription>& allCurveDescriptions);
	bool load1Ddata(const std::string filePath, std::vector<std::pair<double, std::complex<double>>>& curveData, bool& isComplex, double xScale);
	std::string getPlotName(const std::string& resultName);

	std::string tmpDirPath;
	std::string resultBasePath;
	ParametricCombination parameters;
	std::list<DatasetDescription> allCurveDescriptions;
	std::map<std::string, std::string> plotContainers;
};
