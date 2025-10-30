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

#include "OTServiceFoundation/BusinessLogicHandler.h"

#include "ParametricCombination.h"

#include "DatasetDescription.h"
#include "ParameterDescription.h"

namespace ltspice
{
	class RawData;
}

class ResultManager : public BusinessLogicHandler
{
public:
	ResultManager(ot::components::ModelComponent* modelComponent);
	virtual ~ResultManager();

	void extractResults(const std::string& ltSpicefileNameBase);

private:
	void processParametricResults(ltspice::RawData& resultData, std::list<ParametricCombination>& parameterRuns, std::list<DatasetDescription>& allCurveDescriptions);
	void getParametricCombinations(const std::string& logFileName, std::list<ParametricCombination>& parameterRuns);
	void addParameterDescriptions(ParametricCombination& currentRun, ltspice::RawData& resultData, std::list<ot::Variable> parameterValuesXAxis, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions);
	void addCurveData(ltspice::RawData& resultData, std::list<std::shared_ptr<ParameterDescription>>& allParameterDescriptions, std::list<DatasetDescription>& allCurveDescriptions, size_t numberOfXValues, size_t indexOffset);
	void storeCurves(std::list<DatasetDescription>& allCurveDescriptions);
	void clear();
	std::string getUnitFromType(const std::string& type);
};
