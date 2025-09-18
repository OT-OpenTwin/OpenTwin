//! @file JSONString.hpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSONString.h"

inline ot::JsonString::JsonString(const char* _cstr, JsonAllocator& _allocator) :
	JsonValue(_cstr, _allocator) {
}

inline ot::JsonString::JsonString(const std::string& _str, JsonAllocator& _allocator) :
	JsonValue(_str.c_str(), _allocator) {
}