//! @file Owner.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/Owner.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_GlobalOwnerId "goid_"

ot::GlobalOwner& ot::GlobalOwner::instance(void) {
	static GlobalOwner g_instance;
	return g_instance;
}

bool ot::GlobalOwner::getIdFromJson(OT_rJSON_val& _object, ot::serviceID_t& _id) {
	if (!_object.HasMember(OT_JSON_MEMBER_GlobalOwnerId)) {
		OT_LOG_EA("JSON object member Global Owner Id \"" OT_JSON_MEMBER_GlobalOwnerId "\" is missing");
		return false;
	}

	if (!_object[OT_JSON_MEMBER_GlobalOwnerId].IsUint()) {
		OT_LOG_EA("JSON object member Global Owner Id \"" OT_JSON_MEMBER_GlobalOwnerId "\" invalid format");
		return false;
	}

	_id = (ot::serviceID_t)_object[OT_JSON_MEMBER_GlobalOwnerId].GetUint();
	return true;
}

ot::ServiceOwner_t ot::GlobalOwner::ownerFromJson(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_GlobalOwnerId, Uint);
	return ot::ServiceOwner_t((ot::serviceID_t)_object[OT_JSON_MEMBER_GlobalOwnerId].GetUint());
}

void ot::GlobalOwner::addToJsonObject(OT_rJSON_doc& _doc, OT_rJSON_val& _object) {
	ot::rJSON::add(_doc, _object, OT_JSON_MEMBER_GlobalOwnerId, id());
}

