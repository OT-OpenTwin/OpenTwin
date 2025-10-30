// @otlicense
// File: WindowsServiceManager.h
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
#define NOMINMAX //Supress windows macros min max
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

class WindowsServiceManager
{
public:
    WindowsServiceManager(const std::string& serviceName);
    ~WindowsServiceManager();

    std::string getMongoDBServerBinPath();

    void startService();
    void stopService();
    void restartService();
private:
    SC_HANDLE m_scManager;
    SC_HANDLE m_service;
};
