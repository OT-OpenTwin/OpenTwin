
#include "GetDPLauncher.h"

#include "DataBase.h"
#include "Application.h"
#include "SolverBase.h"
#include "SolverElectrostatics.h"

#include "EntityMeshTet.h"
#include "EntityMeshTetData.h"
#include "EntityBinaryData.h"
#include "ClassFactory.h"
#include "EntityProperties.h"

#include "Types.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include <windows.h> // winapi

#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>


GetDPLauncher::GetDPLauncher(Application *app)
	: application(app)
{

}

GetDPLauncher::~GetDPLauncher()
{

}

std::string GetDPLauncher::startSolver(std::string &logFileText, const std::string &dataBaseURL, const std::string &uiUrl, const std::string &projectName, EntityBase* solverEntity, int serviceID, int sessionCount, ot::components::ModelComponent *modelComponent)
{
	std::string outputText;
	logFileText.clear();

	// Find the name of the temporary directory where we run the solver and create the directory
	std::string tempDirPath = getUniqueTempDir();
	if (tempDirPath.empty()) return "ERROR: Unable to create temporary working directory (TMP environment variable needs to be set)";

	if (_mkdir(tempDirPath.c_str()) == -1)
	{
		return "ERROR: Unable to create temporary working directory (TMP environment variable needs to be set)";
	}

	// Extract the mesh into the temp folder
	ot::UID meshDataID = 0;
	std::string meshDataName;
	std::string meshFileName = extractMesh(solverEntity, tempDirPath, meshDataID, meshDataName);
	if (meshFileName.empty()) return "ERROR: Unable to read the specified tetrahedral mesh (ensure to specify a valid mesh)";

	// Get all mesh entities of the current mesh
	std::map<ot::UID, ot::EntityInformation> meshItemInfo;
	readMeshItemInfo(meshDataID, meshItemInfo);

	// Get the properties for all entities of the current mesh
	std::map<ot::UID, EntityProperties> entityProperties;
	application->modelComponent()->getEntityProperties(meshDataID, true, "Solver", entityProperties);

	// Get all the material property infomation
	std::map<std::string, EntityProperties> materialProperties;
	readMaterialProperties(materialProperties);

	// Create a solver depending on the type
	std::string problemType = getProblemType(solverEntity);

	SolverBase * solver = nullptr;

	if (problemType == "Electrostatics")
	{
		solver = new SolverElectrostatics();
	}
	else
	{
		// Unknown solver type
		return "ERROR: Unknown problem type: " + problemType;
	}

	std::map<std::string, size_t> groupNameToIdMap;
	readGroupsFromMesh(meshFileName, groupNameToIdMap);

	application->uiComponent()->setProgressInformation("Solver running: " + problemType, true);

	try
	{
		// Build the solver input file in the temp folder
		std::string controlFileName = tempDirPath + "\\model.pro";
		std::ofstream controlFile(controlFileName);
		
		solver->setData(solverEntity, meshDataName, meshItemInfo, entityProperties, groupNameToIdMap, materialProperties);
		solver->writeInputFile(controlFile);
		controlFile.close();

		// Run the solver
		logFileText = solver->runSolver(tempDirPath, application->uiComponent());

		// Convert the results
		solver->convertResults(tempDirPath, application, solverEntity);
	}
	catch (std::string& error)
	{
		outputText = "ERROR: " + error;
	}

	application->uiComponent()->closeProgressInformation();

	// Delete the solver object
	delete solver;
	solver = nullptr;

	// Remove the temp dir if requested
	EntityPropertiesBoolean* debug = dynamic_cast<EntityPropertiesBoolean*>(solverEntity->getProperties().getProperty("Debug"));
	assert(debug != nullptr);

	bool debugFlag = false;
	if (debug != nullptr) debugFlag = debug->getValue();

	if (debugFlag)
	{
		outputText += "\n\nWARNING: The working folder has not been deleted for debugging purposes: " + tempDirPath;
	}
	else
	{
		if (!deleteDirectory(tempDirPath)) return "ERROR: Unable to remove the temporary working directory: " + tempDirPath;
	}

	// return the result string
	return outputText;
}

void GetDPLauncher::readMeshItemInfo(ot::UID meshDataID, std::map<ot::UID, ot::EntityInformation>& meshItemInfo)
{
	std::list<ot::EntityInformation> info;
	application->modelComponent()->getEntityChildInformation(meshDataID, info, true);

	for (auto item : info)
	{
		meshItemInfo[item.getID()] = item;
	}
}

void GetDPLauncher::readMaterialProperties(std::map<std::string, EntityProperties>& materialProperties)
{
	std::map<ot::UID, EntityProperties> entityProperties;
	application->modelComponent()->getEntityProperties("Materials", true, "", entityProperties);

	ot::UIDList entityIDList;
	for (auto item : entityProperties)
	{
		entityIDList.push_back(item.first);
	}

	std::list<ot::EntityInformation> entityInformation;
	application->modelComponent()->getEntityInformation(entityIDList, entityInformation);

	for (auto &entity : entityInformation)
	{
		materialProperties[entity.getName()] = entityProperties[entity.getID()];
	}
}

void GetDPLauncher::readGroupsFromMesh(const std::string& meshFileName, std::map<std::string, size_t>& groupNameToIdMap)
{
	// Here we process the mesh file in between the lines $PhysicalNames and $EndPhysicalNames

	std::ifstream meshFile(meshFileName);
	
	bool processingActive = false;
	std::string line;

	while (getline(meshFile, line))
	{
		if (processingActive)
		{
			if (line == "$EndPhysicalNames")
			{
				return;
			}

			// Here we interpret the line defining the group: <groud dimension>, <group id>, <group name>
			std::stringstream lineStream(line);
			size_t dimension = 0;
			size_t groupid = 0;

			lineStream >> dimension;
			lineStream >> groupid;

			std::string groupName;
			getline(lineStream, groupName);

			groupName = groupName.substr(2, groupName.length() - 3);

			groupNameToIdMap[groupName] = groupid;
		}
		else
		{
			if (line == "$PhysicalNames")
			{
				getline(meshFile, line); // Skip the next line
				processingActive = true;
			}
		}
	}
}

std::string GetDPLauncher::getProblemType(EntityBase* solverEntity)
{
	EntityPropertiesSelection* solver = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Problem type"));
	assert(solver != nullptr);

	if (solver == nullptr) return "";

	return solver->getValue();
}

std::string GetDPLauncher::extractMesh(EntityBase* solverEntity, const std::string &tempDirPath, ot::UID &meshDataID, std::string &currentMeshDataName)
{
	// Get the name of the currently selected mesh
	EntityPropertiesEntityList* mesh = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Mesh"));
	assert(mesh != nullptr);
	if (mesh == nullptr) return "";

	std::string currentMeshName = mesh->getValueName();

	// Get the name of the mesh data
	currentMeshDataName = currentMeshName + "/Mesh";

	// Get the ID of the mesh data item
	std::list<string> entityList;
	std::list<ot::EntityInformation> entityInfo;

	entityList.push_back(currentMeshDataName);

	application->modelComponent()->getEntityInformation(entityList, entityInfo);
	if (entityInfo.empty()) return "";

	meshDataID = entityInfo.front().getID();

	// Get the gmsh data storage ID
	ot::UIDList entityIDList;
	entityIDList.push_back(meshDataID);
	application->prefetchDocumentsFromStorage(entityIDList);

	EntityMeshTetData* entity = dynamic_cast<EntityMeshTetData*> (application->modelComponent()->readEntityFromEntityIDandVersion(meshDataID, application->getPrefetchedEntityVersion(meshDataID), application->getClassFactory()));
	if (entity == nullptr) return "";

	ot::UID gmshDataStorageID = entity->getGmshDataStorageId();

	delete entity;
	entity = nullptr;

	// Now we load the gmsh data
	entityIDList.clear();
	entityIDList.push_back(gmshDataStorageID);
	application->prefetchDocumentsFromStorage(entityIDList);

	EntityBinaryData* entityData = dynamic_cast<EntityBinaryData*> (application->modelComponent()->readEntityFromEntityIDandVersion(gmshDataStorageID, application->getPrefetchedEntityVersion(gmshDataStorageID), application->getClassFactory()));
	if (entityData == nullptr) return "";

	std::vector<char> meshContent = entityData->getData();

	delete entityData;
	entityData = nullptr;

	// Finally write the mesh file to the tmp dir
	std::string meshFileName = tempDirPath + "\\model.msh";

	std::ofstream file(meshFileName, std::ios::binary);
	file.write(meshContent.data(), meshContent.size());
	file.close();

	return meshFileName;
}

std::string GetDPLauncher::readOutputFile(const std::string &fileName)
{
	std::ifstream outputFile(fileName);

	std::stringstream content;
	std::string line;

	for (std::string line; std::getline(outputFile, line); ) 
	{
		content << line << std::endl;
	}

	return content.str();
}

std::string GetDPLauncher::getUniqueTempDir(void)
{
	std::string tempDir = getSystemTempDir();

	size_t count = 1;
	std::string uniqueTempDir;

	do
	{
		uniqueTempDir = tempDir + "\\GETDP_WORK" + std::to_string(count);
		count++;

	} while (checkFileOrDirExists(uniqueTempDir));

	return uniqueTempDir;
}

std::string GetDPLauncher::getSystemTempDir(void)
{
	return readEnvironmentVariable("TMP");
}

std::string GetDPLauncher::readEnvironmentVariable(const std::string &variableName)
{
	std::string variableValue;

	const int nSize = 32767;
	char *buffer = new char[nSize];

	if (GetEnvironmentVariableA(variableName.c_str(), buffer, nSize))
	{
		variableValue = buffer;
	}

	return variableValue;
}

bool GetDPLauncher::checkFileOrDirExists(const std::string &path)
{
	struct stat info;

	if (stat(path.c_str(), &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;   // This is a directoy
	else
		return true;   // This might be a file
}

bool GetDPLauncher::deleteDirectory(const std::string &pathName)
{
	// First delete all files in the directoy
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	std::string fileNamePattern = pathName + "\\*";

	hFind = FindFirstFileA(fileNamePattern.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		std::string fileName = FindFileData.cFileName;
		if (fileName != "." && fileName != "..")
		{
			fileName = pathName + "\\" + fileName;
			if (!DeleteFileA(fileName.c_str())) std::cout << "ERROR: Unable to delete file: " << fileName << std::endl;
		}

		while (FindNextFileA(hFind, &FindFileData))
		{
			std::string fileName = FindFileData.cFileName;
			if (fileName != "." && fileName != "..")
			{
				fileName = pathName + "\\" + fileName;
				if (!DeleteFileA(fileName.c_str())) std::cout << "ERROR: Unable to delete file: " << fileName << std::endl;
			}
		}
		
		FindClose(hFind);
	}

	// Now remove the empty directory
	bool success = RemoveDirectoryA(pathName.c_str());
	return success;
}

