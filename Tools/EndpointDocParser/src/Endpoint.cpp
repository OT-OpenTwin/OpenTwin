// @otlicense
// File: Endpoint.cpp
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
