// @otlicense
// File: FDTDLauncher.cpp
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

#include "FDTDLauncher.h"
#include "FDTDConfig.h"

#include "DataBase.h"
#include "Application.h"
#include "SolverBase.h"
#include "SolverFDTD.h"

#include "EntityMeshTet.h"
#include "EntityMeshTetData.h"
#include "EntityBinaryData.h"
#include "EntityProperties.h"

#include "CartesianMeshToSTL.h"

#include "OldTreeIcon.h"

#include "OTCore/LogDispatcher.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include <windows.h> // winapi

#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>


FDTDLauncher::FDTDLauncher(Application *app)
	: application(app)
{

}

FDTDLauncher::~FDTDLauncher()
{

}

std::string FDTDLauncher::startSolver(std::string &logFileText, const std::string &dataBaseURL, const std::string &uiUrl, const std::string &projectName, EntityBase* solverEntity, int serviceID, int sessionCount, ot::components::ModelComponent *modelComponent)
{
	std::string outputText;
	logFileText.clear();

	// Find the name of the temporary directory where we run the solver and create the directory
	std::string tempDirPath = getUniqueTempDir();
	if (tempDirPath.empty()) return "ERROR: Unable to create temporary working directory (TMP environment variable needs to be set)";

	if (_mkdir(tempDirPath.c_str()) == -1) {
		return "ERROR: Unable to create temporary working directory (TMP environment variable needs to be set)";
	}

	EntityPropertiesEntityList *mesh = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty("Mesh"));
	assert(mesh != nullptr);
	if (mesh == nullptr) {
		return "ERROR: Unable to read mesh information for solver.";
	}
	
	// Create a solver depending on the type
	std::string problemType = getProblemType(solverEntity);

	SolverBase * solver = nullptr;

	solver = new SolverFDTD();

	application->getUiComponent()->setProgressInformation("Solver running: " + problemType, true);

	try
	{
		// Create the FDTD configuration and write the XML file
		FDTDConfig cfg;
		cfg.setFromEntity(solverEntity);
		cfg.loadSTLMesh(mesh->getValueName(), tempDirPath);
		tinyxml2::XMLDocument doc;
		cfg.addToXML(doc);
		// Save the XML file
		std::string tempFilePath = tempDirPath + "\\FDTD.xml";
		doc.SaveFile(tempFilePath.c_str());

		// Run the solver
		logFileText = solver->runSolver(tempDirPath, application->getUiComponent());

		// Convert the results
		//solver->convertResults(tempDirPath, application, solverEntity);
	}
	catch (std::string& error)
	{
		outputText = "ERROR: " + error;
	}
	catch (const std::exception& e) {
		outputText = "ERROR: Exception occurred: " + std::string(e.what());
	}
	application->getUiComponent()->closeProgressInformation();

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

void FDTDLauncher::readMeshItemInfo(ot::UID meshDataID, std::map<ot::UID, ot::EntityInformation>& meshItemInfo)
{
	std::list<ot::EntityInformation> info;
	ot::ModelServiceAPI::getEntityChildInformation(meshDataID, info, true);

	for (const auto& item : info)
	{
		meshItemInfo[item.getEntityID()] = item;
	}
}

void FDTDLauncher::readMaterialProperties(std::map<std::string, EntityProperties>& materialProperties)
{
	application->getModelComponent()->loadMaterialInformation();

	std::map<ot::UID, EntityProperties> entityProperties;
	ot::ModelServiceAPI::getEntityProperties("Materials", true, "", entityProperties);

	ot::UIDList entityIDList;
	for (const auto& item : entityProperties)
	{
		entityIDList.push_back(item.first);
	}

	std::list<ot::EntityInformation> entityInformation;
	ot::ModelServiceAPI::getEntityInformation(entityIDList, entityInformation);

	for (auto &entity : entityInformation)
	{
		materialProperties[entity.getEntityName()] = entityProperties[entity.getEntityID()];
	}
}

void FDTDLauncher::readGroupsFromMesh(const std::string& meshFileName, std::map<std::string, size_t>& groupNameToIdMap)
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

std::string FDTDLauncher::getProblemType(EntityBase* solverEntity)
{
	EntityPropertiesSelection* solver = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Problem type"));
	assert(solver != nullptr);

	if (solver == nullptr) return "";

	return solver->getValue();
}

std::string FDTDLauncher::extractMesh(EntityBase* solverEntity, const std::string &tempDirPath, ot::UID &meshDataID, std::string &currentMeshDataName)
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

	ot::ModelServiceAPI::getEntityInformation(entityList, entityInfo);
	if (entityInfo.empty()) return "";

	meshDataID = entityInfo.front().getEntityID();

	// Get the gmsh data storage ID
	ot::UIDList entityIDList;
	entityIDList.push_back(meshDataID);
	application->prefetchDocumentsFromStorage(entityIDList);

	EntityMeshTetData* entity = dynamic_cast<EntityMeshTetData*> (ot::EntityAPI::readEntityFromEntityIDandVersion(meshDataID, application->getPrefetchedEntityVersion(meshDataID)));
	if (entity == nullptr) return "";

	ot::UID gmshDataStorageID = entity->getGmshDataStorageId();

	delete entity;
	entity = nullptr;

	// Now we load the gmsh data
	entityIDList.clear();
	entityIDList.push_back(gmshDataStorageID);
	application->prefetchDocumentsFromStorage(entityIDList);

	EntityBinaryData* entityData = dynamic_cast<EntityBinaryData*> (ot::EntityAPI::readEntityFromEntityIDandVersion(gmshDataStorageID, application->getPrefetchedEntityVersion(gmshDataStorageID)));
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

std::string FDTDLauncher::readOutputFile(const std::string &fileName)
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

std::string FDTDLauncher::getUniqueTempDir(void)
{
	std::string tempDir = getSystemTempDir();

	size_t count = 1;
	std::string uniqueTempDir;

	do
	{
		uniqueTempDir = tempDir + "\\FDTD_WORK" + std::to_string(count);
		count++;

	} while (checkFileOrDirExists(uniqueTempDir));

	return uniqueTempDir;
}

std::string FDTDLauncher::getSystemTempDir(void)
{
	return readEnvironmentVariable("TMP");
}

std::string FDTDLauncher::readEnvironmentVariable(const std::string &variableName)
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

bool FDTDLauncher::checkFileOrDirExists(const std::string &path)
{
	struct stat info;

	if (stat(path.c_str(), &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;   // This is a directoy
	else
		return true;   // This might be a file
}

bool FDTDLauncher::deleteDirectory(const std::string &pathName)
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

