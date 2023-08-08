#pragma once

#include "SolverBase.h"

class SolverElectrostatics : public SolverBase {
public:
	SolverElectrostatics() {}
	virtual ~SolverElectrostatics() {}

	virtual void writeInputFile(std::ofstream& controlFile) override;
	virtual void runSolver(const std::string& tempDirPath) override;
	virtual void convertResults(const std::string& tempDirPath) override;

private:
	void getMaterialsToObjectsMap(std::map<std::string, std::list<std::string>>& materialsToObjectsMap);
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
};
