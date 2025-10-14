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

