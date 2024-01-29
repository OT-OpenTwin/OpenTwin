#include "StudioSuiteConnector/ProjectManager.h"
#include "StudioSuiteConnector/ServiceConnector.h"
#include "StudioSuiteConnector/StudioConnector.h"
#include "StudioSuiteConnector/VersionFile.h"
#include "StudioSuiteConnector/ProgressInfo.h"

#include "OTCommunication/ActionTypes.h"

#include <filesystem>
#include <algorithm>

void ProjectManager::setStudioServiceData(const std::string& studioSuiteServiceURL, QObject* mainObject)
{
	ServiceConnector::getInstance().setServiceURL(studioSuiteServiceURL);
	ServiceConnector::getInstance().setMainObject(mainObject);

	ProgressInfo::getInstance().setMainObject(mainObject);
}

void ProjectManager::importProject(const std::string &fileName, const std::string &projectName)
{
	ProgressInfoHelper progress;
	progress.getProgressInfo().setProgressState(true, "Importing project", false);
	progress.getProgressInfo().setProgressValue(0);

	// Determine the base project name (without .cst extension)
	std::string baseProjectName = getBaseProjectName(fileName);

	// Create the cache folder
	std::string cacheFolderName = createCacheFolder(baseProjectName);

	// Open the cst project in a studio suite instance
	StudioConnector::getInstance().openProject(fileName);

	// Now save the project (if needed) and extract the data 
	StudioConnector::getInstance().saveProject();
	StudioConnector::getInstance().extractInformation();

	// Get the files to be uploaded
	std::list<std::string> uploadFileList = determineUploadFiles(baseProjectName);

	// Retrieve UIDs and Version IDs
	std::list<ot::UID> entityIDList, entityVersionList;
	retrieveEntityIDsAndVersions(uploadFileList.size(), entityIDList, entityVersionList);

	progress.getProgressInfo().setProgressValue(10);

	// Upload files
	uploadFiles(uploadFileList, entityIDList, entityVersionList);

	// Create the new version
	std::string newVersion = commitNewVersion();

	progress.getProgressInfo().setProgressValue(70);

	// Copy the files to the cache directory
	copyCacheFiles(baseProjectName, newVersion, cacheFolderName);

	// Store version information
	writeVersionFile(projectName, newVersion, cacheFolderName);

	progress.getProgressInfo().setProgressValue(100);
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
		path = dirEntry.path().string();
		std::replace(path.begin(), path.end(), '\\', '/');

		uploadFiles.push_back(path);
	}

	return uploadFiles;
}

void ProjectManager::retrieveEntityIDsAndVersions(size_t count, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList)
{
	entityIDList.clear();
	entityVersionList.clear();

	ot::JsonDocument commandDoc;
	commandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SS_GetIDList, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_COUNT, ot::JsonValue((int64_t)count), commandDoc.GetAllocator());

	std::string response;
	ServiceConnector::getInstance().sendExecuteRequest(commandDoc, response);

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	rapidjson::Value entityIDArray = responseDoc[OT_ACTION_PARAM_MODEL_EntityIDList].GetArray();
	rapidjson::Value versionArray = responseDoc[OT_ACTION_PARAM_MODEL_EntityVersionList].GetArray();

	size_t numberEntityID = entityIDArray.Size();
	size_t numberVersion = versionArray.Size();
	assert(numberEntityID == numberVersion);

	for (size_t i = 0; i < numberEntityID; i++)
	{
		entityIDList.push_back(entityIDArray[i].GetInt64());
	}

	for (size_t i = 0; i < numberVersion; i++)
	{
		entityVersionList.push_back(versionArray[i].GetInt64());
	}
}

void ProjectManager::uploadFiles(std::list<std::string> &uploadFileList, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList)
{

}

std::string ProjectManager::commitNewVersion(void)
{
	return "1.0.0";
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

		std::filesystem::copy_options options;
		options |= std::filesystem::copy_options::recursive;

		std::filesystem::copy(resultFolderName, versionFolderName + "/Result", options);
	}
	catch (std::exception)
	{
		throw("Unable to copy data to cache: " + versionFolderName);
	}
}

void ProjectManager::writeVersionFile(const std::string& projectName, const std::string& newVersion, const std::string& cacheFolderName)
{
	VersionFile version(std::string(cacheFolderName + "/version.info"));

	version.write(projectName, newVersion);
}


