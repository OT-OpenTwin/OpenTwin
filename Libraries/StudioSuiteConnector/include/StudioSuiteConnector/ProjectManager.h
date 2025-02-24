#pragma once

#include <string>
#include <map>
#include <list>
#include <sstream>
#include <filesystem>

#include "OTCore/CoreTypes.h"

#include "StudioSuiteConnector/InfoFileManager.h"
#include "StudioSuiteConnector/Result1DFileManager.h"

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
    void setStudioServiceData(const std::string& studioSuiteServiceURL, QObject* mainObject);

    void importProject(const std::string& fileName, const std::string& prjName, const std::string& changeMessage, bool incResults, bool incParametricResults);
    std::string getCurrentVersion(const std::string& fileName, const std::string& prjName);
    void commitProject(const std::string& fileName, const std::string& prjName, const std::string& changeComment, bool incResults, bool incParametricResults);
    void getProject(const std::string& fileName, const std::string& prjName, const std::string& version);
    void uploadFiles(std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList, ot::UID infoEntityID, ot::UID infoEntityVersion);
    void downloadFiles(const std::string& fileName, const std::string& projectName, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList, const std::string& version);
    void copyFiles(const std::string& newVersion);
    std::string getLocalFileName() { return localProjectFileName; }
    void setLocalFileName(std::string fileName);
    bool checkValidLocalFile(std::string fileName, std::string projectName, bool ensureProjectExists, std::string& errorMessage);

private:
    enum operationType { OPERATION_NONE, OPERATION_IMPORT, OPERATION_GET, OPERATION_COMMIT};

    ProjectManager() : currentOperation(OPERATION_NONE), includeResults(false), includeParametricResults(false) {};
    ~ProjectManager() {};

    std::string                getBaseProjectName(const std::string& cstFileName);
    std::string                createCacheFolder(const std::string& baseProjectName);
    std::list<std::string>     determineUploadFiles(const std::string& baseProjectName, bool incResults);
    void                       uploadFiles(const std::string& projectRoot, std::list<std::string>& uploadFileList, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList);
    void                       commitNewVersion(const std::string& changeMessage);
    void                       copyCacheFiles(const std::string& baseProjectName, const std::string& newVersion, const std::string& cacheFolderName, bool copyResults);
    void                       writeVersionFile(const std::string& baseProjectName, const std::string &projectName, const std::string& newVersion, const std::string& cacheFolderName);
    void                       sendUnitsInformation(const std::string& projectRoot);
    void                       sendMaterialInformation(const std::string& projectRoot);
    void                       sendParameterInformation(const std::string& projectRoot);
    void                       sendShapeInformationAndTriangulation(const std::string& projectRoot, InfoFileManager& infoFileManager);
    void                       readFileContent(const std::string& fileName, std::string& content);
    std::map<std::string, int> determineAllShapes(std::stringstream fileContent);
    void                       sendTriangulations(const std::string& projectRoot, std::map<std::string, int> trianglesMap, InfoFileManager& infoFileManager);
    void                       sendTriangleLists(std::list<std::string>& shapeNames, std::list<std::string>& shapeTriangles, std::list<std::string>& shapeHash);
    void                       deleteLocalProjectFiles(const std::string& baseProjectName);
    bool                       restoreFromCache(const std::string& baseProjectName, const std::string& cacheFolderName, const std::string& version);
    bool                       downloadFile(const std::string& cacheFolderVersion, ot::UID entityID, ot::UID version);
    void                       copyFile(const std::string& sourceFile, const std::string& destFile);
    void                       copyDirectory(const std::string& sourceDir, const std::string& destDir);
    void                       getCacheFileWriteTimes(const std::string& cacheFolderName, std::map<std::string, std::filesystem::file_time_type>& cacheFileWriteTimes, std::map<std::string, bool>& cacheFiles);
    bool                       fileContentDiffers(const std::string& file1, const std::string& file2);
    std::string                calculateHash(const std::string& fileContent);
    void                       send1dResultData(const std::string& projectRoot, InfoFileManager& infoFileManager);
    std::list<int>             getAllRunIds(const std::string& uploadDirectory);
    std::list<int>             checkForChangedData(std::list<int>& allRunIds, const std::string& uploadDirectory, Result1DFileManager& result1DData, InfoFileManager& infoFileManager);

    std::list<std::string> uploadFileList;
    std::string projectName;
    std::string baseProjectName;
    std::string cacheFolderName;
    std::map<std::string, std::pair<ot::UID, ot::UID>> newOrModifiedFiles;
    std::map<std::string, std::pair<ot::UID, ot::UID>> dependentDataFiles;
    std::list<std::string> deletedFiles;
    std::string changeMessage;
    bool includeResults;
    bool includeParametricResults;
    operationType currentOperation;
    std::string localProjectFileName;
};
