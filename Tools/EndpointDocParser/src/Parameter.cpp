#include "Parameter.h"

#include <iostream>

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
