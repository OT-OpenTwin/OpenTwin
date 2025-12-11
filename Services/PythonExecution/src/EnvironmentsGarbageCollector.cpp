#include "EnvironmentsGarbageCollector.h"
#include <filesystem>
#include "OTSystem/FileSystem.h"
#include "OTSystem/DateTime.h"

#include <assert.h>
#include "PredefinedEnvironments.h"

void EnvironmentsGarbageCollector::removeEnvironmentsIfNecessary()
{
    ot::DateTime::Time currentTime = ot::DateTime::currentTimestampAsStruct();
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
                ot::DateTime::Time lastAccessTime = ot::FileSystem::getLastAccessTime(environment.string());

                if (std::stoi(currentTime.m_year) > std::stoi(lastAccessTime.m_year) || 
                    std::stoi(currentTime.m_month) > std::stoi(lastAccessTime.m_month) || 
                    std::stoi(currentTime.m_day) > std::stoi(lastAccessTime.m_day) + m_daysUntilRemoval)
                {
                    std::filesystem::remove_all(environment.string());
                }
            }
        }
    }    
}
