// @otlicense
// File: ParametricResult1DManager.h
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

class Result1DManager;
class Application;
class RunIDContainer;
class Result1DData;
class ResultCollectionExtender;


#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "MetadataSeries.h"
#include "DatasetDescription.h"
#include "OTCore/DefensiveProgramming.h"

#include <string>
#include <list>
#include <map>
#include <vector>


class ParametricResult1DManager : public BusinessLogicHandler
{
public:
	ParametricResult1DManager(Application* app);
	ParametricResult1DManager() = delete;
	~ParametricResult1DManager();

	void clear();
	void extractData(Result1DManager& result1DManager);
	void storeDataInResultCollection();

protected:
	inline bool invariant() { return true; };

private:
	std::string determineRunIDLabel(std::list<int>& runIDList);
	
	std::list<DatasetDescription> extractDataDescriptionCurve(const std::string& _category, RunIDContainer* _runIDContainer, int _runID);
	bool extractDataDescriptionSParameter(const std::string& _category, RunIDContainer* _runIDContainer, int _runID, DatasetDescription &dataDescription);
	std::list<std::shared_ptr<ParameterDescription>> extractParameterDescription(const std::string& _category, RunIDContainer* _runIDContainer, int _runID);

	int determineNumberOfPorts(const std::string& category, std::map<std::string, Result1DData*>& categoryResults, std::vector<Result1DData*>& sources);

	void parseAxisLabel(const std::string& value, std::string& label, std::string& unit);

	std::list<DatasetDescription> m_allDataDescriptions;

	std::string m_resultFolderName;
	std::string m_seriesNameBase = "CST Imported Results";
	std::string m_parameterNameRunID = "Run ID";
	Application* m_application;
};
