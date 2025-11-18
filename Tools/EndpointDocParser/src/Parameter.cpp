// @otlicense
// File: Parameter.cpp
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
#include "Parameter.h"
#include "Application.h"

// std header
#include <iostream>
#include <sstream>

Parameter::Parameter()
    : m_dataType(DataType::Unknown) {
}

std::string Parameter::getDataTypeString() const {
    switch (m_dataType) {
        case Boolean:   return "Boolean";
        case Char:      return "Char";
        case Integer:   return "Integer";
        case Float:     return "Float";
        case Double:    return "Double";
        case String:    return "String";
        case Array:     return "Array";
        case Object:    return "Object";
        case Enum:      return "Enum";
        case UnsignedInteger64:   return "Unsigned Integer 64";
        default:      return "Unknown";
    }
}

void Parameter::printDescription() const {
    for (const std::string& line : m_description) {
        OT_LOG_D(line);
    }
}

void Parameter::printParameter() const {
    OT_LOG_D("Printing Parameter:");
    OT_LOG_D("Parameter name: " + m_name);
    OT_LOG_D("Parameter macro: " + m_macro);
    OT_LOG_D("Parameter data type: " + getDataTypeString());
    OT_LOG_D("Parameter description:");
    printDescription();
}

std::string Parameter::getDetailedDescriptionFormattedForSphinx() const {
    std::list<std::string> detailedDescription = getDescription();
    std::ostringstream out;

    // Parameter has no detailed description
    if (detailedDescription.size() == 1) {
        const std::string& briefDescription = detailedDescription.front();

            return briefDescription;
    }

    // Parameter has a detailed description
    bool inNoteBlock = false;
    bool inWarningBlock = false;
    bool isFirstLine = true;
    bool needsSpacing = false;
    bool lastLineWasEmpty = false;
    bool paragraphBreakAdded = false;

    for (const std::string& line : detailedDescription) {
        // Check if line starts with @note
        if (Application::startsWith(line, "@note")) {
            // Close any open warning block
            if (inWarningBlock) {
                out << "\n";
                inWarningBlock = false;
                needsSpacing = true;
            }

            // Start new note block if not already in one
            if (!inNoteBlock) {
                if (needsSpacing || (!isFirstLine && !inWarningBlock)) {
                    out << "\n";  // Single spacing before note block
                }
                out << "        .. note::\n";
                inNoteBlock = true;
                needsSpacing = false;
            }

            // Extract content after "@note "
            std::string content = line.substr(5);
            if (!content.empty() && content[0] == ' ') {
                content = content.substr(1);
            }

            // Add content with proper indentation and pipe format
            if (!content.empty()) {
                out << "            | " << content << "\n";
                paragraphBreakAdded = false;
            }
            else {
                // Empty @note line creates paragraph break within note block
                out << "            |\n";
                paragraphBreakAdded = true;
            }

            lastLineWasEmpty = content.empty();
        }
        // Check if line starts with @warning
        else if (Application::startsWith(line, "@warning")) {
            // Close any open note block
            if (inNoteBlock) {
                out << "\n";
                inNoteBlock = false;
                needsSpacing = true;
            }

            // Start new warning block if not already in one
            if (!inWarningBlock) {
                if (needsSpacing || (!isFirstLine && !inNoteBlock)) {
                    out << "\n";  // Single spacing before warning block
                }
                out << "        .. warning::\n";
                inWarningBlock = true;
                needsSpacing = false;
            }

            // Extract content after "@warning "
            std::string content = line.substr(8);
            if (!content.empty() && content[0] == ' ') {
                content = content.substr(1);
            }

            // Add content with proper indentation and pipe format
            if (!content.empty()) {
                out << "            | " << content << "\n";
                paragraphBreakAdded = false;
            }
            else {
                // Empty @warning line creates paragraph break within warning block
                out << "            |\n";
                paragraphBreakAdded = true;
            }

            lastLineWasEmpty = content.empty();
        }
        // Regular line (not @note or @warning)
        else {
            // Close any open blocks
            if (inNoteBlock || inWarningBlock) {
                out << "\n";
                inNoteBlock = false;
                inWarningBlock = false;
                needsSpacing = true;
                paragraphBreakAdded = false;
            }

            // Handle empty lines as paragraph breaks
            if (line.empty()) {
                if (!isFirstLine && !lastLineWasEmpty && !paragraphBreakAdded) {
                    out << "        |\n";
                    paragraphBreakAdded = true;
                }
                lastLineWasEmpty = true;
            }
            else {
                // Don't add extra paragraph break if one was already added for the empty line
                if (lastLineWasEmpty && !isFirstLine && !needsSpacing && !paragraphBreakAdded) {
                    out << "        |\n";
                }

                if (needsSpacing && !isFirstLine) {
                    needsSpacing = false;
                }

                if (isFirstLine) {
                    out << "| " << line << "\n";
                }
                else {
                    out << "        | " << line << "\n";
                }

                lastLineWasEmpty = false;
                paragraphBreakAdded = false;
            }
        }

        isFirstLine = false;
    }

    // Remove trailing newline to avoid extra spacing in table
    std::string result = out.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}