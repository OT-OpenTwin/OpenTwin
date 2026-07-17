// @otlicense
// File: FDTDSolver.h
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
#include <vector>
#include <list>

class Application;

class EntityBase;
class EntityMeshCartesian;
class EntityUnits;
class ResultManager;

class FDTDSolver
{
public:
	FDTDSolver(Application *_application, EntityBase* _solverEntity, EntityMeshCartesian* _meshEntity, const std::string &_openEMSPath, const std::string& _tempDirPath);
	virtual ~FDTDSolver() {};

	std::string generateRunCommand();
	void convertAndStoreResults(const std::string &logFileText);

private:
	void addPreparationData(std::stringstream& runCommand);
	void addSolverSetup(std::stringstream& runCommand);
	void addBoundaries(std::stringstream& runCommand);
	void addSolverRun(std::stringstream& runCommand);
	void addPostprocessing(std::stringstream& runCommand);
	void addUnits(std::stringstream& runCommand);
	std::string escapeBackslashes(const std::string& input);
	void convertAndStoreFrequencyDomainDump(const std::string& resultName, const std::string& fieldType, const std::string& unit);
	void convertAndStoreTimeDomainDump(const std::string& resultName, const std::string& fieldType, const std::string& unit);
	void convertAndStoreSingleFrequencyDomainDump(const std::string& absFileName, const std::string& argFileName, const std::string& fieldType, const std::string& unit);
	void convertAndStoreSingleTimeDomainDump(std::list<std::string>& resultFileList, const std::string& resultName, const std::string& fieldType, const std::string& unit);
	std::string toLower(std::string s);
	std::vector<char> readFile(const std::string& filename);
	std::string parseComplexResultFileName(const std::string& input);
	bool toDouble(const std::string& s, double& value);
	std::string doubleToString(double value);
	double extractRangeMax(const std::vector<char>& data);
	void convert1DTimeSignal(const std::string& resultName, const std::string& fileName, const std::string& quantityName, ResultManager& result1D, int readDataColumnOnly = -1);
	void convert1DFrequencySpectrum(const std::string& resultName, const std::string& fileName, const std::string& quantityName, ResultManager& result1D);
	double readTimeStepWidthFromLogText(const std::string& logFileText);

	Application* application;
	EntityBase *solverEntity;
	EntityMeshCartesian *meshEntity;
	std::string openEMSPath;
	std::string tempDirPath;
	double timeStepWidth;

	EntityUnits* entityUnits;
};
