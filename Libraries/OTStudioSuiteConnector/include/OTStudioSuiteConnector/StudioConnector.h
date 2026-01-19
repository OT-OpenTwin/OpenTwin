// @otlicense
// File: StudioConnector.h
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

#include "OTCore/ReturnMessage.h"

#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qobject.h>
#include <QtCore/qprocess.h>

#include <string>
#include <atomic>
#include <chrono>

class StudioConnector
{
public:
    StudioConnector() {};
    ~StudioConnector();

    std::string searchProjectAndExtractData(const std::string& fileName, const std::string& projectRoot, bool includeResults, bool includeParametricResults);
    std::string closeProject(const std::string& fileName);
    std::string openProject(const std::string& fileName);

private:
    void startSubprocess();
    std::string findSubprocessPath();
    void initiateProcess();
    void runSubprocess();
    bool startProcess();
    void connectWithSubprocess();
    ot::ReturnMessage executeCommand(const std::string& command);
    ot::ReturnMessage send(const std::string& message);
    bool checkSubprocessResponsive(std::string& errorMessage);
    void closeSubprocess();
    bool waitForResponse();
    void getProcessErrorOccured(std::string& message);
    void getSocketErrorOccured(std::string& message);

    void determineStudioSuiteInstallation(int& version, std::string& studioPath);
    std::list<long long> getRunningDesignEnvironmentProcesses();
    std::string generateExtractScript(const std::string& studioPath, std::string fileName, std::string exportFolder, std::list<long long> studioPidList, bool includeResults, bool includeParametricResults);
    std::string generateOpenScript(const std::string& studioPath, std::string fileName);
    std::string generateCloseScript(const std::string& studioPath, std::string fileName, std::list<long long> studioPidList);

    bool m_subProcessRunning = false;
    std::string m_serverName;
    const std::string m_executableName = "PythonExecution.exe";
    std::string m_subprocessPath;
    std::string m_pythonModulePath;

    const int m_numberOfRetries = 3;
    const int m_timeoutSubprocessStart = 50000;//5 seconds
    const int m_timeoutServerConnect = m_timeoutSubprocessStart;
    const int m_timeoutSendingMessage = m_timeoutSubprocessStart;
    const int m_heartBeat = m_timeoutSubprocessStart;

    QProcess m_subProcess;
    QLocalServer m_server;
    QLocalSocket* m_socket = nullptr;
};
