// @otlicense
// File: JSONNumber.h
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
#include "OTCore/JSONTypes.h"
#include "OTCore/CoreTypes.h"

namespace ot {

	class JsonNumber : public JsonValue {
		OT_DECL_NOCOPY(JsonNumber)
		OT_DECL_DEFMOVE(JsonNumber)
		OT_DECL_NODEFAULT(JsonNumber)
	public:
		explicit JsonNumber(int32_t _value) noexcept : JsonValue(_value) {};

		explicit JsonNumber(uint32_t _value) noexcept : JsonValue(_value) {};

		explicit JsonNumber(int64_t _value) noexcept : JsonValue(_value) {};

		explicit JsonNumber(uint64_t _value) noexcept : JsonValue(_value) {};

		explicit JsonNumber(float _value) noexcept : JsonValue(_value) {};

		explicit JsonNumber(double _value) noexcept : JsonValue(_value) {};

		~JsonNumber() {};
	};

}
