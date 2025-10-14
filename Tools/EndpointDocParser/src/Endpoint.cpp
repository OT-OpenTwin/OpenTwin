// Open Twin header
#include "OTCore/LogDispatcher.h"

// project header
#include "Endpoint.h"

// std header
#include <iostream>

Endpoint::Endpoint()
    : m_messageType(MessageType::mTLS) {
}

void Endpoint::printDetailedDescription() const {
    for (const std::string& line : m_detailedDescription) {
        OT_LOG_D(line);
    }
}

std::string Endpoint::getMessageTypeString() const {
    switch (m_messageType) {
        case mTLS:    return "mTLS";
        case TLS:     return "TLS";
        default:      return "Unknown";
    }
}

void Endpoint::printResponseDescription() const {
    for (const std::string& line : m_responseDescription) {
        OT_LOG_D(line);
    }
}

void Endpoint::printEndpoint() const {
    OT_LOG_D("Printing Endpoint: ");
    OT_LOG_D("================== ");
    OT_LOG_D("Endpoint name: " + m_name);
    OT_LOG_D("Endpoint action: " + m_action);
    OT_LOG_D("Brief description: " + m_briefDescription);
    OT_LOG_D("Detailed description: ");
    printDetailedDescription();
    OT_LOG_D("Message type: " + getMessageTypeString());
    
    OT_LOG_D("Parameters:");
    OT_LOG_D("----------- ");
    for (const Parameter& param : m_parameters) {
        param.printParameter();
    }
    
    OT_LOG_D("Response parameters:");
    OT_LOG_D("-------------------- ");
    for (const Parameter& rparam : m_responseParameters) {
        rparam.printParameter();
    }

    OT_LOG_D("Response description:");
    printResponseDescription();
    OT_LOG_D("\n");
}
