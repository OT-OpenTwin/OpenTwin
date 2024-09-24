#include "WindowsServiceManager.h"
#include "WindowsUtilityFuctions.h"

#include <vector>
#include <iostream>
#include <thread>

WindowsServiceManager::WindowsServiceManager(const std::string& serviceName)
{
    std::wstring temp(serviceName.begin(), serviceName.end());
    m_scManager = OpenSCManager(nullptr, nullptr, GENERIC_ALL);
    if (!m_scManager)
    {
        auto errorCode = GetLastError();
        if (errorCode == ERROR_ACCESS_DENIED)
        {
            throw std::exception("Access denied. Please run as administrator.");
        }
        else
        {
            std::string errorMessage = wuf::getErrorMessage(errorCode);
            throw std::exception(std::string("Failed accessing the mongodb service, due to the issue: " + errorMessage).c_str());
        }
    }

    m_service = OpenService(m_scManager, L"MongoDB", GENERIC_ALL);
    if (!m_service)
    {
        std::string errorMessage = wuf::getErrorMessage(GetLastError());
        throw std::exception(std::string("Failed accessing the mongodb service, due to the issue: " + errorMessage).c_str());
    }
}

WindowsServiceManager::~WindowsServiceManager()

{
    CloseServiceHandle(m_service);
    CloseServiceHandle(m_scManager);
}

std::string WindowsServiceManager::getMongoDBServerBinPath()

{
    //First we get the properties of the service
    DWORD bytesNeeded = 0;
    QueryServiceConfig(m_service, nullptr, 0, &bytesNeeded);
    //if (GetLastError() != NO_ERROR)
    //{
    //    std::string errorMessage = getErrorMessage(GetLastError());
    //    throw std::exception(std::string("Failed reading the mongodb service config, due to the issue: " + errorMessage).c_str());
    //}

    std::vector<BYTE> buffer(bytesNeeded);
    LPQUERY_SERVICE_CONFIG serviceConfig = reinterpret_cast<LPQUERY_SERVICE_CONFIG>(buffer.data());

    if (!QueryServiceConfig(m_service, serviceConfig, bytesNeeded, &bytesNeeded))
    {
        std::string errorMessage = wuf::getErrorMessage(GetLastError());
        throw std::exception(std::string("Failed reading the mongodb service config, due to the issue: " + errorMessage).c_str());
    }

    //Now we extract the components 
    std::wstring binPathW(serviceConfig->lpBinaryPathName);
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &binPathW[0], (int)binPathW.size(), NULL, 0, NULL, NULL);
    std::string binPath(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &binPathW[0], (int)binPathW.size(), &binPath[0], size_needed, NULL, NULL);

    return binPath;
}

void WindowsServiceManager::restartService()

{
    stopService();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    startService();
}

void WindowsServiceManager::startService()

{
    SERVICE_STATUS status;

    if (!QueryServiceStatus(m_service, &status))
    {
        throw std::runtime_error("Failed to query service status");
    }
    else
    {
        while (status.dwCurrentState == SERVICE_STOP_PENDING)
        {
            if (!QueryServiceStatus(m_service, &status))
            {
                throw std::runtime_error("Failed to query service status");
            }
            else
            {
                std::cout << "Waiting for service to stop ...\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        if (status.dwCurrentState == SERVICE_STOPPED)
        {
            std::cout << "Starting service.\n";
            if (!ControlService(m_service, SERVICE_CONTROL_CONTINUE, &status))
            {
                std::string errorMessage = wuf::getErrorMessage(GetLastError());
                throw std::exception(std::string("Failed to start the mongodb service, due to the issue: " + errorMessage).c_str());
            }
        }
    }
}

void WindowsServiceManager::stopService()

{
    SERVICE_STATUS status;

    if (!QueryServiceStatus(m_service, &status))
    {
        throw std::runtime_error("Failed to query service status");
    }
    else
    {
        if (status.dwCurrentState != SERVICE_STOPPED && status.dwCurrentState != SERVICE_STOP_PENDING)
        {
            std::cout << "Stopping service.\n";
            if (!ControlService(m_service, SERVICE_CONTROL_STOP, &status))
            {
                std::string errorMessage = wuf::getErrorMessage(GetLastError());
                throw std::exception(std::string("Failed to stop the mongodb service, due to the issue: " + errorMessage).c_str());
            }

            if (status.dwCurrentState != SERVICE_STOP_PENDING && status.dwCurrentState != SERVICE_STOPPED)
            {
                throw std::exception(("Failed to close service. Current state: " + wuf::getErrorMessage(status.dwCurrentState)).c_str());
            }
        }

        while (status.dwCurrentState == SERVICE_STOP_PENDING)
        {
            if (!QueryServiceStatus(m_service, &status))
            {
                throw std::runtime_error("Failed to query service status");
            }
            else
            {
                std::cout << "Stopping of service pending ...\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
}
