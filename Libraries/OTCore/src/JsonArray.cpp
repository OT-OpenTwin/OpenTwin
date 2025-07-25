//! @file JsonArray.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSONArray.h"
#include "OTCore/JSONString.h"

ot::JsonArray::JsonArray() : JsonValue(rapidjson::kArrayType) {};

ot::JsonArray::JsonArray(const std::list<bool>& _lst, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::vector<bool>& _vec, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::list<int32_t>& _lst, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::vector<int32_t>& _vec, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::list<uint32_t>& _lst, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::vector<uint32_t>& _vec, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::list<int64_t>& _lst, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::vector<int64_t>& _vec, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::list<uint64_t>& _lst, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::vector<uint64_t>& _vec, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::list<float>& _lst, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::vector<float>& _vec, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::list<double>& _lst, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::vector<double>& _vec, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(itm, _allocator);
	}
}

ot::JsonArray::JsonArray(const std::list<std::string>& _lst, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _lst) {
		this->PushBack(JsonString(itm, _allocator), _allocator);
	}
}

ot::JsonArray::JsonArray(const std::vector<std::string>& _vec, JsonAllocator& _allocator)
	: rapidjson::Value(rapidjson::kArrayType) {
	for (auto itm : _vec) {
		this->PushBack(JsonString(itm, _allocator), _allocator);
	}
}
