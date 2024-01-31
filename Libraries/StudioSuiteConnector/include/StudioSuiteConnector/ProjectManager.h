#pragma once

#include <string>
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
    void uploadAndCopyFiles(std::list<ot::UID> &entityIDList, std::list<ot::UID> &entityVersionList);

private:
    ProjectManager() {};
    ~ProjectManager() {};

    std::string             getBaseProjectName(const std::string& cstFileName);
    std::string             createCacheFolder(const std::string& baseProjectName);
    std::list<std::string>  determineUploadFiles(const std::string& baseProjectName);
    void                    uploadFiles(std::list<std::string>& uploadFileList, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList);
    std::string             commitNewVersion(void);
    void                    copyCacheFiles(const std::string& baseProjectName, const std::string& newVersion, const std::string& cacheFolderName);
    void                    writeVersionFile(const std::string &projectName, const std::string& newVersion, const std::string& cacheFolderName);


    std::list<std::string> uploadFileList;
    std::string projectName;
    std::string baseProjectName;
    std::string cacheFolderName;
};
