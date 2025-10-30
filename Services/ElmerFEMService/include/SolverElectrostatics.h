// @otlicense
// File: SolverElectrostatics.h
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

#include "SolverBase.h"

class Application;

class SolverElectrostatics : public SolverBase {
public:
	SolverElectrostatics() {}
	virtual ~SolverElectrostatics() {}

	virtual void writeInputFile(std::ofstream& controlFile, Application *app) override;
	virtual std::string runSolver(const std::string& tempDirPath, ot::components::UiComponent* uiComponent) override;
	virtual void convertResults(const std::string& tempDirPath, Application* app, EntityBase* solverEntity) override;

private:
	void getMaterialsToObjectsMap(std::map<std::string, std::list<std::string>>& materialsToObjectsMap, Application *app);
	void getPotentialDefinitions(std::map<std::string, double>& potentialDefinitions);
	void buildMaterialAliases(std::map<std::string, std::list<std::string>>& materialsToObjectsMap, std::map<std::string, std::string>& materialNameToAliasMap);
	void buildPotentialAliases(std::map<std::string, double>& potentialDefinitions, std::map<std::string, std::string>& potentialNameToAliasMap);
	std::list<int> meshVolumeGroupIdList(const std::list<std::string>& itemNames);
	std::list<int> meshSurfaceGroupIdList(const std::list<std::string>& itemNames);
	std::string getGroupList(const std::list<int>& list);
	std::string getGroupList(const std::list<std::string>& list);
	void writeGroups(std::ofstream& controlFile, std::map<std::string, std::list<std::string>>& materialsToObjectsMap, std::map<std::string, std::string>& materialNameToAliasMap,
					 std::map<std::string, double>& potentialDefinitions, std::map<std::string, std::string>& potentialNameToAliasMap);
	void writeFunctions(std::ofstream& controlFile, std::map<std::string, std::string>& materialNameToAliasMap);
	void writeConstraints(std::ofstream& controlFile, std::map<std::string, double>& potentialDefinitions, std::map<std::string, std::string>& potentialNameToAliasMap);
	void writeFunctionSpace(std::ofstream& controlFile);
	void writeJacobian(std::ofstream& controlFile);
	void writeIntegration(std::ofstream& controlFile);
	void writeFormulation(std::ofstream& controlFile);
	void writeResolution(std::ofstream& controlFile);
	void writePostProcessing(std::ofstream& controlFile);
	void writePostOperation(std::ofstream& controlFile);
	void convertPotential(const std::string& tempDirPath, Application* app, EntityBase* solverEntity, long long& globalVisualizationMeshID, long long& globalVisualizationMeshVersion);
	void convertGlobalPotential(const std::string& tempDirPath, std::map<std::string, std::string> &nodeToPotentialMap, Application* app, EntityBase* solverEntity, long long& globalVisualizationMeshID, long long& globalVisualizationMeshVersion);
	void convertSurfacePotentials(const std::string& tempDirPath, std::map<std::string, std::string> &nodeToPotentialMap, Application* app, EntityBase* solverEntity);
	void convertEfield(const std::string& tempDirPath, Application* app, EntityBase* solverEntity, long long& globalVisualizationMeshID, long long& globalVisualizationMeshVersion);
	size_t getOrAddNode(const std::string& node, const std::string& potential, std::map<std::string, size_t>& nodeToIndexMap, std::list<std::string>& nodeList, std::list<std::string>& potentialList, size_t& nodeIndex, std::map<std::string, std::string>& nodeToPotentialMap);
	size_t getOrAddCellNode(const std::string& node, std::map<std::string, size_t>& nodeToIndexMap, std::list<std::string>& nodeList, size_t& nodeIndex);
	void storeMesh(int numberNodes, int cellType, std::list<std::string>& nodeList, std::list<std::vector<size_t>>& cellList, size_t& cellListSize, Application* app, EntityBase* solverEntity, long long& visualizationMeshID, long long& visualizationMeshVersion);
	void storeMeshScalarData(size_t numberPoints, size_t numberCells, std::list<std::string>& potentialList, Application* app, EntityBase* solverEntity, long long& visualizationMeshDataID, long long& visualizationMeshDataVersion);
	void storeMeshVectorData(size_t numberPoints, size_t numberCells, std::list<double>& magnitudeList, std::list<std::string>& vectorList, Application* app, EntityBase* solverEntity, long long& visualizationMeshDataID, long long& visualizationMeshDataVersion);
};
