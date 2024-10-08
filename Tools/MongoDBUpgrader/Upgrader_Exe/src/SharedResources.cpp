#include "SharedResources.h"
#include <boost/dll.hpp>
#include "WindowsUtilityFuctions.h"

CurrentPaths::CurrentPaths()
{
    //std::string executableAbsPath = boost::dll::program_location().string();
    m_executablDirectory = wuf::getExecutablePath();
    size_t parentPos =m_executablDirectory.find_last_of("\\");
    std::string parentDir = m_executablDirectory.substr(0, parentPos);
    m_mongoServerCollectionDir = parentDir + "\\" + m_mongoServerDirName;
    m_mongoNewVersionInstallationExe = parentDir + "\\" + m_mongoInstallerDirName + "\\" + m_newMongoVersionInstaller;
}
