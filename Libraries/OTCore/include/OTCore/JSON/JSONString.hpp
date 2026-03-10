// @otlicense
// File: JSONString.hpp
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

#pragma once

// OpenTwin header
#include "OTCore/JSON/JSONString.h"

inline ot::JsonString::JsonString(const char* _cstr, JsonAllocator& _allocator) :
	JsonValue(_cstr, _allocator) 
{}

inline ot::JsonString::JsonString(const std::string& _str, JsonAllocator& _allocator) :
	JsonValue(_str.c_str(), _allocator) 
{}

inline ot::JsonStringRef::JsonStringRef(const char* _cstr) : 
	JsonStringRef(_cstr, static_cast<rapidjson::SizeType>(std::strlen(_cstr))) 
{}

inline ot::JsonStringRef::JsonStringRef(const char* _cstr, uint32_t _strLen) : 
	JsonValue(_cstr, _strLen) 
{}
