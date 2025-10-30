// @otlicense
// File: MongoDBUpgrader.h
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
#include "MongoDBSettings.h"

class MongoDBUpgrader
{
public:
    MongoDBUpgrader(const MongoDBSettings &_settings, const std::string& _tempCfgPath);

    int checkForFeatureCompatibilityVersion(int _startVersionMajor);

    void performUpgrade(int version);
    void performUpgrade4_2To4_4();
    static const int getSupportedMaxVersion() { return maxVersion; }

private:
    static const int maxVersion = 7;
    const MongoDBSettings& m_settings;
    const std::string m_cfgPath;

    void updateServerFCV(const std::string& _version);
    void runServer(const std::string _path);
};
