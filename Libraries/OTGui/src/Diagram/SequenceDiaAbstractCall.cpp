// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Diagram/SequenceDiaAbstractCall.h"

std::string ot::SequenceDiaAbstractCall::callTypeToString(CallType _type) {
	switch (_type) {
	case CallType::DirectCall: return "DirectCall";
	case CallType::SelfCall: return "SelfCall";
	default:
		OT_LOG_EAS("Unknown CallType enum value (" + std::to_string(static_cast<int>(_type)) + ")");
		return "Unknown";
	}
}

ot::SequenceDiaAbstractCall::CallType ot::SequenceDiaAbstractCall::stringToCallType(const std::string& _str) {
	if (_str == "DirectCall") return CallType::DirectCall;
	if (_str == "SelfCall") return CallType::SelfCall;
	else {
		OT_LOG_EAS("Unknown CallType string value \"" + _str + "\"");
		return CallType::DirectCall;
	}
}

void ot::SequenceDiaAbstractCall::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember(JsonString(callTypeJsonKey(), _allocator), JsonString(callTypeToString(getCallType()), _allocator), _allocator);
	_jsonObject.AddMember("Test", JsonString(m_text, _allocator), _allocator);
}

void ot::SequenceDiaAbstractCall::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	OTAssert(stringToCallType(json::getString(_jsonObject, callTypeJsonKey())) == getCallType(), "CallType mismatch when deserializing SequenceDiaAbstractCall");
	m_text = json::getString(_jsonObject, "Test");
}