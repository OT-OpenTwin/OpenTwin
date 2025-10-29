// @otLicense
// @otLicense-end

#pragma once

#include "OTCore/TypeNames.h"
#include "EntityInformation.h"

// TINYXML2
#include "tinyxml2.h"

// FDTDConfig
#include "FDTDConfig.h"

// STD INCLUDES
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <map>
#include <vector>
#include <list>
#include <array>
#include <string>
#include <sstream>
#include <windows.h> 

class EntityBase;
class Application;

namespace ot {
	namespace components {
		class UiComponent;
	}
}

#include "EntityProperties.h"

class SolverBase {
public:
	SolverBase() : solverEntity(nullptr) {}
	virtual ~SolverBase() {}

	void setData(EntityBase* _solverEntity, const std::string &_meshDataName, std::map<ot::UID, ot::EntityInformation>& _meshItemInfo,
				 std::map<ot::UID, EntityProperties>& _entityProperties, std::map<std::string, size_t>& _groupNameToIdMap,
				 std::map<std::string, EntityProperties>& _materialProperties);

	virtual void writeInputFile(std::ofstream& controlFile, Application *app) = 0;
	virtual std::string runSolver(const std::string& tempDirPath, ot::components::UiComponent* uiComponent) = 0;
	//virtual void convertResults(const std::string& tempDirPath, Application* app, EntityBase *solverEntity) = 0;

	void runSolverExe(const std::string& inputFileName, const std::string& solvTarget, const std::string& postTarget, const std::string& workingDirectory, ot::components::UiComponent* uiComponent);

protected:
	bool runExecutableAndWaitForCompletion(std::string commandLine, std::string workingDirectory, ot::components::UiComponent* uiComponent);
	std::string readEnvironmentVariable(const std::string& variableName);
	bool isPECMaterial(const std::string& materialName);
	void ReadFromPipe(HANDLE g_hChildStd_OUT_Rd, ot::components::UiComponent* uiComponent);

	EntityBase* solverEntity;
	std::string meshDataName;
	std::map<ot::UID, ot::EntityInformation> meshItemInfo;
	std::map<ot::UID, EntityProperties> entityProperties;
	std::map<std::string, size_t> groupNameToIdMap;
	std::map<std::string, EntityProperties> materialProperties;

	std::stringstream solverOutput;
};
