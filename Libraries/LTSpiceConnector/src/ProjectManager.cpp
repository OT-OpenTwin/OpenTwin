#include "LTSpiceConnector/ProjectManager.h"
#include "LTSpiceConnector/ServiceConnector.h"
#include "LTSpiceConnector/VersionFile.h"
#include "LTSpiceConnector/ProgressInfo.h"
#include "LTSpiceConnector/Result1DFileManager.h"

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

void ProjectManager::setLTSpiceServiceData(const std::string& ltSpiceServiceURL, QObject* mainObject)
{
	ServiceConnector::getInstance().setServiceURL(ltSpiceServiceURL);
	ServiceConnector::getInstance().setMainObject(mainObject);

	ProgressInfo::getInstance().setMainObject(mainObject);
}

void ProjectManager::importProject(const std::string& fileName, const std::string& prjName, const std::string &message, bool incResults)
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

		ProgressInfo::getInstance().setProgressState(true, "Importing project", false);
		ProgressInfo::getInstance().setProgressValue(0);

		// Determine the base project name (without .asc extension)
		baseProjectName = getBaseProjectName(fileName);

		// Create the cache folder
		cacheFolderName = createCacheFolder(baseProjectName);

		// Get the files to be uploaded
		uploadFileList = determineUploadFiles(baseProjectName, includeResults);

		ProgressInfo::getInstance().setProgressState(true, "Importing project", false);
		ProgressInfo::getInstance().setProgressValue(15);

		std::string hostName = QHostInfo::localHostName().toStdString();

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_UPLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COUNT, 2 * uploadFileList.size(), doc.GetAllocator());  // We need ids for the data entities and the file entities
		doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonString(fileName, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());

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

void ProjectManager::commitProject(const std::string& fileName, const std::string& prjName, const std::string& changeComment, bool incResults)
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

		ProgressInfo::getInstance().setProgressState(true, "Committing project", false);
		ProgressInfo::getInstance().setProgressValue(0);

		// Determine the base project name (without .asc extension)
		baseProjectName = getBaseProjectName(fileName);

		// Set the name of the cache folder
		cacheFolderName = baseProjectName + ".cache";

		// Get the files to be uploaded
		uploadFileList = determineUploadFiles(baseProjectName, includeResults);

		ProgressInfo::getInstance().setProgressState(true, "Committing project", false);
		ProgressInfo::getInstance().setProgressValue(15);

		std::string hostName = QHostInfo::localHostName().toStdString();

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_UPLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COUNT, 2 * uploadFileList.size(), doc.GetAllocator());  // We need ids for the data entities and the file entities
		doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonString(fileName, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());

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

		// Determine the base project name (without .asc extension)
		baseProjectName = getBaseProjectName(fileName);

		// Set the name of the cache folder
		cacheFolderName = baseProjectName + ".cache";

		ProgressInfo::getInstance().setProgressState(true, "Getting project", false);
		ProgressInfo::getInstance().setProgressValue(10);

		// Delete project files
		deleteLocalProjectFiles(baseProjectName);

		// Check whether version is in the cache
		if (!restoreFromCache(baseProjectName, cacheFolderName, version))
		{
			// The project was not found in the cache. Therefore, the files need to be retrieved from the repo
			ProgressInfo::getInstance().setProgressValue(20);

			ot::JsonDocument doc;
			// ******** NOT YET IMPLEMENTED ****
			assert(0);
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_DOWNLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());

			ServiceConnector::getInstance().sendExecuteRequest(doc);
		}
		else
		{
			ProgressInfo::getInstance().setProgressValue(90);

			// Update the version file
			writeVersionFile(baseProjectName, projectName, version, cacheFolderName);

			ProgressInfo::getInstance().setProgressState(false, "", false);
			ProgressInfo::getInstance().unlockGui();

			ProgressInfo::getInstance().showInformation("The LTSpice project has been restored successfully to version " + version + ".");
		}
	}
	catch (std::string& error)
	{
		ProgressInfo::getInstance().setProgressState(false, "", false);
		ProgressInfo::getInstance().showError(error);
		ProgressInfo::getInstance().unlockGui();
	}
}

void ProjectManager::uploadFiles(std::list<ot::UID> &entityIDList, std::list<ot::UID> &entityVersionList, ot::UID infoEntityID, ot::UID infoEntityVersion)
{
	ProgressInfo::getInstance().setProgressValue(15);

	try
	{
		// Determine the project root folder
		std::filesystem::path projectPath(baseProjectName);
		std::string projectRoot = projectPath.parent_path().string();

		// Load the hash information for the entities 
		InfoFileManager infoFileManager(infoEntityID, infoEntityVersion);

		// Upload files (progress range 15-90)
		uploadFiles(projectRoot, uploadFileList, entityIDList, entityVersionList);

		// Create the new version
		commitNewVersion(changeMessage);

		ProgressInfo::getInstance().setProgressValue(90);
	}
	catch (std::string& error)
	{
		ProgressInfo::getInstance().setProgressState(false, "", false);
		ProgressInfo::getInstance().unlockGui();
		ProgressInfo::getInstance().showError(error);
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

	bool appendData = false; // includeParametricResults;
	std::string dataContent;
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

			// Create a buffer for the compressed storage
			uLong compressedSize = compressBound((uLong)uncompressedDataLength);

			char* compressedData = new char[compressedSize];
			compress((Bytef*)compressedData, &compressedSize, (Bytef*)buffer.data(), uncompressedDataLength);

			buffer.clear();

			// Convert the binary to an encoded string
			int encoded_data_length = Base64encode_len(compressedSize);
			char* base64_string = new char[encoded_data_length];

			Base64encode(base64_string, compressedData, compressedSize); // "base64_string" is a then null terminated string that is an encoding of the binary data pointed to by "data"

			delete[] compressedData;
			compressedData = nullptr;

			dataContent = std::string(base64_string);

			delete[] base64_string;
			base64_string = nullptr;
		}
	}

	// Finally send the zip archive to the server (together with the information whether the data shall be replaced or added)
	// In case that we do not have any results or we do not want to add the results, the message is still sent, but with an empty content

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_RESULT1D, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_APPEND, appendData, doc.GetAllocator());  // We need ids for the data entities and the file entities
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(dataContent, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content_UncompressedDataLength, rapidjson::Value(uncompressedDataLength), doc.GetAllocator());

	ServiceConnector::getInstance().sendExecuteRequest(doc);
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

		ProgressInfo::getInstance().setProgressValue(100);

		if (currentOperation == OPERATION_IMPORT)
		{
			ProgressInfo::getInstance().showInformation("The LTSpice project has been imported successfully.");
		}
		else if (currentOperation == OPERATION_COMMIT)
		{
			ProgressInfo::getInstance().showInformation("The LTSpice project has been commited successfully (version: " + newVersion + ").");
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

std::string ProjectManager::getBaseProjectName(const std::string& ltsFileName)
{
	size_t index = ltsFileName.rfind('.');
	if (index == std::string::npos) throw("Unable to determine project base name from LTSpice file name: " + ltsFileName);

	return ltsFileName.substr(0, index);
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

	path = baseProjectName + ".asc";
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
		std::string projectDirName = std::filesystem::path(baseProjectName + ".asc").parent_path().string();
		std::replace(projectDirName.begin(), projectDirName.end(), '\\', '/');

		std::string cacheResultDirName = versionFolderName;

		for (const auto& dirEntry : std::filesystem::directory_iterator(projectDirName))
		{
			if (!dirEntry.is_directory())
			{
				path = dirEntry.path().string();
				std::replace(path.begin(), path.end(), '\\', '/');

				std::string fileExtension = dirEntry.path().extension().string();
				transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

				if (fileExtension != ".asc")
				{
					std::string filter = baseProjectName + ".";
					std::string firstPart = path.substr(0, filter.length());

					if (firstPart == filter)
					{
						// This file belongs to the project and is not the main schematic file
						std::string correspondingCacheFile = path.substr(projectDirName.length() + 1);

						if (cacheFiles.count(correspondingCacheFile) == 0)
						{
							// This file does not exist in the cache
							uploadFiles.push_back(path);
						}
						else
						{
							// This file exists in the cache
							cacheFiles[correspondingCacheFile] = true; // This file is still present

							auto cacheFileTimeStamp = cacheFileWriteTimes[correspondingCacheFile];
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
				}
			}
		}
	}

	// Now we check whether there are any files in the cache which do not exist anymore
	for (auto file : cacheFiles)
	{
		if (!file.second)
		{
			std::string fileExtension = std::filesystem::path(file.first).extension().string();
			transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);

			if (fileExtension != ".asc")
			{
				// This file is not present anymore
				deletedFiles.push_back("Files/" + file.first);
			}
		}
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
	std::string cacheDirName = versionFolderName;

	for (const auto& dirEntry : std::filesystem::directory_iterator(cacheDirName))
	{
		std::string path = dirEntry.path().string();
		std::replace(path.begin(), path.end(), '\\', '/');

		std::string cacheFile = path.substr(cacheDirName.length()+1);
		cacheFiles[cacheFile] = false;

		cacheFileWriteTimes[cacheFile] = std::filesystem::last_write_time(path);
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

	// Now copy all project files to the cache version folder
	std::string projectDirName = std::filesystem::path(baseProjectName + ".asc").parent_path().string();
	std::replace(projectDirName.begin(), projectDirName.end(), '\\', '/');

	for (const auto& dirEntry : std::filesystem::directory_iterator(projectDirName))
	{
		if (!dirEntry.is_directory())
		{
			std::string path = dirEntry.path().string();
			std::replace(path.begin(), path.end(), '\\', '/');

			std::string filter = baseProjectName + ".";
			std::string firstPart = path.substr(0, filter.length());

			if (firstPart == filter)
			{
				// This file belongs to the project 
				try
				{
					copyFile(path, versionFolderName);
				}
				catch (std::exception& error)
				{
					throw("Unable to copy data to cache (" + std::string(error.what()) + ") : " + versionFolderName);
				}
			}
		}
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

	int fileCount = 0;
	int lastPercent = 15;

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

		int percent = (int)(75.0 * fileCount / uploadFileList.size() + 15.0);
		if (percent > lastPercent)
		{
			ProgressInfo::getInstance().setProgressValue(percent);
			lastPercent = percent;
		}
		fileCount++;
	}

	DataBase::GetDataBase()->queueWriting(false);

	ProgressInfo::getInstance().setProgressValue(90);
}

void ProjectManager::commitNewVersion(const std::string &changeMessage)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_FILES_UPLOADED, doc.GetAllocator()), doc.GetAllocator());
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
	for (auto item : vertexList)
	{
		data << item;
	}

	// And finally calculate the hash
	QCryptographicHash hashCalculator(QCryptographicHash::Md5);
	hashCalculator.addData(data.str().c_str(), data.str().size());
	
	std::string hashValue = hashCalculator.result().toHex().toStdString();

	return hashValue;
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
	std::string projectDirName = std::filesystem::path(baseProjectName + ".asc").parent_path().string();
	std::replace(projectDirName.begin(), projectDirName.end(), '\\', '/');

	for (const auto& dirEntry : std::filesystem::directory_iterator(projectDirName))
	{
		if (!dirEntry.is_directory())
		{
			std::string path = dirEntry.path().string();
			std::replace(path.begin(), path.end(), '\\', '/');

			std::string filter = baseProjectName + ".";
			std::string firstPart = path.substr(0, filter.length());

			if (firstPart == filter)
			{
				// This file belongs to the project 
				try
				{
					std::remove(path.c_str());
				}
				catch (std::exception& error)
				{
					throw("Unable to remove local project file (" + std::string(error.what()) + ") : " + path);
				}
			}
		}
	}
}

bool ProjectManager::restoreFromCache(const std::string& baseProjectName, const std::string& cacheFolderName, const std::string& version)
{
	std::string projectDirName = std::filesystem::path(baseProjectName + ".asc").parent_path().string();
	std::replace(projectDirName.begin(), projectDirName.end(), '\\', '/');

	try
	{
		std::string cacheDirectory = cacheFolderName + "/" + version;

		// Check whether current version is in cache
		if (std::filesystem::is_directory(cacheDirectory))
		{
			for (const auto& dirEntry : std::filesystem::directory_iterator(cacheDirectory))
			{
				if (!dirEntry.is_directory())
				{
					std::string path = dirEntry.path().string();
					std::replace(path.begin(), path.end(), '\\', '/');

					try
					{
						copyFile(path, projectDirName);
					}
					catch (std::exception& error)
					{
						throw("Unable to copy data from cache (" + std::string(error.what()) + ") : " + path);
					}

				}
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

	bool ltSpiceFileExists = std::filesystem::is_regular_file(fileName);
	if (!ltSpiceFileExists && ensureProjectExists)
	{
		errorMessage = "The specified file does not exist.";
		return false; // We need an existing project, but this one did not exist
	}

	// Now we check whether the associated directory is ok
	if (!ltSpiceFileExists)
	{
		// The cache folder must not exist (if the file does not exist)
		if (std::filesystem::is_directory(cacheFolderName))
		{
			errorMessage = "The project file does not exist, but there is a cache folder with the same name. \n"
				           "This would cause problems when the LTSpice project is opened.";

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

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIDs);

	bool success = true;

	// Here we have a progress range from 20-90
	int entityCount = 0;
	int lastPercent = 20;
	for (auto entity : prefetchIDs)
	{
		// Download a single cache file
		success &= downloadFile(cacheFolderVersion, entity.first, entity.second);

		int percent = (int)(70.0 * entityCount / prefetchIDs.size() + 20.0);
		if (percent > lastPercent)
		{
			ProgressInfo::getInstance().setProgressValue(percent);
			lastPercent = percent;
		}

		entityCount++;
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
