#pragma once

#include "OTServiceFoundation/BusinessLogicHandler.h"
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

    void openProject(const std::string &fileName);
    void saveProject();
    void extractInformation();

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
    bool connectToRunningDesignEnvironmentProcess(long long pid, std::string fileName);

    bool subProcessRunning = false;
    std::string serverName;
    const std::string executableName = "PythonExecution.exe";
    std::string subprocessPath;
    std::string pythonModulePath;

    const int numberOfRetries = 3;
    const int timeoutSubprocessStart = 50000;//5 seconds
    const int timeoutServerConnect = timeoutSubprocessStart;
    const int timeoutSendingMessage = timeoutSubprocessStart;
    const int heartBeat = timeoutSubprocessStart;

    std::atomic_bool startupChecked = false;

    QProcess subProcess;
    QLocalServer server;
    QLocalSocket* socket = nullptr;
};