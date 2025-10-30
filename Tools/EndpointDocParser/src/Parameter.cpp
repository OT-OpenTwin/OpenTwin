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

// std header
#include <iostream>

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

