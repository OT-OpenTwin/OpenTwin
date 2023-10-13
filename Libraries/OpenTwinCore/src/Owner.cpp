//! @file Owner.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/Owner.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/rJSONHelper.h"

ot::GlobalOwner& ot::GlobalOwner::instance(void) {
	static GlobalOwner g_instance;
	return g_instance;
}

bool ot::ServiceOwner::getIdFromJson(OT_rJSON_val& _object, ot::serviceID_t& _id) {
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

ot::ServiceOwner_t ot::ServiceOwner::ownerFromJson(OT_rJSON_val& _object) {
	if (!_object.HasMember(m_jsonMemberName.c_str())) {
		OT_LOG_EA("JSON object member for owner Id is missing ");
		return false;
	}

	if (!_object[m_jsonMemberName.c_str()].IsUint()) {
		OT_LOG_EA("JSON object member for owner Id has wrong format");
		return false;
	}
	return ot::ServiceOwner_t((ot::serviceID_t)_object[m_jsonMemberName.c_str()].GetUint());
}

void ot::ServiceOwner::addToJsonObject(OT_rJSON_doc& _doc, OT_rJSON_val& _object) {
	ot::rJSON::add(_doc, _object, m_jsonMemberName.c_str(), id());
}

