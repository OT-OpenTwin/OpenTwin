#include "OpenTwinCore/OwnerService.h"


bool ot::OwnerService::getIdFromJson(OT_rJSON_val& _object, ot::serviceID_t& _id) {
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

ot::OwnerService ot::OwnerService::ownerFromJson(OT_rJSON_val& _object) {
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

void ot::OwnerService::addToJsonObject(OT_rJSON_doc& _doc, OT_rJSON_val& _object) {
	ot::rJSON::add(_doc, _object, m_jsonMemberName.c_str(), getId());
}

