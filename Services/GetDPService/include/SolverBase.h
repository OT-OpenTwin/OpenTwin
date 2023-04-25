#pragma once

#include "OpenTwinCore/TypeNames.h"
#include "OpenTwinFoundation/EntityInformation.h"

#include <fstream>
#include <map>
#include <list>
#include <string>

class EntityBase;

#include "EntityProperties.h"

class SolverBase {
public:
	SolverBase() : solverEntity(nullptr) {}
	virtual ~SolverBase() {}

	void setData(EntityBase* _solverEntity, const std::string &_meshDataName, std::map<ot::UID, ot::EntityInformation>& _meshItemInfo,
				 std::map<ot::UID, EntityProperties>& _entityProperties, std::map<std::string, size_t>& _groupNameToIdMap,
				 std::map<std::string, EntityProperties>& _materialProperties);

	virtual void writeInputFile(std::ofstream& controlFile) = 0;
	virtual void runSolver(const std::string& tempDirPath) = 0;
	virtual void convertResults(const std::string& tempDirPath) = 0;

	void runSolverExe(const std::string& inputFileName, const std::string& solvTarget, const std::string& postTarget, const std::string& workingDirectory);

protected:
	bool runExecutableAndWaitForCompletion(std::string commandLine, std::string workingDirectory);
	std::string readEnvironmentVariable(const std::string& variableName);
	bool isPECMaterial(const std::string& materialName);

	EntityBase* solverEntity;
	std::string meshDataName;
	std::map<ot::UID, ot::EntityInformation> meshItemInfo;
	std::map<ot::UID, EntityProperties> entityProperties;
	std::map<std::string, size_t> groupNameToIdMap;
	std::map<std::string, EntityProperties> materialProperties;
};
