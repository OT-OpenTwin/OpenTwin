// @otlicense
// File: EnvironmentsGarbageCollector.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "EnvironmentsGarbageCollector.h"
#include <filesystem>
#include "OTSystem/FileSystem.h"
#include "OTSystem/DateTime.h"

#include <assert.h>
#include "PredefinedEnvironments.h"
#include "OTCore/LogDispatcher.h"

void EnvironmentsGarbageCollector::markForCleanup()
{
    OT_LOG_D("Marking environments for cleanup");
    ot::DateTime currentTime = ot::DateTime::current();
    const std::string homePath = m_interpreterPathSettings.getEnvironmentsBasePath();
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
		// The number of environments exceeds the limit, so we check for old predefined environments to mark for removal
        for (const std::filesystem::path& environment : allEnvironments)
        {
            std::string fileName = environment.filename().string();
            if (std::find(allPredefinedEnvironments.begin(), allPredefinedEnvironments.end(), fileName) == allPredefinedEnvironments.end()
                && fileName != m_interpreterPathSettings.getCustomEnvironmentPath())
            {
				//We only consider custom environments for removal

				// Now check if the environment has not been accessed for m_daysUntilRemoval days
                ot::DateTime lastAccessTime = ot::FileSystem::getLastAccessTime(environment.string());
                if (currentTime.getYear() > lastAccessTime.getYear() ||
                    currentTime.getMonth() > lastAccessTime.getMonth() ||
                    currentTime.getDay() > lastAccessTime.getDay() + m_daysUntilRemoval)
                {
                    //std::filesystem::remove_all(environment.string());
                    if (fileName.find(m_removalMarker) == std::string::npos)
                    {
                        std::string renamedFullPath = environment.string() + m_removalMarker;
                        std::filesystem::rename(environment.string(),renamedFullPath);
                        OT_LOG_D("Marked environment for removal: " + environment.string());
                    }
                }
            }
        }
    }
}

void EnvironmentsGarbageCollector::removeMarkedEnvironments()
{
    OT_LOG_D("Removing marked environments");
    const std::string homePath = m_interpreterPathSettings.getEnvironmentsBasePath();
    std::list<std::filesystem::path> allEnvironments;
    for (const auto& entry : std::filesystem::directory_iterator(homePath))
    {
        if (entry.is_directory())
        {
            allEnvironments.push_back(entry.path());
        }
    }
    for (const std::filesystem::path& environment : allEnvironments)
    {
        std::string fileName = environment.filename().string();
        if (fileName.find(m_removalMarker) != std::string::npos)
        {
            std::filesystem::remove_all(environment.string());
            OT_LOG_D("Removed environment: " + environment.string());
        }
	}
}


void EnvironmentsGarbageCollector::run()
{
    OT_LOG_D("Started garbage collector.");
    assert(!m_interpreterPathSettings.getBinPath().empty());
    try
    {
        markForCleanup();
        removeMarkedEnvironments();
    }
    catch (std::exception& _e)
    {
        OT_LOG_E("EnvironmentsGarbageCollector crashed: " + std::string(_e.what()));
    }
}
