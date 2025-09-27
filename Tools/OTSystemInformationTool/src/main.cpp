//! @file main.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ThisComputerInfo.h"
#include "OTCore/LogNotifierFileWriter.h"

// std header
#include <chrono>
#include <iomanip>
#include <fstream>
#include <iostream>

using namespace ot;

#define BUILD_INFO "OpenTwin System Information Tool - Build " + std::string(__DATE__) + " - " + std::string(__TIME__)

int main(int _argc, const char* _argv[]) {
    // Initialize logging
    LogDispatcher::initialize("SystemInformationTool", true);

    // Check arguments
    if (_argc != 2) {
        OT_LOG_E("Please provide the file name as the argument. Aborting...");
        return 1;
    }

    // Activate file logging
    std::string fileName(_argv[1]);
    LogNotifierFileWriter* fileLogger = nullptr;
    try {
        fileLogger = new LogNotifierFileWriter(fileName);
    }
    catch (const std::exception& _e) {
        OT_LOG_E(_e.what());
    }
    catch (...) {
        OT_LOG_E("Unknown error");
    }

    // Register file logger
    if (!fileLogger) {
        OT_LOG_E("Failed to create file logger. Aborting...");
        return 2;
    }
    LogDispatcher::instance().addReceiver(fileLogger);

    OT_LOG_I(BUILD_INFO);

    // Gather info
    std::string info = ThisComputerInfo::toInfoString(ThisComputerInfo::Environment | ThisComputerInfo::MongoDBConfig | ThisComputerInfo::MongoDBConfigFileContent);

    OT_LOG_I("Timestamp: " + ot::DateTime::currentTimestamp(ot::DateTime::RFC3339));
    OT_LOG_I("Information:\n\n" + info);

    // Finalize logging
    fileLogger->flushAndCloseStream();

    return 0;
}
