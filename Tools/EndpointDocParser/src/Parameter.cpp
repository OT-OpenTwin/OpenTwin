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

void Parameter::printParameter() const {
    std::cout << "Printing Parameter: \n";
    std::cout << "Parameter name: " << m_name << "\n";
    std::cout << "Parameter macro: " << m_macro << "\n";
    std::cout << "Parameter data type: " << getDataTypeString() << "\n";
    std::cout << "Parameter description: " << m_description << "\n";
}

