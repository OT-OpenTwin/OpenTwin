// @otlicense

#include "OTCore/OwnerService.h"


bool ot::OwnerService::getIdFromJson(JsonValue& _object, ot::serviceID_t& _id) {
	if (!_object.HasMember(m_jsonMemberName.c_str())) {
		OT_LOG_EA("JSON object member for owner Id is missing ");
		return false;
	}

	if (!_object[m_jsonMemberName.c_str()].IsUint()) {
		OT_LOG_EA("JSON object member for owner Id has wrong format");
		return false;
	}

	_id = (ot::serviceID_t)_object[m_jsonMemberName.c_str()].GetUint();
	return true;
}

ot::OwnerService ot::OwnerService::ownerFromJson(JsonValue& _object) {
	if (!_object.HasMember(m_jsonMemberName.c_str())) {
		OT_LOG_EA("JSON object member for owner Id is missing ");
		return false;
	}

	if (!_object[m_jsonMemberName.c_str()].IsUint()) {
		OT_LOG_EA("JSON object member for owner Id has wrong format");
		return false;
	}
	return ot::OwnerService((ot::serviceID_t)_object[m_jsonMemberName.c_str()].GetUint());
}

void ot::OwnerService::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) {
	_object.AddMember(JsonValue(m_jsonMemberName.c_str(), _allocator), JsonValue(this->getId()), _allocator);
}

