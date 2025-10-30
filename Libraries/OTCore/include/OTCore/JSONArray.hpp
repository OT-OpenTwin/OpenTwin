// @otlicense
// File: JSONArray.hpp
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
#include "OTCore/JSONArray.h"
#include "OTCore/JSONString.h"

inline ot::JsonArray::JsonArray() : JsonValue(rapidjson::kArrayType) {};

inline ot::JsonArray::JsonArray(const std::list<bool>& _lst, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::vector<bool>& _vec, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::list<int16_t>& _lst, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::vector<int16_t>& _vec, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::list<uint16_t>& _lst, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::vector<uint16_t>& _vec, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::list<int32_t>& _lst, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::vector<int32_t>& _vec, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::list<uint32_t>& _lst, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::vector<uint32_t>& _vec, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::list<int64_t>& _lst, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::vector<int64_t>& _vec, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::list<uint64_t>& _lst, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::vector<uint64_t>& _vec, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::list<float>& _lst, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::vector<float>& _vec, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::list<double>& _lst, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::vector<double>& _vec, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::list<std::string>& _lst, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (const auto& itm : _lst) {
		this->PushBack(JsonString(itm, _allocator), _allocator);
	}
}

inline ot::JsonArray::JsonArray(const std::vector<std::string>& _vec, JsonAllocator& _allocator)
	: JsonValue(rapidjson::kArrayType) {
	for (const auto& itm : _vec) {
		this->PushBack(JsonString(itm, _allocator), _allocator);
	}
}
