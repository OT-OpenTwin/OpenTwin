// @otlicense

#include "LTSpiceConnector/ProjectManager.h"
#include "LTSpiceConnector/VersionFile.h"

#include "OTCommunication/ActionTypes.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"
#include "OTFrontendConnectorAPI/CommunicationAPI.h"

#include "EntityBinaryData.h"
#include "EntityFile.h"
#include "DataBase.h"

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

void ProjectManager::setLTSpiceServiceData(const std::string& ltSpiceServiceURL)
{
	ot::CommunicationAPI::setDefaultConnectorServiceUrl(ltSpiceServiceURL);
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

		ot::WindowAPI::setProgressBarVisibility("Importing Project", true, false);
		ot::WindowAPI::setProgressBarValue(0);

		// Determine the base project name (without .asc extension)
		baseProjectName = getBaseProjectName(fileName);

		// Create the cache folder
		cacheFolderName = createCacheFolder(baseProjectName);

		// Get the files to be uploaded
		uploadFileList = determineUploadFiles(baseProjectName, includeResults);

		ot::WindowAPI::setProgressBarVisibility("Importing Project", true, false);
		ot::WindowAPI::setProgressBarValue(15);

		std::string hostName = QHostInfo::localHostName().toStdString();

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_UPLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COUNT, 2 * uploadFileList.size(), doc.GetAllocator());  // We need ids for the data entities and the file entities
		doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonString(fileName, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());

		std::string tmp;
		ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
	}
	catch (std::string &error)
	{
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::showErrorPrompt("Import LT Spice Project", error);
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

		ot::WindowAPI::setProgressBarVisibility("Committing Project", true, false);
		ot::WindowAPI::setProgressBarValue(0);

		// Determine the base project name (without .asc extension)
		baseProjectName = getBaseProjectName(fileName);

		// Set the name of the cache folder
		cacheFolderName = baseProjectName + ".cache";

		// Get the files to be uploaded
		uploadFileList = determineUploadFiles(baseProjectName, includeResults);

		ot::WindowAPI::setProgressBarVisibility("Committing Project", true, false);
		ot::WindowAPI::setProgressBarValue(15);

		std::string hostName = QHostInfo::localHostName().toStdString();

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_UPLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COUNT, 2 * uploadFileList.size(), doc.GetAllocator());  // We need ids for the data entities and the file entities
		doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonString(fileName, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());

		std::string tmp;
		ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
	}
	catch (std::string& error)
	{
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::showErrorPrompt("Commit LT Spice Project", error);
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

		// Determine the base project name (without .asc extension)
		baseProjectName = getBaseProjectName(fileName);

		// Set the name of the cache folder
		cacheFolderName = baseProjectName + ".cache";

		ot::WindowAPI::setProgressBarVisibility("Getting Project", true, false);
		ot::WindowAPI::setProgressBarValue(10);

		// Delete project files
		deleteLocalProjectFiles(baseProjectName);

		// Check whether version is in the cache
		if (!restoreFromCache(baseProjectName, cacheFolderName, version))
		{
			// The project was not found in the cache. Therefore, the files need to be retrieved from the repo
			ot::WindowAPI::setProgressBarValue(20);

			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_DOWNLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());

			std::string tmp;
			ot::CommunicationAPI::sendExecute(doc.toJson(), tmp);
		}
		else
		{
			ot::WindowAPI::setProgressBarValue(90);

			// Update the version file
			writeVersionFile(baseProjectName, projectName, version, cacheFolderName);

			ot::WindowAPI::setProgressBarVisibility("", false, false);
			ot::WindowAPI::lockUI(false);

			ot::WindowAPI::showInfoPrompt("Get LT Spice Project", "The LTSpice project has been restored successfully to version " + version + ".");
		}
	}
	catch (std::string& error)
	{
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::showErrorPrompt("Get LT Spice Project", error);
		ot::WindowAPI::lockUI(false);
	}
}

void ProjectManager::uploadFiles(std::list<ot::UID> &entityIDList, std::list<ot::UID> &entityVersionList)
{
	ot::WindowAPI::setProgressBarValue(15);

	try
	{
		// Determine the project root folder
		std::filesystem::path projectPath(baseProjectName);
		std::string projectRoot = projectPath.parent_path().string();

		// Upload files (progress range 15-90)
		uploadFiles(projectRoot, uploadFileList, entityIDList, entityVersionList);

		// Create the new version
		commitNewVersion(changeMessage);

		ot::WindowAPI::setProgressBarValue(90);
	}
	catch (std::string& error)
	{
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::showErrorPrompt("Upload LT Spice Project Files", error);
		ot::WindowAPI::lockUI(false);
	}
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
			ot::WindowAPI::showInfoPrompt("Import LT Spice Project", "The LTSpice project has been imported successfully.");
		}
		else if (currentOperation == OPERATION_COMMIT)
		{
			ot::WindowAPI::showInfoPrompt("Commit LT Spice Project", "The LTSpice project has been commited successfully (version: " + newVersion + ").");
		}
		else
		{
			assert(0); // Unexpected operation
		}
	}
	catch (std::string& error)
	{
		ot::WindowAPI::showErrorPrompt("Store LT Spice Project Files", error);
	}

	ot::WindowAPI::setProgressBarVisibility("", false, false);
	ot::WindowAPI::lockUI(false);
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
				}
			}
		}
	}

	// Now we check whether there are any files in the cache which do not exist anymore
	for (const auto& file : cacheFiles)
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

	if (!copyResults)
	{
		try
		{
			copyFile(baseProjectName + ".asc", versionFolderName);
		}
		catch (std::exception& error)
		{
			throw("Unable to copy data to cache (" + std::string(error.what()) + ") : " + versionFolderName);
		}
	}
	else
	{
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
		EntityBinaryData *dataEntity = new EntityBinaryData(dataEntityID, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

		std::ifstream dataFile(file, std::ios::binary | std::ios::ate);

		std::streampos size = dataFile.tellg();
		dataFile.seekg(0, std::ios::beg);

		char *memBlock = new char[size];
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
		EntityFile* fileEntity = new EntityFile(fileEntityID, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

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

		int percent = (int)(75.0 * fileCount / uploadFileList.size() + 15.0);
		if (percent > lastPercent)
		{
			ot::WindowAPI::setProgressBarValue(percent);
			lastPercent = percent;
		}
		fileCount++;
	}

	DataBase::instance().setWritingQueueEnabled(false);

	ot::WindowAPI::setProgressBarValue(90);
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
	for (const auto& entityID : entityIDList)
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
		ot::WindowAPI::showErrorPrompt("Get LT Spice Project", "The LTSpice project could not be restored to version " + version + ".");
		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::lockUI(false);

		return;
	}
	
	// Finally restore the project from the cache
	if (!restoreFromCache(baseProjectName, cacheFolderName, version))
	{
		ot::WindowAPI::showErrorPrompt("Get LT Spice Project", "The LTSpice project could not be restored to version " + version + ".");

		ot::WindowAPI::setProgressBarVisibility("", false, false);
		ot::WindowAPI::lockUI(false);

		return;
	}

	// Update the version file
	writeVersionFile(baseProjectName, projectName, version, cacheFolderName);

	ot::WindowAPI::setProgressBarVisibility("", false, false);
	ot::WindowAPI::lockUI(false);

	ot::WindowAPI::showInfoPrompt("Get LT Spice Project", "The LTSpice project has been restored successfully to version " + version + ".");
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
