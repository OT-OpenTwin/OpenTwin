// @otlicense

// OpenTwin header
#include "OTGui/Diagram/SequenceDiaDirectCall.h"

void ot::SequenceDiaDirectCall::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	SequenceDiaAbstractCall::addToJsonObject(_jsonObject, _allocator);

	_jsonObject.AddMember("FunctionName", JsonString(m_functionName, _allocator), _allocator);
}

void ot::SequenceDiaDirectCall::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	SequenceDiaAbstractCall::setFromJsonObject(_jsonObject);

	m_functionName = json::getString(_jsonObject, "FunctionName");
}