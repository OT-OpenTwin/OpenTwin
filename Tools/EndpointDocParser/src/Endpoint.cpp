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
#include "Application.h"

// std header
#include <iostream>
#include <sstream>

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

std::string Endpoint::getDetailedDescriptionFormattedForSphinx(DescriptionType _descriptionType) const {
    std::list<std::string> detailedDescription;
    std::ostringstream out;

    bool inNoteBlock = false;
    bool inWarningBlock = false;

    switch (_descriptionType) {
    case Endpoint::detailedDescription:
        detailedDescription = getDetailedDescription();
        OT_LOG_D("Converting detailed description into Sphinx format.");
        break;
    case Endpoint::detailedResponseDescription:
        detailedDescription = getResponseDescription();
        OT_LOG_D("Converting detailed response description into Sphinx format.");
        break;
    }
    
    for (const std::string& line : detailedDescription) {
        if (Application::startsWith(line, "@note")) {
            // Close any open warning block
            if (inWarningBlock) {
                out << "\n";
                inWarningBlock = false;
            }

            // Start new note block if not already in one
            if (!inNoteBlock) {
                out << ".. note::\n\n";
                inNoteBlock = true;
            }

            // Extract content after "@note "
            std::string content = line.substr(5);
            if (!content.empty() && content[0] == ' ') {
                content = content.substr(1);
            }

            // Add content with proper indentation
            if (!content.empty()) {
                out << "   " << content << "\n";
            }
            else {
                out << "\n"; // Empty line creates paragraph break within note block
            }
        }
        // Check if line starts with @warning
        else if (Application::startsWith(line, "@warning")) {
            // Close any open note block
            if (inNoteBlock) {
                out << "\n";
                inNoteBlock = false;
            }

            // Start new warning block if not already in one
            if (!inWarningBlock) {
                out << ".. warning::\n\n";
                inWarningBlock = true;
            }

            // Extract content after "@warning "
            std::string content = line.substr(8);
            if (!content.empty() && content[0] == ' ') {
                content = content.substr(1);
            }

            // Add content with proper indentation
            if (!content.empty()) {
                out << "   " << content << "\n";
            }
            else {
                out << "\n"; // Empty line creates paragraph break within warning block
            }
        }
        // Regular line (not @note or @warning)
        else {
            // Close any open blocks
            if (inNoteBlock || inWarningBlock) {
                out << "\n";
                inNoteBlock = false;
                inWarningBlock = false;
            }

            // Add regular content
            if (line.empty()) {
                out << "\n"; // Empty line creates paragraph break
            }
            else {
                out << line << "\n";
            }
        }
    }

    // Close any remaining open blocks
    if (inNoteBlock || inWarningBlock) {
        out << "\n";
    }

    return out.str();
}
