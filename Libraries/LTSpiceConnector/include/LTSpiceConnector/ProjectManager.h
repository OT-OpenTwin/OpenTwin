#pragma once

#include <string>
#include <map>
#include <list>
#include <sstream>
#include <filesystem>

#include "OTCore/CoreTypes.h"

#include "LTSpiceConnector/InfoFileManager.h"

#include <qobject.h>

class ProjectManager
{
public:
    static ProjectManager& getInstance()
    {
        static ProjectManager instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    void openProject();
    void setLTSpiceServiceData(const std::string& ltSpiceServiceURL);

    void importProject(const std::string& fileName, const std::string& prjName, const std::string& changeMessage, bool incResults);
    std::string getCurrentVersion(const std::string& fileName, const std::string& prjName);
    void commitProject(const std::string& fileName, const std::string& prjName, const std::string& changeComment, bool incResults);
    void getProject(const std::string& fileName, const std::string& prjName, const std::string& version);
    void uploadFiles(std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList);
    void downloadFiles(const std::string& fileName, const std::string& projectName, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList, const std::string& version);
    void copyFiles(const std::string& newVersion);
    std::string getLocalFileName() { return localProjectFileName; }
    void setLocalFileName(std::string fileName);
    bool checkValidLocalFile(std::string fileName, std::string projectName, bool ensureProjectExists, std::string& errorMessage);

private:
    enum operationType { OPERATION_NONE, OPERATION_IMPORT, OPERATION_GET, OPERATION_COMMIT};

    ProjectManager() : currentOperation(OPERATION_NONE), includeResults(false) {};
    ~ProjectManager() {};

    std::string                getBaseProjectName(const std::string& ltsFileName);
    std::string                createCacheFolder(const std::string& baseProjectName);
    std::list<std::string>     determineUploadFiles(const std::string& baseProjectName, bool incResults);
    void                       uploadFiles(const std::string& projectRoot, std::list<std::string>& uploadFileList, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList);
    void                       commitNewVersion(const std::string& changeMessage);
    void                       copyCacheFiles(const std::string& baseProjectName, const std::string& newVersion, const std::string& cacheFolderName, bool copyResults);
    void                       writeVersionFile(const std::string& baseProjectName, const std::string &projectName, const std::string& newVersion, const std::string& cacheFolderName);
    void                       deleteLocalProjectFiles(const std::string& baseProjectName);
    bool                       restoreFromCache(const std::string& baseProjectName, const std::string& cacheFolderName, const std::string& version);
    bool                       downloadFile(const std::string& cacheFolderVersion, ot::UID entityID, ot::UID version);
    void                       copyFile(const std::string& sourceFile, const std::string& destFile);
    void                       copyDirectory(const std::string& sourceDir, const std::string& destDir);
    void                       getCacheFileWriteTimes(const std::string& cacheFolderName, std::map<std::string, std::filesystem::file_time_type>& cacheFileWriteTimes, std::map<std::string, bool>& cacheFiles);
    bool                       fileContentDiffers(const std::string& file1, const std::string& file2);

    std::list<std::string> uploadFileList;
    std::string projectName;
    std::string baseProjectName;
    std::string cacheFolderName;
    std::map<std::string, std::pair<ot::UID, ot::UID>> newOrModifiedFiles;
    std::map<std::string, std::pair<ot::UID, ot::UID>> dependentDataFiles;
    std::list<std::string> deletedFiles;
    std::string changeMessage;
    bool includeResults;
    operationType currentOperation;
    std::string localProjectFileName;
};
