#include "SharedResources.h"
#include <boost/dll.hpp>
#include "WindowsUtilityFuctions.h"

CurrentPaths::CurrentPaths()
{
    //std::string executableAbsPath = boost::dll::program_location().string();
    m_executablDirectory = wuf::getExecutablePath();
    m_mongoServerCollectionDir = m_executablDirectory + "\\" + m_mongoServerDirName;
    m_mongoNewVersionInstallationExe = m_executablDirectory + "\\" + m_mongoInstallerDirName + "\\" + m_newMongoVersionInstaller;
}
