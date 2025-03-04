//! @file SystemServiceConfigInfo.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/ArchitectureInfo.h"
#include "OTCore/String.h"
#include "OTCore/Logger.h"
#include "OTCore/SystemServiceConfigInfo.h"

#ifdef OT_OS_WINDOWS
#include <Windows.h>
#endif // OT_OS_WINDOWS


ot::SystemServiceConfigInfo::SystemServiceConfigInfo() {

}

ot::SystemServiceConfigInfo::SystemServiceConfigInfo(const char* _serviceName) : SystemServiceConfigInfo() {
    this->loadSystemServiceConfig(_serviceName);
}

void ot::SystemServiceConfigInfo::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
    _object.AddMember("Service", JsonString(m_serviceName, _allocator), _allocator);
    _object.AddMember("Display", JsonString(m_displayName, _allocator), _allocator);
    _object.AddMember("Bin", JsonString(m_binaryPath, _allocator), _allocator);
    _object.AddMember("Cfg", JsonString(m_configPath, _allocator), _allocator);
    _object.AddMember("Ser", JsonString(m_servicePath, _allocator), _allocator);
    _object.AddMember("Type", JsonString(m_startType, _allocator), _allocator);
}

void ot::SystemServiceConfigInfo::setFromJsonObject(const ot::ConstJsonObject& _object) {
    m_serviceName = json::getString(_object, "Service");
    m_displayName = json::getString(_object, "Display");
    m_binaryPath = json::getString(_object, "Bin");
    m_configPath = json::getString(_object, "Cfg");
    m_servicePath = json::getString(_object, "Ser");
    m_startType = json::getString(_object, "Type");
}

bool ot::SystemServiceConfigInfo::loadSystemServiceConfig(const char* _serviceName) {
    m_serviceName = _serviceName;

    // Open SCM
    SC_HANDLE hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!hSCManager) {
        OT_LOG_E("Failed to open SCM. Error Code: " + std::to_string(GetLastError()));
        return false;
    }

    // Open service query
    SC_HANDLE hService = OpenServiceA(hSCManager, m_serviceName.c_str(), SERVICE_QUERY_CONFIG);
    if (!hService) {
        OT_LOG_E("Failed to open service. Error Code: " + std::to_string(GetLastError()));
        CloseServiceHandle(hSCManager);
        return false;
    }

    // Configure query
    DWORD bytesNeeded;
    QueryServiceConfig(hService, nullptr, 0, &bytesNeeded);

    std::vector<BYTE> buffer(bytesNeeded);
    LPQUERY_SERVICE_CONFIG serviceConfig = reinterpret_cast<LPQUERY_SERVICE_CONFIG>(buffer.data());

    // Run query
    bool queryOk = QueryServiceConfig(hService, serviceConfig, bytesNeeded, &bytesNeeded);
    if (!queryOk) {
        OT_LOG_E("QueryServiceConfig. Error Code: " + std::to_string(GetLastError()));
    }
    else {
        // Get data
        m_displayName = (serviceConfig->lpDisplayName ? String::toString(serviceConfig->lpDisplayName) : "");
        std::string rawBinaryPath = (serviceConfig->lpBinaryPathName ? String::toString(serviceConfig->lpBinaryPathName) : "");

        // Get start type
        switch (serviceConfig->dwStartType) {
        case SERVICE_BOOT_START:   m_startType = "Boot Start (" + std::to_string(SERVICE_BOOT_START) + ")"; break;
        case SERVICE_SYSTEM_START: m_startType = "System Start (" + std::to_string(SERVICE_SYSTEM_START) + ")"; break;
        case SERVICE_AUTO_START:   m_startType = "Automatic (" + std::to_string(SERVICE_AUTO_START) + ")"; break;
        case SERVICE_DEMAND_START: m_startType = "Manual (" + std::to_string(SERVICE_DEMAND_START) + ")"; break;
        case SERVICE_DISABLED:     m_startType = "Disabled (" + std::to_string(SERVICE_DISABLED) + ")"; break;
        default:                   m_startType = "Unknown"; break;
        }

        m_binaryPath.clear();
        m_configPath.clear();
        m_servicePath.clear();

        // Split binary path
        const std::string configKeyword = "config";
        const std::string serviceKeyword = "service";
        const std::list<std::string> binaryPaths = String::split(rawBinaryPath, "--", true);

        // Try to find binary, config and service paths
        for (const std::string& binaryPath : binaryPaths) {
            // Trim string by removing unwanted characters
            std::string trimmed = String::removePrefixSuffix(binaryPath, " \t\n\"\'");

            if (trimmed.find(configKeyword) == 0) {
                if (m_configPath.empty()) {
                    m_configPath = String::removePrefixSuffix(trimmed.substr(configKeyword.length()), " \t\n\"\'");
                }
                else {
                    OT_LOG_EA("BinaryPath syntax error: Config path duplicate");
                }
            }
            else if (trimmed.find(serviceKeyword) == 0) {
                if (m_servicePath.empty()) {
                    m_servicePath = String::removePrefixSuffix(trimmed.substr(serviceKeyword.length()), " \t\n\"\'");
                }
                else {
                    OT_LOG_EA("BinaryPath syntax error: Service path duplicate");
                }
            }
            else if (m_binaryPath.empty()) {
                m_binaryPath = trimmed;
            }
            else {
                OT_LOG_EA("BinaryPath syntax error: Binary path duplicate");
            }
        }
    } // queryOk

    return queryOk;
}
