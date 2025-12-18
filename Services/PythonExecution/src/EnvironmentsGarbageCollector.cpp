#include "EnvironmentsGarbageCollector.h"
#include <filesystem>
#include "OTSystem/FileSystem.h"
#include "OTSystem/DateTime.h"

#include <assert.h>
#include "PredefinedEnvironments.h"

void EnvironmentsGarbageCollector::removeEnvironmentsIfNecessary()
{
    ot::DateTime currentTime = ot::DateTime::current();
    const std::string homePath = "";
    std::list<std::filesystem::path> allEnvironments;
    for (const auto& entry : std::filesystem::directory_iterator(homePath)) 
    {
        if (entry.is_directory()) 
        {
            allEnvironments.push_back(entry.path());
        }
    }
    auto allPredefinedEnvironments = PredefinedEnvironments::getAll();
    auto numberOfCustomEnvironments = allEnvironments.size() - allPredefinedEnvironments.size();
    if (numberOfCustomEnvironments > m_numberOfEnvironmentsLimit)
    {
        for (const std::filesystem::path& environment: allEnvironments)
        {
            std::string fileName = environment.filename().string();
            if (std::find(allPredefinedEnvironments.begin(), allPredefinedEnvironments.end(), fileName) != allPredefinedEnvironments.end())
            {
                ot::DateTime lastAccessTime = ot::FileSystem::getLastAccessTime(environment.string());

                if (currentTime.getYear() > lastAccessTime.getYear() || 
                    currentTime.getMonth() > lastAccessTime.getMonth() ||
                    currentTime.getDay() > lastAccessTime.getDay() + m_daysUntilRemoval)
                {
                    std::filesystem::remove_all(environment.string());
                }
            }
        }
    }    
}
