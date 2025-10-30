// @otlicense
// File: SharedResources.h
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

#pragma once

#include <string>


class CurrentPaths
{
public:
    static const CurrentPaths& INSTANCE()
    {
        static CurrentPaths instance;
        return instance;
    }

    const std::string& getExecutableDirectory() const { return m_executablDirectory; }
    const std::string& getMongoServerCollectionDirectory() const { return m_mongoServerCollectionDir; }
    const std::string& getMongoNewVersionInstallationExe() const { return m_mongoNewVersionInstallationExe; }
    const std::string& getMongoVersion4_2() const { return m_mongoVersion4_2; }
    const std::string& getMongoVersion4_4() const { return m_mongoVersion4_4; }
    const std::string& getMongoVersion5() const { return m_mongoVersion5; }
    const std::string& getMongoVersion6() const { return m_mongoVersion6; }
    const std::string& getMongoVersion7() const { return m_mongoVersion7; }

private:
    CurrentPaths();

    std::string m_executablDirectory = "";
    std::string m_mongoServerCollectionDir = "";
    std::string m_mongoNewVersionInstallationExe = "";
    const std::string m_mongoVersion4_2 = "4.2.25";
    const std::string m_mongoVersion4_4 = "4.4.29";
    const std::string m_mongoVersion5 = "5.0.28";
    const std::string m_mongoVersion6 = "6.0.16";
    const std::string m_mongoVersion7 = "7.0.14";

    const std::string m_mongoServerDirName = "MongoDB_Server";
    const std::string m_mongoInstallerDirName = "MongoDB_Installer";
    const std::string m_newMongoVersionInstaller = "mongodb-windows-x86_64-7.0.14-signed.msi";
};
