// @otlicense

// OpenTwin header
#include "OTCore/JSONObject.h"
#include "OTCore/Serializable.h"

ot::JsonObject::JsonObject() : JsonValue(rapidjson::kObjectType) {}

ot::JsonObject::JsonObject(const Serializable& _serializable, JsonAllocator& _allocator) :
	JsonValue(rapidjson::kObjectType) {
	_serializable.addToJsonObject(*this, _allocator);
}

ot::JsonObject::JsonObject(const Serializable* _serializable, JsonAllocator& _allocator) :
	JsonValue(rapidjson::kObjectType) {
	OTAssertNullptr(_serializable);
	_serializable->addToJsonObject(*this, _allocator);
}
