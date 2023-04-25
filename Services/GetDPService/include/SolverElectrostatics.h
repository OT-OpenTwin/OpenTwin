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
	std::list<int> meshGroupIdList(const std::list<std::string>& itemNames);
	std::string getGroupList(const std::list<int>& list);
	std::string getGroupList(const std::list<std::string>& list);
	void writeGroups(std::ofstream& controlFile, std::map<std::string, std::list<std::string>>& materialsToObjectsMap, std::map<std::string, std::string>& materialNameToAliasMap);
	void writeFunctions(std::ofstream& controlFile, std::map<std::string, std::string>& materialNameToAliasMap);
	void writeConstraints(std::ofstream& controlFile);
	void writeFunctionSpace(std::ofstream& controlFile);
	void writeJacobian(std::ofstream& controlFile);
	void writeIntegration(std::ofstream& controlFile);
	void writeFormulation(std::ofstream& controlFile);
	void writeResolution(std::ofstream& controlFile);
	void writePostProcessing(std::ofstream& controlFile);
	void writePostOperation(std::ofstream& controlFile);
};
