//! @file SelectionInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/SelectionInformation.h"

void ot::SelectionInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("UIDs", JsonArray(m_uids, _allocator), _allocator);
}

void ot::SelectionInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_uids = json::getUInt64List(_object, "UIDs");
}
