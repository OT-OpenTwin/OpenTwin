#pragma once

#include <string>
#include <map>
#include <list>

#include "OTCore/CoreTypes.h"

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

    void setStudioServiceData(const std::string& studioSuiteServiceURL, QObject* mainObject);

    void importProject(const std::string& fileName, const std::string& prjName);
    void uploadFiles(std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList);
    void copyFiles(const std::string& newVersion);

private:
    ProjectManager() {};
    ~ProjectManager() {};

    std::string             getBaseProjectName(const std::string& cstFileName);
    std::string             createCacheFolder(const std::string& baseProjectName);
    std::list<std::string>  determineUploadFiles(const std::string& baseProjectName);
    void                    uploadFiles(const std::string& projectRoot, std::list<std::string>& uploadFileList, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList);
    void                    commitNewVersion(const std::string& changeMessage);
    void                    copyCacheFiles(const std::string& baseProjectName, const std::string& newVersion, const std::string& cacheFolderName);
    void                    writeVersionFile(const std::string& baseProjectName, const std::string &projectName, const std::string& newVersion, const std::string& cacheFolderName);


    std::list<std::string> uploadFileList;
    std::string projectName;
    std::string baseProjectName;
    std::string cacheFolderName;
    std::map<std::string, std::pair<ot::UID, ot::UID>> newOrModifiedFiles;
    std::map<std::string, std::pair<ot::UID, ot::UID>> dependentDataFiles;
    std::list<std::string> deletedFiles;
    std::string changeMessage;
};
