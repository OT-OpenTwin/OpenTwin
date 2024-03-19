#include "StudioSuiteConnector/ProjectManager.h"
#include "StudioSuiteConnector/ServiceConnector.h"
#include "StudioSuiteConnector/StudioConnector.h"
#include "StudioSuiteConnector/VersionFile.h"
#include "StudioSuiteConnector/ProgressInfo.h"

#include "OTCommunication/ActionTypes.h"

#include "EntityBinaryData.h"
#include "EntityFile.h"
#include "DataBase.h"
#include "ClassFactory.h"

#include <filesystem>
#include <algorithm>
#include <chrono>
#include <thread>
#include <sstream>
#include <cstdio>

#include <QFileDialog>					// QFileDialog
#include <qdir.h>						// QDir

void ProjectManager::setStudioServiceData(const std::string& studioSuiteServiceURL, QObject* mainObject)
{
	ServiceConnector::getInstance().setServiceURL(studioSuiteServiceURL);
	ServiceConnector::getInstance().setMainObject(mainObject);

	ProgressInfo::getInstance().setMainObject(mainObject);
}

void ProjectManager::importProject(const std::string& fileName, const std::string& prjName)
{
	try
	{
		currentOperation = OPERATION_IMPORT;

		uploadFileList.clear();
		projectName.clear();
		baseProjectName.clear();
		cacheFolderName.clear();
		newOrModifiedFiles.clear();
		dependentDataFiles.clear();
		deletedFiles.clear();

		projectName = prjName;
		changeMessage = "Import CST Studio Suite project";

		ProgressInfo::getInstance().setProgressState(true, "Importing project", false);
		ProgressInfo::getInstance().setProgressValue(0);

		// Determine the base project name (without .cst extension)
		baseProjectName = getBaseProjectName(fileName);

		// Create the cache folder
		cacheFolderName = createCacheFolder(baseProjectName);

		// Open the cst project in a studio suite instance, save it and extract the data
		StudioConnector studioObject;
		studioObject.searchProjectAndExtractData(fileName);

		// Get the files to be uploaded
		uploadFileList = determineUploadFiles(baseProjectName);

		ProgressInfo::getInstance().setProgressValue(10);

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_UPLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COUNT, 2 * uploadFileList.size(), doc.GetAllocator());  // We need ids for the data entities and the file entities

		ServiceConnector::getInstance().sendExecuteRequest(doc);
	}
	catch (std::string &error)
	{
		ProgressInfo::getInstance().setProgressState(false, "", false);
		ProgressInfo::getInstance().showError(error);
		ProgressInfo::getInstance().unlockGui();
	}
}

std::string ProjectManager::getCurrentVersion(const std::string& fileName, const std::string& prjName)
{
	baseProjectName = getBaseProjectName(fileName);

	// Set the name of the cache folder
	cacheFolderName = baseProjectName + ".cache";
	if (!std::filesystem::is_directory(cacheFolderName))
	{
		throw std::string("The cache folder does not exist. This project does not seem to be initialized or connected to version control.");
	}

	// Load the version data
	VersionFile version(cacheFolderName + "\\version.info");
	version.read();

	// We need to ensure that the local cache diretory actually belongs to the open project.
	if (version.getProjectName() != prjName)
	{
		// This cache folder does not belong to the right project
		throw std::string("The local file data does not belong to the project.");
	}

	// We need to ensure that the model is at the right version in OT
	return version.getVersion();
}

void ProjectManager::commitProject(const std::string& fileName, const std::string& prjName, const std::string& changeComment)
{
	try
	{
		currentOperation = OPERATION_COMMIT;

		uploadFileList.clear();
		projectName.clear();
		baseProjectName.clear();
		cacheFolderName.clear();
		newOrModifiedFiles.clear();
		dependentDataFiles.clear();
		deletedFiles.clear();

		projectName = prjName;
		changeMessage = changeComment;

		ProgressInfo::getInstance().setProgressState(true, "Committing project changes", false);
		ProgressInfo::getInstance().setProgressValue(0);

		// Determine the base project name (without .cst extension)
		baseProjectName = getBaseProjectName(fileName);

		// Set the name of the cache folder
		cacheFolderName = baseProjectName + ".cache";

		// Open the cst project in a studio suite instance, save it and extract the data
		StudioConnector studioObject;
		studioObject.searchProjectAndExtractData(fileName);

		// Get the files to be uploaded
		uploadFileList = determineUploadFiles(baseProjectName);

		ProgressInfo::getInstance().setProgressValue(10);

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_UPLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COUNT, 2 * uploadFileList.size(), doc.GetAllocator());  // We need ids for the data entities and the file entities

		ServiceConnector::getInstance().sendExecuteRequest(doc);
	}
	catch (std::string& error)
	{
		ProgressInfo::getInstance().setProgressState(false, "", false);
		ProgressInfo::getInstance().showError(error);
		ProgressInfo::getInstance().unlockGui();
	}
}

void ProjectManager::getProject(const std::string& fileName, const std::string& prjName, const std::string& version)
{
	try
	{
		currentOperation = OPERATION_GET;

		uploadFileList.clear();
		projectName.clear();
		baseProjectName.clear();
		cacheFolderName.clear();
		newOrModifiedFiles.clear();
		dependentDataFiles.clear();
		deletedFiles.clear();

		projectName = prjName;

		ProgressInfo::getInstance().setProgressState(true, "Getting project", false);
		ProgressInfo::getInstance().setProgressValue(0);

		// Determine the base project name (without .cst extension)
		baseProjectName = getBaseProjectName(fileName);

		// Set the name of the cache folder
		cacheFolderName = baseProjectName + ".cache";

		// Make sure to close the cst project in a studio suite instance
		StudioConnector studioObject;
		studioObject.closeProject(fileName);

		// Delete project files
		deleteLocalProjectFiles(baseProjectName);

		// Check whether version is in the cache
		if (!restoreFromCache(baseProjectName, cacheFolderName, version))
		{
			// The project was not found in the cache. Therefore, the files need to be retrieved from the repo
			ProgressInfo::getInstance().setProgressValue(10);

			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_DOWNLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());

			ServiceConnector::getInstance().sendExecuteRequest(doc);
		}
		else
		{
			// We have successfully restored the project data, so we can now open the project
			studioObject.openProject(fileName);

			// Update the version file
			writeVersionFile(baseProjectName, projectName, version, cacheFolderName);

			ProgressInfo::getInstance().setProgressState(false, "", false);
			ProgressInfo::getInstance().unlockGui();

			ProgressInfo::getInstance().showInformation("The CST Studio Suite project has been restored successfully to version " + version + ".");
		}
	}
	catch (std::string& error)
	{
		ProgressInfo::getInstance().setProgressState(false, "", false);
		ProgressInfo::getInstance().showError(error);
		ProgressInfo::getInstance().unlockGui();
	}
}

void ProjectManager::uploadFiles(std::list<ot::UID> &entityIDList, std::list<ot::UID> &entityVersionList)
{
	ProgressInfo::getInstance().setProgressValue(10);

	try
	{
		// Determine the project root folder
		std::filesystem::path projectPath(baseProjectName);
		std::string projectRoot = projectPath.parent_path().string();

		// Upload files
		uploadFiles(projectRoot, uploadFileList, entityIDList, entityVersionList);

		// Send the units information
		sendUnitsInformation(baseProjectName);

		// Send the material information
		sendMaterialInformation(baseProjectName);

		// Send the shapes information and triangulations
		sendShapeInformationAndTriangulation(baseProjectName);

		// Create the new version
		commitNewVersion(changeMessage);

		ProgressInfo::getInstance().setProgressValue(70);
	}
	catch (std::string& error)
	{
		ProgressInfo::getInstance().showError(error);
	}

	ProgressInfo::getInstance().setProgressState(false, "", false);
	ProgressInfo::getInstance().unlockGui();
}

void ProjectManager::copyFiles(const std::string &newVersion)
{
	try
	{
		// Copy the files to the cache directory
		copyCacheFiles(baseProjectName, newVersion, cacheFolderName);

		// Store version information
		writeVersionFile(baseProjectName, projectName, newVersion, cacheFolderName);

		ProgressInfo::getInstance().setProgressValue(100);

		if (currentOperation == OPERATION_IMPORT)
		{
			ProgressInfo::getInstance().showInformation("The CST Studio Suite project has been imported successfully.");
		}
		else if (currentOperation == OPERATION_COMMIT)
		{
			ProgressInfo::getInstance().showInformation("The CST Studio Suite project has been commited successfully (version: " + newVersion + ").");
		}
		else
		{
			assert(0); // Unexpected operation
		}
	}
	catch (std::string& error)
	{
		ProgressInfo::getInstance().showError(error);
	}

	ProgressInfo::getInstance().setProgressState(false, "", false);
	ProgressInfo::getInstance().unlockGui();
}

std::string ProjectManager::getBaseProjectName(const std::string& cstFileName)
{
	size_t index = cstFileName.rfind('.');
	if (index == std::string::npos) throw("Unable to determine project base name from cst file name: " + cstFileName);

	return cstFileName.substr(0, index);
}

std::string ProjectManager::createCacheFolder(const std::string& baseProjectName)
{
	std::string cacheFolderName = baseProjectName + ".cache";

	if (std::filesystem::exists(cacheFolderName))
	{
		throw("Unable to create cache folder since it already exists: " + cacheFolderName);
	}

	if (!std::filesystem::create_directory(cacheFolderName))
	{
		throw("Unable to create cache folder: " + cacheFolderName);
	}

	return cacheFolderName;
}

std::list<std::string> ProjectManager::determineUploadFiles(const std::string& baseProjectName)
{
	std::list<std::string> uploadFiles;
	std::string path;

	path = baseProjectName + ".cst";
	std::replace(path.begin(), path.end(), '\\', '/');

	uploadFiles.push_back(path);

	// Now add the content of the results folder
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(baseProjectName + "/Result"))
	{
		if (!dirEntry.is_directory())
		{
			path = dirEntry.path().string();
			std::replace(path.begin(), path.end(), '\\', '/');

			uploadFiles.push_back(path);
		}
	}

	return uploadFiles;
}

void ProjectManager::copyCacheFiles(const std::string& baseProjectName, const std::string &newVersion, const std::string &cacheFolderName)
{
	// Create new version subfolder
	std::string versionFolderName = cacheFolderName + "/" + newVersion;

	if (std::filesystem::exists(versionFolderName))
	{
		throw("Unable to create cache version folder since it already exists: " + versionFolderName);
	}

	if (!std::filesystem::create_directory(versionFolderName))
	{
		throw("Unable to create cache version folder: " + versionFolderName);
	}

	if (!std::filesystem::create_directory(versionFolderName + "/Result"))
	{
		throw("Unable to create cache result folder: " + versionFolderName + "/Result");
	}

	// Now copy all upload files to the cache version folder
	std::string cstFileName = baseProjectName + ".cst";
	std::string resultFolderName = baseProjectName + "/Result";

	std::replace(cstFileName.begin(), cstFileName.end(), '\\', '/');
	std::replace(resultFolderName.begin(), resultFolderName.end(), '\\', '/');

	try
	{
		std::filesystem::copy(cstFileName, versionFolderName);
		std::filesystem::copy(resultFolderName, versionFolderName + "/Result", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
	}
	catch (std::exception &error)
	{
		throw("Unable to copy data to cache (" + std::string(error.what()) + ") : " + versionFolderName);
	}
}

void ProjectManager::writeVersionFile(const std::string& baseProjectName, const std::string& projectName, const std::string& newVersion, const std::string& cacheFolderName)
{
	VersionFile version(std::string(cacheFolderName + "/version.info"));
	version.write(baseProjectName, projectName, newVersion);
}

void ProjectManager::uploadFiles(const std::string &projectRoot, std::list<std::string>& uploadFileList, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList)
{
	size_t dataSize = 0;

	DataBase::GetDataBase()->queueWriting(true);

	for (auto file : uploadFileList)
	{
		ot::UID dataEntityID = entityIDList.front(); entityIDList.pop_front();
		ot::UID fileEntityID = entityIDList.front(); entityIDList.pop_front();

		ot::UID dataVersion = entityVersionList.front(); entityVersionList.pop_front();
		ot::UID fileVersion = entityVersionList.front(); entityVersionList.pop_front();

		// add the file to the list of modified files
		newOrModifiedFiles[file] = std::pair<ot::UID, ot::UID>(fileEntityID, fileVersion);
		dependentDataFiles[file] = std::pair<ot::UID, ot::UID>(dataEntityID, dataVersion);

		// upload the binary data entity
		EntityBinaryData *dataEntity = new EntityBinaryData(dataEntityID, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

		std::ifstream dataFile(file, std::ios::binary | std::ios::ate);

		std::streampos size = dataFile.tellg();

		char *memBlock = new char[size];
		dataFile.seekg(0, std::ios::beg);
		dataFile.read(memBlock, size);
		dataFile.close();

		dataSize = size;

		dataEntity->setData(memBlock, size);
		dataEntity->StoreToDataBase(dataVersion);

		delete dataEntity;
		dataEntity = nullptr;

		delete[] memBlock;
		memBlock = nullptr;

		// Upload the file entity
		EntityFile* fileEntity = new EntityFile(fileEntityID, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

		std::string pathName = file.substr(projectRoot.size() + 1);
		std::filesystem::path filePath(pathName);

		fileEntity->setName("Files/" + pathName);
		fileEntity->setFileProperties(pathName, filePath.filename().string(), "Absolute");
		fileEntity->setData(dataEntityID, dataVersion);
		fileEntity->setEditable(false);

		fileEntity->StoreToDataBase(fileVersion);

		delete fileEntity;
		fileEntity = nullptr;

		if (dataSize > 100000000)
		{
			// We have more than 100 MB since the last store
			DataBase::GetDataBase()->flushWritingQueue();
			dataSize = 0;
		}
	}

	DataBase::GetDataBase()->queueWriting(false);
}

void ProjectManager::commitNewVersion(const std::string &changeMessage)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_FILES_UPLOADED, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(changeMessage, doc.GetAllocator()), doc.GetAllocator());

	// add the list of new files and modified files together with the entityID and versions
	std::list<std::string> fileNameList;
	std::list<ot::UID> fileEntityIDList;
	std::list<ot::UID> fileVersionList;
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataVersionList;

	for (auto item : newOrModifiedFiles)
	{
		fileNameList.push_back(item.first);
		fileEntityIDList.push_back(item.second.first);
		fileVersionList.push_back(item.second.second);

		dataEntityIDList.push_back(dependentDataFiles[item.first].first);
		dataVersionList.push_back(dependentDataFiles[item.first].second);
	}

	doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonArray(fileNameList, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, ot::JsonArray(fileEntityIDList, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, ot::JsonArray(fileVersionList, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_DataID, ot::JsonArray(dataEntityIDList, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_DataVersion, ot::JsonArray(dataVersionList, doc.GetAllocator()), doc.GetAllocator());

	// add the list of deleted files
	doc.AddMember(OT_ACTION_CMD_MODEL_DeleteEntity, ot::JsonArray(deletedFiles, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	ServiceConnector::getInstance().sendExecuteRequest(doc);
}

void ProjectManager::sendUnitsInformation(const std::string &projectRoot)
{
	std::string fileContent;
	readFileContent(projectRoot + "/Temp/Upload/units.info", fileContent);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_UNITS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(fileContent, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	ServiceConnector::getInstance().sendExecuteRequest(doc);
}

void ProjectManager::sendMaterialInformation(const std::string& projectRoot)
{
	std::string fileContent;
	readFileContent(projectRoot + "/Temp/Upload/material.info", fileContent);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_MATERIALS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(fileContent, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	ServiceConnector::getInstance().sendExecuteRequest(doc);
}

void ProjectManager::sendShapeInformationAndTriangulation(const std::string& projectRoot)
{
	std::string fileContent;
	readFileContent(projectRoot + "/Temp/Upload/shape.info", fileContent);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_SHAPEINFO, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(fileContent, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	ServiceConnector::getInstance().sendExecuteRequest(doc);

	// Read the list of shape names and assign them the index of the STL file
	std::map<std::string, int> allShapesMap = determineAllShapes(std::stringstream(fileContent));
	 
	// Determine which triangulation need to be sent (either different checksum or new)
	std::map<std::string, int> modifiedShapesMap = allShapesMap;  // TODO: Implement check

	// Now send the modified triangulations (one by one)
	sendTriangulations(projectRoot, modifiedShapesMap);
}

void ProjectManager::sendTriangulations(const std::string& projectRoot, std::map<std::string, int> trianglesMap)
{
	std::list<std::string> shapeNames;
	std::list<std::string> shapeTriangles;

	// We combine potentially several triangulations into a single message in order to avoid sending an excessive number of 
	// messages in case of many small objects.
	size_t dataSize = 0;

	for (auto shape : trianglesMap)
	{
		std::string fileContent;
		readFileContent(projectRoot + "/Temp/Upload/stl" + std::to_string(shape.second) + ".stl", fileContent);

		shapeNames.push_back(shape.first);
		shapeTriangles.push_back(fileContent);

		dataSize += (shape.first.size() + fileContent.size());

		if (dataSize > 10000000)
		{
			sendTriangleLists(shapeNames, shapeTriangles);
			dataSize = 0;
			shapeNames.clear();
			shapeTriangles.clear();
		}
	}

	if (dataSize > 0)
	{
		sendTriangleLists(shapeNames, shapeTriangles);
	}
}

void ProjectManager::sendTriangleLists(std::list<std::string>& shapeNames, std::list<std::string>& shapeTriangles)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_TRIANGLES, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonArray(shapeNames, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonArray(shapeTriangles, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	ServiceConnector::getInstance().sendExecuteRequest(doc);
}

std::map<std::string, int> ProjectManager::determineAllShapes(std::stringstream fileContent)
{
	std::map<std::string, int> shapeNameToIdMap;

	int id = 0; 

	while (!fileContent.eof())
	{
		std::string name, material;

		fileContent >> name;
		fileContent >> material;

		if (!name.empty())
		{
			shapeNameToIdMap[name] = id;
			id++;
		}
	}

	return shapeNameToIdMap;
}

void ProjectManager::readFileContent(const std::string &fileName, std::string &content)
{
	content.clear();

	std::ifstream t(fileName);
	if (!t.is_open()) throw std::string("Unable to read file: " + fileName);

	t.seekg(0, std::ios::end);
	content.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	content.assign((std::istreambuf_iterator<char>(t)),
					std::istreambuf_iterator<char>());
}

void ProjectManager::deleteLocalProjectFiles(const std::string &baseProjectName)
{
	std::string cstFileName = baseProjectName + ".cst";
	std::remove(cstFileName.c_str());

	std::filesystem::remove_all(baseProjectName);
}

bool ProjectManager::restoreFromCache(const std::string& baseProjectName, const std::string& cacheFolderName, const std::string& version)
{
	try
	{
		std::string cacheDirectory = cacheFolderName + "/" + version;

		// Check whether current version is in cache
		if (std::filesystem::is_directory(cacheDirectory))
		{
			std::filesystem::path basePath(baseProjectName);
			std::string simpleFileName = basePath.filename().string() + ".cst";

			std::string cstCacheFileName = cacheDirectory + "/" + simpleFileName;

			// We have a corresponding cache entry -> restore the data
			std::filesystem::create_directory(baseProjectName);

			std::filesystem::copy(cstCacheFileName, baseProjectName + ".cst");
			std::filesystem::copy(cacheDirectory + "/Result", baseProjectName + "/Result", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);

			return true;
		}
	}
	catch (std::exception)
	{

	}

	return false;
}

bool ProjectManager::checkValidLocalFile(std::string fileName, std::string projectName, bool ensureProjectExists)
{
	std::string baseProjectName;

	try
	{
		baseProjectName = getBaseProjectName(fileName);
	}
	catch (std::string)
	{
		// We were unable to extract the project name from the file -> invalid file name
		return false;
	}

	// Set the name of the cache folder
	std::string cacheFolderName = baseProjectName + ".cache";

	bool cstFileExists = std::filesystem::is_regular_file(fileName);
	if (!cstFileExists && ensureProjectExists) return false; // We need an existing project, but this one did not exist

	// Now we check whether the associated directory is ok
	if (cstFileExists)
	{
		// The directory also needs to exist in this case
		if (!std::filesystem::is_directory(baseProjectName))
		{
			return false;
		}
	}
	else
	{
		// The directory must not exist in this case (and the cache folder must not exist as well
		if (std::filesystem::is_directory(baseProjectName))
		{
			return false;
		}

		if (std::filesystem::is_directory(cacheFolderName))
		{
			return false;
		}

		// We have a new project name
		return true;
	}

	// In this case, we have an existing project
	if (!std::filesystem::is_directory(cacheFolderName))
	{
		return false;
	}

	// Load the version data
	VersionFile version(cacheFolderName + "\\version.info");
	version.read();

	// We need to ensure that the local cache diretory actually belongs to the open project.
	return (version.getProjectName() == projectName);
}

void ProjectManager::downloadFiles(const std::string& fileName, const std::string& projectName, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList, const std::string& version)
{
	// Determine the base project name (without .cst extension)
	baseProjectName = getBaseProjectName(fileName);

	// Set the name of the cache folder
	cacheFolderName = baseProjectName + ".cache";

	// Create the cache folder if it does not exist
	if (!std::filesystem::exists(cacheFolderName))
	{
		std::filesystem::create_directory(cacheFolderName);
	}

	std::string cacheFolderVersion = cacheFolderName + "/" + version;

	// Now download the files into the cache directory
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs;
	auto versionID = entityVersionList.begin();
	for (auto entityID : entityIDList)
	{
		prefetchIDs.push_back(std::pair<unsigned long long, unsigned long long>(entityID, *versionID));
		versionID++;
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIDs);

	bool success = true;

	for (auto entity : prefetchIDs)
	{
		// Download a single cache file
		success &= downloadFile(cacheFolderVersion, entity.first, entity.second);
	}

	if (!success)
	{
		ProgressInfo::getInstance().showError("The CST Studio Suite project could not be restored to version " + version + ".");

		ProgressInfo::getInstance().setProgressState(false, "", false);
		ProgressInfo::getInstance().unlockGui();

		return;
	}
	
	// Finally restore the project from the cache
	if (!restoreFromCache(baseProjectName, cacheFolderName, version))
	{
		ProgressInfo::getInstance().showError("The CST Studio Suite project could not be restored to version " + version + ".");

		ProgressInfo::getInstance().setProgressState(false, "", false);
		ProgressInfo::getInstance().unlockGui();

		return;
	}

	// We have successfully restored the project data, so we can now open the project
	StudioConnector studioObject;
	studioObject.openProject(fileName);

	// Update the version file
	writeVersionFile(baseProjectName, projectName, version, cacheFolderName);

	ProgressInfo::getInstance().setProgressState(false, "", false);
	ProgressInfo::getInstance().unlockGui();

	ProgressInfo::getInstance().showInformation("The CST Studio Suite project has been restored successfully to version " + version + ".");
}

bool ProjectManager::downloadFile(const std::string &cacheFolderVersion, ot::UID entityID, ot::UID version)
{
	bool success = true;

	ClassFactory classFactory;
	EntityFile* fileEntity = dynamic_cast<EntityFile*> (DataBase::GetDataBase()->GetEntityFromEntityIDandVersion(entityID, version, &classFactory));

	if (fileEntity != nullptr)
	{
		size_t size = fileEntity->getData()->getData().size();

		std::string entityPath = fileEntity->getPath();
		size_t index = entityPath.find('/');

		std::string fileName = cacheFolderVersion + "/" + entityPath.substr(index+1);

		std::filesystem::path path(fileName);
		std::string parentFolder = path.parent_path().string();

		std::filesystem::create_directories(parentFolder);

		std::ofstream dataFile(fileName, std::ios::binary);
		dataFile.write(fileEntity->getData()->getData().data(), size);
		dataFile.close();

		delete fileEntity;
		fileEntity = nullptr;
	}
	else
	{
		success = false;
	}

	return success;
}

