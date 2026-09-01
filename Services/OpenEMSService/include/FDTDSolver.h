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
#include <map>
#include <set>

class Application;

class EntityBase;
class EntityMeshCartesian;
class EntityUnits;
class EntityFacetData;
class ResultManager;
class EntityWaveguidePort;
class EntityFieldDump;
class EntityVis2D3D;

#include "OTModelEntities/Geometry.h"

class FDTDSolver
{
public:
	FDTDSolver(Application *_application, EntityBase* _solverEntity, EntityMeshCartesian* _meshEntity, const std::string &_openEMSPath, const std::string& _tempDirPath);
	virtual ~FDTDSolver();

	std::string generateRunCommand();
	void convertAndStoreResults(const std::string &logFileText);

private:
	void checkCartesianMesh(std::stringstream& runCommand);
	void addPreparationData(std::stringstream& runCommand);
	void addSolverSetup(std::stringstream& runCommand);
	void addMesh(std::stringstream& runCommand);
	void addGeometry(std::stringstream& runCommand);
	void addSolverRun(std::stringstream& runCommand);
	void addPostprocessing(std::stringstream& runCommand);
	void addUnits(std::stringstream& runCommand);
	void addPorts(std::stringstream& runCommand);
	void addFieldDumps(std::stringstream& runCommand);
	std::string escapeBackslashes(const std::string& input);
	void convertAndStoreFrequencyDomainDump(const std::string& resultFolder, const std::string& resultName, const std::string& fieldType, const std::string& postfix, const std::string& unit, EntityFieldDump* fieldDump);
	void convertAndStoreTimeDomainDump(const std::string& resultFolder, const std::string& resultName, const std::string& fieldType, const std::string& postfix, const std::string& unit, EntityFieldDump* fieldDump);
	void convertAndStoreSingleFrequencyDomainDump(const std::string& absFileName, const std::string& argFileName, const std::string& fieldType, const std::string& postfix, const std::string& unit, EntityFieldDump* fieldDump);
	void convertAndStoreSingleTimeDomainDump(std::list<std::string>& resultFileList, const std::string& resultName, const std::string& fieldType, const std::string& postfix, const std::string& unit, EntityFieldDump* fieldDump);
	std::string toLower(std::string s);
	std::vector<char> readFile(const std::string& filename);
	std::string parseComplexResultFileName(const std::string& input);
	bool toDouble(const std::string& s, double& value);
	std::string doubleToString(double value);
	double extractRangeMax(const std::vector<char>& data);
	void convert1DTimeSignal(const std::string& resultName, const std::string& fileName, const std::string& quantityName, ResultManager& result1D, int readDataColumnOnly = -1);
	void convert1DFrequencySpectrum(const std::string& resultName, const std::string& fileName, const std::string& quantityName, ResultManager& result1D);
	double readTimeStepWidthFromLogText(const std::string& logFileText);
	bool storeSTLGeometry(EntityFacetData* facetData, const std::string& stlFileName);
	std::map<std::string, int> createIntegerPriorities(const std::map<std::string, double>& shapeNameToPriorityMap, double tolerance = 1e-5);
	void addBackgroundMaterial(std::stringstream& runCommand);
	void readMeshLineInformation();
	void writeLinesArray(const std::string& direction, const std::vector<double>& linesArray, std::stringstream& runCommand);
	void readExcitation();
	void readFieldDumps();
	std::list<std::map<int, double>> parseExcitations(std::string_view input);
	void readPorts();
	bool parsePortNumber(const std::string& name, int& portNumber);
	void findPortRange(double position, const std::vector<double>& gridLines, const std::string& minBoundary, const std::string& maxBoundary, double nx, double& from, double& to);
	void convertAndStoreSParameters(ResultManager& result1D);
	void convertAndStoreFieldDumps(const std::string& resultFolderName, const std::string& excitationString);
	bool getFieldDumpTypeAndMode(EntityFieldDump* fieldDump, int& dumpType, int& dumpMode);
	bool getFieldTypeAndUnit(EntityFieldDump* fieldDump, std::string &fieldType, std::string &unit);
	std::string getFrequencyString(EntityFieldDump* fieldDump);
	std::string getSubsamplingString(EntityFieldDump* fieldDump);
	std::string getResolutionString(EntityFieldDump* fieldDump);
	std::string getFieldDumpName(EntityFieldDump* fieldDump);
	std::string getStartStopString(EntityFieldDump* fieldDump);
	bool isFrequencyDump(EntityFieldDump* fieldDump);
	std::size_t mergeCloseNodes(std::vector<Geometry::Node>& nodes, double tolerance);
	bool getGridDimensions(const std::vector<char>& data, int& nx, int& ny, int& nz);
	void fixPlanePLocation(int gridNx, int gridNy, int gridNz, EntityVis2D3D* visualizationEntity, EntityFieldDump* fieldDump);

	Application* application;
	EntityBase *solverEntity;
	EntityMeshCartesian *meshEntity;
	std::string openEMSPath;
	std::string tempDirPath;
	double timeStepWidth;

	std::vector<double> xLines, yLines, zLines;
	std::list<std::map<int, double>> excitationList;
	std::list<EntityWaveguidePort*> waveguidePortList;
	std::list<EntityFieldDump*> fieldDumpList;
	std::set<int> portList;

	std::string xminBoundary;
	std::string xmaxBoundary;
	std::string yminBoundary;
	std::string ymaxBoundary;
	std::string zminBoundary;
	std::string zmaxBoundary;

	EntityUnits* entityUnits;
};
