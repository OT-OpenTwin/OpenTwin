// @otlicense
// File: ProjectManager.cpp
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

#include "StudioSuiteConnector/ProjectManager.h"
#include "StudioSuiteConnector/StudioConnector.h"
#include "StudioSuiteConnector/VersionFile.h"
#include "StudioSuiteConnector/Result1DFileManager.h"

#include "OTCommunication/ActionTypes.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"
#include "OTFrontendConnectorAPI/CommunicationAPI.h"

#include "EntityBinaryData.h"
#include "EntityFile.h"
#include "DataBase.h"
#include "DocumentAPI.h"

#include <filesystem>
#include <algorithm>
#include <chrono>
#include <thread>
#include <sstream>
#include <cstdio>
#include <fstream>

#include <QFileDialog>					// QFileDialog
#include <qdir.h>						// QDir
#include <qsettings>
#include <qhostinfo>
#include <QCryptographicHash>

#include "boost/algorithm/string.hpp"
#include "zlib.h"
#include "base64.h"

void ProjectManager::openProject()
{
	uploadFileList.clear();
	projectName.clear();
	baseProjectName.clear();
	cacheFolderName.clear();
	newOrModifiedFiles.clear();
	dependentDataFiles.clear();
	deletedFiles.clear();
	changeMessage.clear();
	currentOperation = OPERATION_NONE;
	localProjectFileName.clear();
}

void ProjectManager::setLocalFileName(std::string fileName)
{ 
	localProjectFileName = fileName; 
}

void ProjectManager::setStudioServiceData(const std::string& studioSuiteServiceURL)
{
	ot::CommunicationAPI::setDefaultConnectorServiceUrl(studioSuiteServiceURL);
}

void ProjectManager::importProject(const std::string& fileName, const std::string& prjName, const std::string &message, bool incResults, bool incParametricResults)
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
		changeMessage = message;
		includeResults = incResults;
		includeParametricResults = incParametricResults;

		ot::WindowAPI::setProgressBarVisibility("Importing Project", true, false);
		ot::WindowAPI::setProgressBarValue(0);
		ot::WindowAPI::setProgressBarValue(0);

		// Determine the base project name (without .cst extension)
		baseProjectName = getBaseProjectName(fileName);

		// Create the cache folder
		cacheFolderName = createCacheFolder(baseProjectName);

		// Open the cst project in a studio suite instance, save it and extract the data
		StudioConnector studioObject;
		studioObject.searchProjectAndExtractData(fileName, baseProjectName, includeResults, includeParametricResults);

		// Get the files to be uploaded
		uploadFileList = determineUploadFiles(baseProjectName, includeResults);

		ot::WindowAPI::setProgressBarVisibility("Importing project", true, false);
		ot::WindowAPI::setProgressBarValue(15);

		std::string hostName = QHostInfo::localHostName().toStdString();

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_UPLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COUNT, 2 * uploadFileList.size(), doc.GetAllocator());  // We need ids for the data entities and the file entities
		doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonString(fileName, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());

		std::string tmp;
		ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
	}
	catch (std::string &error)
	{
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::showErrorPrompt("Studio Suite Import Error", error);
		ot::WindowAPI::lockUI(false);
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

void ProjectManager::commitProject(const std::string& fileName, const std::string& prjName, const std::string& changeComment, bool incResults, bool incParametricResults)
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
		includeResults = incResults;
		includeParametricResults = incParametricResults;

		ot::WindowAPI::setProgressBarVisibility("Committing Project", true, false);
		ot::WindowAPI::setProgressBarValue(0);

		// Determine the base project name (without .cst extension)
		baseProjectName = getBaseProjectName(fileName);

		// Set the name of the cache folder
		cacheFolderName = baseProjectName + ".cache";

		// Open the cst project in a studio suite instance, save it and extract the data
		StudioConnector studioObject;
		studioObject.searchProjectAndExtractData(fileName, baseProjectName, includeResults, includeParametricResults);

		// Get the files to be uploaded
		uploadFileList = determineUploadFiles(baseProjectName, includeResults);

		ot::WindowAPI::setProgressBarVisibility("Committing project", true, false);
		ot::WindowAPI::setProgressBarValue(15);

		std::string hostName = QHostInfo::localHostName().toStdString();

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_UPLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COUNT, 2 * uploadFileList.size(), doc.GetAllocator());  // We need ids for the data entities and the file entities
		doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonString(fileName, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());

		std::string tmp;
		ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
	}
	catch (std::string& error)
	{
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::showErrorPrompt("Studio Suite Commit Error", error);
		ot::WindowAPI::lockUI(false);
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

		ot::WindowAPI::setProgressBarVisibility("Getting Project", true, false);
		ot::WindowAPI::setProgressBarValue(0);

		// Determine the base project name (without .cst extension)
		baseProjectName = getBaseProjectName(fileName);

		// Set the name of the cache folder
		cacheFolderName = baseProjectName + ".cache";

		// Make sure to close the cst project in a studio suite instance
		StudioConnector studioObject;
		studioObject.closeProject(fileName);

		ot::WindowAPI::setProgressBarVisibility("Getting project", true, false);
		ot::WindowAPI::setProgressBarValue(10);

		// Delete project files
		deleteLocalProjectFiles(baseProjectName);

		// Check whether version is in the cache
		if (!restoreFromCache(baseProjectName, cacheFolderName, version))
		{
			// The project was not found in the cache. Therefore, the files need to be retrieved from the repo
			ot::WindowAPI::setProgressBarValue(20);

			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_DOWNLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());

			std::string tmp;
			ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
		}
		else
		{
			ot::WindowAPI::setProgressBarValue(90);

			// We have successfully restored the project data, so we can now open the project
			studioObject.openProject(fileName);

			// Update the version file
			writeVersionFile(baseProjectName, projectName, version, cacheFolderName);

			ot::WindowAPI::setProgressBarVisibility("", false, false);
			ot::WindowAPI::lockUI(false);
			ot::WindowAPI::showInfoPrompt("Get Project", "The CST Studio Suite project has been restored successfully to version " + version + ".");
		}
	}
	catch (std::string& error)
	{
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::showErrorPrompt("Studio Suite Get Error", error);
		ot::WindowAPI::lockUI(false);
	}
}

void ProjectManager::uploadFiles(std::list<ot::UID> &entityIDList, std::list<ot::UID> &entityVersionList, ot::UID infoEntityID, ot::UID infoEntityVersion)
{
	ot::WindowAPI::setProgressBarValue(15);

	try
	{
		// Determine the project root folder
		std::filesystem::path projectPath(baseProjectName);
		std::string projectRoot = projectPath.parent_path().string();

		// Load the hash information for the entities 
		InfoFileManager infoFileManager(infoEntityID, infoEntityVersion);

		// Upload files (progress range 15-70)
		uploadFiles(projectRoot, uploadFileList, entityIDList, entityVersionList);

		// Send the units information
		sendUnitsInformation(baseProjectName);

		// Send the material information
		sendMaterialInformation(baseProjectName);

		// Send the parameter information
		sendParameterInformation(baseProjectName);

		// Send the history information
		sendHistoryInformation(baseProjectName);

		// Send the shapes information and triangulations (progress range 70-80)
		sendShapeInformationAndTriangulation(baseProjectName, infoFileManager);

		// Send the (parametric) 1D result data (progress range 80-90)
		send1dResultData(baseProjectName, infoFileManager);

		// Create the new version
		commitNewVersion(changeMessage);

		ot::WindowAPI::setProgressBarValue(90);
	}
	catch (std::string& error)
	{
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::lockUI(false);
		ot::WindowAPI::showErrorPrompt("Studio Suite Upload Error", error);
	}
}

void ProjectManager::send1dResultData(const std::string& projectRoot, InfoFileManager &infoFileManager)
{
	// First, we get a list of all available run-ids
	std::string uploadDirectory = projectRoot + "/Temp/Upload/";

	std::list<int> runIds = getAllRunIds(uploadDirectory);

	// Load all result data and determine the hashes
	Result1DFileManager result1DData(uploadDirectory, runIds);

	// Now check whether the run-ids (1,2,3,...) have changed. In this case, a full upload is needed.
	// Otherwise, only the new run-ids need to be uploaded (added).
	std::list<int> changedRunIds = checkForChangedData(runIds, uploadDirectory, result1DData, infoFileManager);

	// Here we have two cases: Either we do not want to include the results. In this case, the append flag needs to be false such that the 
	// previously stored data will be deleted. If the want to include the results, the flag should be true, since if there is no change
	// the previously stored data should be kept. 

	bool appendData = includeParametricResults;  
	std::string fileId;
	size_t uncompressedDataLength = 0;

	if (!changedRunIds.empty())
	{
		appendData = (changedRunIds.size() < runIds.size());

		// Create a zip archive of the to-be uploaded run-ids (including the cache information)
		// In a first step, we calculate the buffer size to store all the data
		uncompressedDataLength = sizeof(size_t);   // Store the number of run IDs
		size_t validRunIds = 0;

		for (int runId : changedRunIds)
		{
			Result1DRunIDContainer* container = result1DData.getRunContainer(runId);
			if (container != nullptr)
			{
				uncompressedDataLength += container->getSize();
				validRunIds++;
			}
		}

		assert(uncompressedDataLength > 0);
		if (uncompressedDataLength > 0)
		{
			std::vector<char> buffer;
			buffer.reserve(uncompressedDataLength);

			// Write the number of valid runids
			char* valueBuffer = (char*)(&validRunIds);

			for (size_t index = 0; index < sizeof(size_t); index++)
			{
				buffer.push_back(valueBuffer[index]);
			}

			// Now we write all data to the buffer
			for (int runId : changedRunIds)
			{
				Result1DRunIDContainer* container = result1DData.getRunContainer(runId);
				if (container != nullptr)
				{
					container->writeToBuffer(runId, buffer);
				}
			}

			// In a next step, we do not need the original data anymore
			result1DData.clear();

			// Now upload the data to gridFS (even 1D data can be very large)
			DataStorageAPI::DocumentAPI doc;

			bsoncxx::types::value result = doc.InsertBinaryDataUsingGridFs((uint8_t*)(buffer.data()), buffer.size(), DataBase::instance().getCollectionName());
			fileId = result.get_oid().value.to_string();
		}
	}

	// Finally send the zip archive to the server (together with the information whether the data shall be replaced or added)
	// In case that we do not have any results or we do not want to add the results, the message is still sent, but with an empty content

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_RESULT1D, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_APPEND, appendData, doc.GetAllocator());  // We need ids for the data entities and the file entities
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(fileId, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content_UncompressedDataLength, rapidjson::Value(uncompressedDataLength), doc.GetAllocator());

	std::string tmp;
	ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
}

std::list<int> ProjectManager::getAllRunIds(const std::string & uploadDirectory)
{
	std::list<int> runIds;

	for (const auto& dirEntry : std::filesystem::directory_iterator(uploadDirectory))
	{
		if (dirEntry.is_directory())
		{
			// This could be a runid
			std::string dirname = dirEntry.path().filename().string();
			if (std::find_if(dirname.begin(), dirname.end(), [](unsigned char c) { return !std::isdigit(c); }) == dirname.end())
			{
				// The directory name consists of digits only -> is an integer number#
				int runId = atoi(dirname.c_str());
				if (runId != 0)
				{
					// We have a parametric result
					runIds.push_back(runId);
				}
			}
		}
	}

	// Finally we sort the list
	runIds.sort();

	return runIds;
}

std::list<int> ProjectManager::checkForChangedData(std::list<int> &allRunIds, const std::string &uploadDirectory, Result1DFileManager & result1DData, InfoFileManager& infoFileManager)
{
	std::list<int> changedRunIds;

	for (int runID : allRunIds)
	{
		// Here we check whether this runID has changed
		bool hasChanged = false;

		Result1DRunIDContainer* container = result1DData.getRunContainer(runID);
		assert(container != nullptr);
		if (container == nullptr) break;

		if (container->hasChanged(runID, infoFileManager))
		{
			changedRunIds.push_back(runID);
		}
	}
		
	return changedRunIds;
}

void ProjectManager::copyFiles(const std::string &newVersion)
{
	try
	{
		// Copy the files to the cache directory
		copyCacheFiles(baseProjectName, newVersion, cacheFolderName, includeResults);

		// Store version information
		writeVersionFile(baseProjectName, projectName, newVersion, cacheFolderName);

		ot::WindowAPI::setProgressBarValue(100);

		if (currentOperation == OPERATION_IMPORT)
		{
			ot::WindowAPI::showInfoPrompt("Import Project", "The CST Studio Suite project has been imported successfully.");
		}
		else if (currentOperation == OPERATION_COMMIT)
		{
			ot::WindowAPI::showInfoPrompt("Commit Project", "The CST Studio Suite project has been commited successfully (version: " + newVersion + ").");
		}
		else
		{
			assert(0); // Unexpected operation
		}
	}
	catch (std::string& error)
	{
		ot::WindowAPI::showErrorPrompt("Studio Suite Cache Error", error);
	}

	ot::WindowAPI::setProgressBarVisibility("", false, false);
	ot::WindowAPI::lockUI(false);
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

std::list<std::string> ProjectManager::determineUploadFiles(const std::string& baseProjectName, bool includeResults)
{
	std::list<std::string> uploadFiles;
	std::string path;

	path = baseProjectName + ".cst";
	std::replace(path.begin(), path.end(), '\\', '/');

	uploadFiles.push_back(path);

	// Read file time stamp information from the corresponding cache directory
	std::map<std::string, std::filesystem::file_time_type> cacheFileWriteTimes;
	std::map<std::string, bool> cacheFiles;
	std::string versionFolderName;

	// Get the current version
	try
	{
		VersionFile version(cacheFolderName + "\\version.info");
		version.read();

		versionFolderName = cacheFolderName + "/" + version.getVersion();

		getCacheFileWriteTimes(versionFolderName, cacheFileWriteTimes, cacheFiles);
	}
	catch (...)
	{
		// The cache does not seem to exist yet
		versionFolderName.clear();
		cacheFileWriteTimes.clear();
		cacheFiles.clear();
	}

	// Now add the content of the results folder

	if (includeResults)
	{
		std::string resultDirName = baseProjectName + "/Result";
		std::string cacheResultDirName = versionFolderName + "/Result";

		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(resultDirName))
		{
			if (!dirEntry.is_directory())
			{
				path = dirEntry.path().string();
				std::replace(path.begin(), path.end(), '\\', '/');

				std::string correspondingCacheFile = path.substr(resultDirName.length());

				if (cacheFiles.count(correspondingCacheFile) == 0)
				{
					// This file does not exist in the cache
					uploadFiles.push_back(path);
				}
				else
				{
					// This file exists in the cache
					cacheFiles[correspondingCacheFile] = true; // This file is still present

					std::filesystem::file_time_type cacheFileTimeStamp = cacheFileWriteTimes[correspondingCacheFile];
					auto newFileTimeStamp = std::filesystem::last_write_time(path);

					if (newFileTimeStamp > cacheFileTimeStamp)
					{
						// The time stamp of this file has changed. Now we need to compare the file content to determine whether 
						// the file has indeed changed
						if (fileContentDiffers(path, cacheResultDirName + correspondingCacheFile))
						{
							// This file has changed compared to the data in the cache
							uploadFiles.push_back(path);
						}
					}
				}
			}
			else
			{
				path = dirEntry.path().string();
				std::replace(path.begin(), path.end(), '\\', '/');

				std::string correspondingCacheFile = path.substr(resultDirName.length());

				cacheFiles[correspondingCacheFile] = true; // This file is still present
			}
		}
	}

	// Now we check whether there are any files in the cache which do not exist anymore
	std::string fileNamePrefix = "Files/" + std::filesystem::path(baseProjectName).filename().string() + "/Result";
	for (const auto& file : cacheFiles)
	{
		if (!file.second)
		{
			// This file is not present anymore
			deletedFiles.push_back(fileNamePrefix + file.first);
		}
	}

	if (!includeResults)
	{
		deletedFiles.push_back(fileNamePrefix);
		deletedFiles.push_back("Files/" + std::filesystem::path(baseProjectName).filename().string());
	}

	return uploadFiles;
}

bool ProjectManager::fileContentDiffers(const std::string& file1, const std::string& file2)
{
	std::ifstream in1(file1, std::ios::binary);
	std::ifstream in2(file2, std::ios::binary);

	std::ifstream::pos_type size1, size2;

	size1 = in1.seekg(0, std::ifstream::end).tellg();
	in1.seekg(0, std::ifstream::beg);

	size2 = in2.seekg(0, std::ifstream::end).tellg();
	in2.seekg(0, std::ifstream::beg);

	if (size1 != size2)
	{
		// The file size is different
		return true;
	}

	static const size_t BLOCKSIZE = 4096;
	size_t remaining = size1;

	while (remaining)
	{
		char buffer1[BLOCKSIZE], buffer2[BLOCKSIZE];
		size_t size = std::min(BLOCKSIZE, remaining);

		in1.read(buffer1, size);
		in2.read(buffer2, size);

		if (0 != memcmp(buffer1, buffer2, size))
		{
			// The file content differs
			return true;
		}
			
		remaining -= size;
	}

	return false; // The files are identical
}


void ProjectManager::getCacheFileWriteTimes(const std::string& versionFolderName, std::map<std::string, std::filesystem::file_time_type>& cacheFileWriteTimes, std::map<std::string, bool> &cacheFiles)
{
	std::string resultDirName = versionFolderName + "/Result";

	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(resultDirName))
	{
		std::string path = dirEntry.path().string();
		std::replace(path.begin(), path.end(), '\\', '/');

		std::string cacheFile = path.substr(resultDirName.length());
		cacheFiles[cacheFile] = false;

		if (!dirEntry.is_directory())
		{
			cacheFileWriteTimes[cacheFile] = std::filesystem::last_write_time(path);
		}
	}
}

void ProjectManager::copyCacheFiles(const std::string& baseProjectName, const std::string &newVersion, const std::string &cacheFolderName, bool copyResults)
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

	if (copyResults)
	{
		if (!std::filesystem::create_directory(versionFolderName + "/Result"))
		{
			throw("Unable to create cache result folder: " + versionFolderName + "/Result");
		}
	}

	// Now copy all upload files to the cache version folder
	std::string cstFileName = baseProjectName + ".cst";
	std::string resultFolderName = baseProjectName + "/Result";

	std::replace(cstFileName.begin(), cstFileName.end(), '\\', '/');
	std::replace(resultFolderName.begin(), resultFolderName.end(), '\\', '/');

	try
	{
		copyFile(cstFileName, versionFolderName);

		if (copyResults)
		{
			copyDirectory(resultFolderName, versionFolderName + "/Result");
		}
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

	DataBase::instance().setWritingQueueEnabled(true);

	int fileCount = 0;
	int lastPercent = 15;

	ot::EntityCallbackBase::CallbackFlags callbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection |
		ot::EntityCallbackBase::Callback::DataNotify
	);

	for (const auto& file : uploadFileList)
	{
		ot::UID dataEntityID = entityIDList.front(); entityIDList.pop_front();
		ot::UID fileEntityID = entityIDList.front(); entityIDList.pop_front();

		ot::UID dataVersion = entityVersionList.front(); entityVersionList.pop_front();
		ot::UID fileVersion = entityVersionList.front(); entityVersionList.pop_front();

		// add the file to the list of modified files
		newOrModifiedFiles[file] = std::pair<ot::UID, ot::UID>(fileEntityID, fileVersion);
		dependentDataFiles[file] = std::pair<ot::UID, ot::UID>(dataEntityID, dataVersion);

		// upload the binary data entity
		EntityBinaryData *dataEntity = new EntityBinaryData(dataEntityID, nullptr, nullptr, nullptr);

		std::ifstream dataFile(file, std::ios::binary | std::ios::ate);

		std::streampos size = dataFile.tellg();

		char *memBlock = new char[size];
		dataFile.seekg(0, std::ios::beg);
		dataFile.read(memBlock, size);
		dataFile.close();

		dataSize = size;

		dataEntity->setData(memBlock, size);
		dataEntity->storeToDataBase(dataVersion);

		delete dataEntity;
		dataEntity = nullptr;

		delete[] memBlock;
		memBlock = nullptr;

		// Upload the file entity
		EntityFile* fileEntity = new EntityFile(fileEntityID, nullptr, nullptr, nullptr);
		fileEntity->registerCallbacks(callbacks, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

		std::string pathName = file.substr(projectRoot.size() + 1);
		std::filesystem::path filePath(pathName);

		fileEntity->setName("Files/" + pathName);
		fileEntity->setFileProperties(pathName, filePath.filename().string(), "Absolute");
		fileEntity->setDataEntity(dataEntityID, dataVersion);
		fileEntity->setEditable(false);

		fileEntity->storeToDataBase(fileVersion);

		delete fileEntity;
		fileEntity = nullptr;

		if (dataSize > 100000000)
		{
			// We have more than 100 MB since the last store
			DataBase::instance().flushWritingQueue();
			dataSize = 0;
		}

		int percent = (int)(50.0 * fileCount / uploadFileList.size() + 15.0);
		if (percent > lastPercent)
		{
			ot::WindowAPI::setProgressBarValue(percent);
			lastPercent = percent;
		}
		fileCount++;
	}

	DataBase::instance().setWritingQueueEnabled(false);

	ot::WindowAPI::setProgressBarValue(70);
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

	for (const auto& item : newOrModifiedFiles)
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
	std::string tmp;
	ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
}

void ProjectManager::sendUnitsInformation(const std::string &projectRoot)
{
	std::string fileContent;
	readFileContent(projectRoot + "/Temp/Upload/units.info", fileContent);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_UNITS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(fileContent, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	std::string tmp;
	ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
}

void ProjectManager::sendMaterialInformation(const std::string& projectRoot)
{
	std::string fileContent;
	readFileContent(projectRoot + "/Temp/Upload/material.info", fileContent);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_MATERIALS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(fileContent, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	std::string tmp;
	ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
}

void ProjectManager::sendParameterInformation(const std::string& projectRoot)
{
	std::string fileContent;
	readFileContent(projectRoot + "/Temp/Upload/parameters.info", fileContent);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_PARAMETERS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(fileContent, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	std::string tmp;
	ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
}

void ProjectManager::sendHistoryInformation(const std::string& projectRoot)
{
	std::string fileContent;
	readFileContent(projectRoot + "/Model/3D/Model.mod", fileContent);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_HISTORY, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(fileContent, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	std::string tmp;
	ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
}

void ProjectManager::sendShapeInformationAndTriangulation(const std::string& projectRoot, InfoFileManager &infoFileManager)
{
	std::string fileContent;
	readFileContent(projectRoot + "/Temp/Upload/shape.info", fileContent);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_SHAPEINFO, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(fileContent, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	std::string tmp;
	ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);

	// Read the list of shape names and assign them the index of the STL file
	std::map<std::string, int> allShapesMap = determineAllShapes(std::stringstream(fileContent));
	 
	// Now send the modified triangulations (one by one)
	sendTriangulations(projectRoot, allShapesMap, infoFileManager);
}

void ProjectManager::sendTriangulations(const std::string& projectRoot, std::map<std::string, int> trianglesMap, InfoFileManager& infoFileManager)
{
	std::list<std::string> shapeNames;
	std::list<std::string> shapeTriangles;
	std::list<std::string> shapeHash;

	// We combine potentially several triangulations into a single message in order to avoid sending an excessive number of 
	// messages in case of many small objects.
	size_t dataSize = 0;

	int shapeCount = 0;
	int lastPercent = 70;

	for (const auto& shape : trianglesMap)
	{
		std::string fileContent;
		readFileContent(projectRoot + "/Temp/Upload/stl" + std::to_string(shape.second) + ".stl", fileContent);

		std::string hash = calculateHash(fileContent);

		if (infoFileManager.getTriangleHash(shape.first) != hash)
		{
			// The hash value is different -> sending the shape is required

			shapeNames.push_back(shape.first);
			shapeTriangles.push_back(fileContent);
			shapeHash.push_back(hash);

			dataSize += (shape.first.size() + fileContent.size() + shapeHash.size());

			if (dataSize > 10000000)
			{
				sendTriangleLists(shapeNames, shapeTriangles, shapeHash);
				dataSize = 0;
				shapeNames.clear();
				shapeTriangles.clear();
			}
		}

		int percent = (int)(8.0 * shapeCount / trianglesMap.size() + 70.0);
		if (percent > lastPercent)
		{
			ot::WindowAPI::setProgressBarValue(percent);
			lastPercent = percent;
		}
		shapeCount++;
	}

	if (dataSize > 0)
	{
		sendTriangleLists(shapeNames, shapeTriangles, shapeHash);
	}

	ot::WindowAPI::setProgressBarValue(80);
}

std::string ProjectManager::calculateHash(const std::string& fileContent)
{
	// First, we process the file and extract each pair of three vertices in one string
	std::stringstream stlData(fileContent);

	int vertexID = 0;
	std::string vertexData;
	vertexData.reserve(320);

	std::list<std::string> vertexList;

	while (!stlData.eof())
	{
		std::string line;
		std::getline(stlData, line);

		size_t index = line.find("vertex");

		if (index != std::string::npos)
		{
			// This line contains a vertex
			vertexData += line.substr(index+6);
			vertexID++;

			if (vertexID == 3)
			{
				// We have collected three vertices -> next triangle
				vertexList.push_back(vertexData);
				vertexData.clear();
				vertexData.reserve(320);
				vertexID = 0;
			}
		}
	}

	// Now we need to sort the list with the vertex data, since the ordering of the triangles may differ from time to time
	vertexList.sort();

	// Now get the list into a string
	std::stringstream data;
	for (const auto& item : vertexList)
	{
		data << item;
	}

	// And finally calculate the hash
	QCryptographicHash hashCalculator(QCryptographicHash::Md5);
	hashCalculator.addData(data.str().c_str(), data.str().size());
	
	std::string hashValue = hashCalculator.result().toHex().toStdString();

	return hashValue;
}

void ProjectManager::sendTriangleLists(std::list<std::string>& shapeNames, std::list<std::string>& shapeTriangles, std::list<std::string>& shapeHash)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_TRIANGLES, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonArray(shapeNames, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonArray(shapeTriangles, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Hash, ot::JsonArray(shapeHash, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	std::string tmp;
	ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
}

std::map<std::string, int> ProjectManager::determineAllShapes(std::stringstream fileContent)
{
	std::map<std::string, int> shapeNameToIdMap;

	int id = 0; 

	while (!fileContent.eof())
	{
		std::string name, material;

		std::getline(fileContent, name);
		std::getline(fileContent, material);

		if (!name.empty() && !material.empty())
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

			copyFile(cstCacheFileName, baseProjectName + ".cst");

			if (std::filesystem::is_directory(cacheDirectory + "/Result"))
			{
				copyDirectory(cacheDirectory + "/Result", baseProjectName + "/Result");
			}

			return true;
		}
	}
	catch (std::exception)
	{

	}

	return false;
}

void ProjectManager::copyFile(const std::string& sourceFile, const std::string& destFile)
{
	std::filesystem::copy(sourceFile, destFile, std::filesystem::copy_options::update_existing);

	// Now apply the time stamp from the original file
	auto lastWrite = std::filesystem::last_write_time(sourceFile);
	std::filesystem::last_write_time(destFile, lastWrite);
}

void ProjectManager::copyDirectory(const std::string& sourceDir, const std::string& destDir)
{
	std::filesystem::copy(sourceDir, destDir, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);

	// Now apply the time stamps from the original files
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(sourceDir))
	{
		auto lastWrite = std::filesystem::last_write_time(dirEntry.path());
		std::string relativeFileName = dirEntry.path().string().substr(sourceDir.length());
		std::string destFileName = destDir + relativeFileName;
		std::filesystem::last_write_time(destFileName, lastWrite);
	}
}

bool ProjectManager::checkValidLocalFile(std::string fileName, std::string projectName, bool ensureProjectExists, std::string &errorMessage)
{
	errorMessage.clear();

	std::string baseProjectName;

	try
	{
		baseProjectName = getBaseProjectName(fileName);
	}
	catch (std::string)
	{
		// We were unable to extract the project name from the file -> invalid file name
		errorMessage = "The file path is invalid";
		return false;
	}

	// Set the name of the cache folder
	std::string cacheFolderName = baseProjectName + ".cache";

	bool cstFileExists = std::filesystem::is_regular_file(fileName);
	if (!cstFileExists && ensureProjectExists)
	{
		errorMessage = "The specified file does not exist.";
		return false; // We need an existing project, but this one did not exist
	}

	// Now we check whether the associated directory is ok
	if (cstFileExists)
	{
		// The directory also needs to exist in this case
		if (!std::filesystem::is_directory(baseProjectName))
		{
			errorMessage = "The project exists, but does not have an associated project folder. \n"
						   "Please open the project in CST Studio Suite to extract the project's content.";
			return false;
		}
	}
	else
	{
		// The directory must not exist in this case (and the cache folder must not exist as well
		if (std::filesystem::is_directory(baseProjectName))
		{
			errorMessage = "The project file does not exist, but there is a folder with the same name. \n"
				           "This would cause problems when the CST Studio Suite project is opened.";

			return false;
		}

		if (std::filesystem::is_directory(cacheFolderName))
		{
			errorMessage = "The project file does not exist, but there is a cache folder with the same name. \n"
				           "This would cause problems when the CST Studio Suite project is opened.";

			return false;
		}

		// We have a new project name
		return true;
	}

	// In this case, we have an existing project
	if (!std::filesystem::is_directory(cacheFolderName))
	{
		errorMessage = "The project exists, but does not have an associated cache folder. \n"
			           "This project does not seem to be connected to version control.";

		return false;
	}

	// Load the version data
	VersionFile version(cacheFolderName + "\\version.info");
	version.read();

	// We need to ensure that the local cache diretory actually belongs to the open project.
	if (version.getProjectName() != projectName)
	{
		errorMessage = "Inconcistent cache information: The project cache information does not belong to the associated project.\n";

		return false;
	}

	return true;
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

	DataBase::instance().prefetchDocumentsFromStorage(prefetchIDs);

	bool success = true;

	// Here we have a progress range from 20-90
	int entityCount = 0;
	int lastPercent = 20;
	for (const auto& entity : prefetchIDs)
	{
		// Download a single cache file
		success &= downloadFile(cacheFolderVersion, entity.first, entity.second);

		int percent = (int)(70.0 * entityCount / prefetchIDs.size() + 20.0);
		if (percent > lastPercent)
		{
			ot::WindowAPI::setProgressBarValue(percent);
			lastPercent = percent;
		}

		entityCount++;
	}

	if (!success)
	{
		ot::WindowAPI::showErrorPrompt("Studio Suite Error", "The CST Studio Suite project could not be restored to version " + version + ": Some file could not be downloaded.");
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::lockUI(false);

		return;
	}
	
	// Finally restore the project from the cache
	if (!restoreFromCache(baseProjectName, cacheFolderName, version))
	{
		ot::WindowAPI::showErrorPrompt("Studio Suite Error", "The CST Studio Suite project could not be restored to version " + version + ": The cache data seems to be incomplete.");
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::lockUI(false);
		return;
	}

	// We have successfully restored the project data, so we can now open the project
	StudioConnector studioObject;
	studioObject.openProject(fileName);

	// Update the version file
	writeVersionFile(baseProjectName, projectName, version, cacheFolderName);

	ot::WindowAPI::setProgressBarVisibility("", false, false);
	ot::WindowAPI::lockUI(false);
	ot::WindowAPI::showInfoPrompt("Restore Project", "The CST Studio Suite project has been restored successfully to version " + version + ".");
}

bool ProjectManager::downloadFile(const std::string &cacheFolderVersion, ot::UID entityID, ot::UID version)
{
	bool success = true;

	EntityFile* fileEntity = dynamic_cast<EntityFile*> (DataBase::instance().getEntityFromEntityIDandVersion(entityID, version));

	if (fileEntity != nullptr)
	{
		size_t size = fileEntity->getDataEntity()->getData().size();

		std::string entityPath = fileEntity->getPath();
		size_t index = entityPath.find('/');

		std::string fileName = cacheFolderVersion + "/" + entityPath.substr(index+1);

		std::filesystem::path path(fileName);
		std::string parentFolder = path.parent_path().string();

		std::filesystem::create_directories(parentFolder);

		std::ofstream dataFile(fileName, std::ios::binary);
		dataFile.write(fileEntity->getDataEntity()->getData().data(), size);
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
