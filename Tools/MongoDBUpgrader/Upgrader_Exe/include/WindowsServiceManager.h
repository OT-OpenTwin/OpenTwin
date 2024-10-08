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
